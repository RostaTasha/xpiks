/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QString>
#include <QThread>
#include "shutterstockqueryengine.h"
#include "suggestionartwork.h"
#include "keywordssuggestor.h"
#include "../Encryption/aes-qt.h"
#include "libraryqueryworker.h"
#include "locallibrary.h"
#include "../Conectivity/simplecurlrequest.h"
#include "../Common/defines.h"
#include "../Models/settingsmodel.h"

namespace Suggestion {
    ShutterstockQueryEngine::ShutterstockQueryEngine(int engineID, Models::SettingsModel *settingsModel):
        SuggestionQueryEngineBase(engineID, settingsModel)
    {
        m_ClientId = "28a2a9b917961a0cbc343c81b2dd0f6618377f9210aa3182e5cc9f5588f914d918ede1533c9e06b91769c89e80909743";
        m_ClientSecret = "5092d9a967c2f19b57aac29bc09ac3b9e6ae5baec1a371331b73ff24f1625d95c4f3fef90bdacfbe9b0b3803b48c269192bc55f14bb9c2b5a16d650cd641b746eb384fcf9dbd53a96f1f81215921b04409f3635ecf846ffdf01ee04ba76624c9";
    }

    void ShutterstockQueryEngine::submitQuery(const QStringList &queryKeywords, QueryResultsType resultsType) {
        LOG_INFO << queryKeywords;
        QUrl url = buildQuery(queryKeywords, resultsType);

        QString decodedClientId = Encryption::decodeText(m_ClientId, "MasterPassword");
        QString decodedClientSecret = Encryption::decodeText(m_ClientSecret, "MasterPassword");

        QString authStr = QString("%1:%2").arg(decodedClientId).arg(decodedClientSecret);
        QString headerData = "Basic " + QString::fromLatin1(authStr.toLocal8Bit().toBase64());

        auto *settings = getSettingsModel();
        auto *proxySettings = settings->retrieveProxySettings();

        QString resourceUrl = QString::fromLocal8Bit(url.toEncoded());
        Conectivity::SimpleCurlRequest *request = new Conectivity::SimpleCurlRequest(resourceUrl);
        request->setRawHeaders(QStringList() << "Authorization: " + headerData);
        request->setProxySettings(proxySettings);

        QThread *thread = new QThread();

        request->moveToThread(thread);

        QObject::connect(thread, SIGNAL(started()), request, SLOT(process()));
        QObject::connect(request, SIGNAL(stopped()), thread, SLOT(quit()));

        QObject::connect(request, SIGNAL(stopped()), request, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        QObject::connect(request, SIGNAL(requestFinished(bool)), this, SLOT(requestFinishedHandler(bool)));

        thread->start();
    }

    void ShutterstockQueryEngine::requestFinishedHandler(bool success) {
        LOG_INFO << "success:" << success;

        Conectivity::SimpleCurlRequest *request = qobject_cast<Conectivity::SimpleCurlRequest *>(sender());

        if (success) {
            QJsonParseError error;

            QJsonDocument document = QJsonDocument::fromJson(request->getResponseData(), &error);

            if (error.error == QJsonParseError::NoError) {
                QJsonObject jsonObject = document.object();
                QJsonValue dataValue = jsonObject["data"];

                if (dataValue.isArray()) {
                    std::vector<std::shared_ptr<SuggestionArtwork> > suggestionArtworks;
                    parseResponse(dataValue.toArray(), suggestionArtworks);
                    setResults(suggestionArtworks);
                    emit resultsAvailable();
                }
            } else {
                LOG_WARNING << "parsing error:" << error.errorString();
                emit errorReceived(tr("Can't parse the response"));
            }
        } else {
            LOG_WARNING << "error:" << request->getErrorString();
            emit errorReceived(request->getErrorString());
        }

        request->dispose();
    }

    void ShutterstockQueryEngine::parseResponse(const QJsonArray &jsonArray,
                                                std::vector<std::shared_ptr<SuggestionArtwork> > &suggestionArtworks) {
        LOG_DEBUG << "#";
        foreach (const QJsonValue &value, jsonArray) {
            QJsonObject imageResult = value.toObject();

            if (imageResult.contains("assets")) {
                QJsonObject assetsObject = imageResult["assets"].toObject();
                if (assetsObject.contains("large_thumb")) {
                    QJsonObject thumb = assetsObject["large_thumb"].toObject();
                    if (thumb.contains("url")) {
                        QString url = thumb["url"].toString();
                        QStringList keywordsList;

                        if (imageResult["keywords"].isArray()) {
                            foreach (const QJsonValue &keyword, imageResult["keywords"].toArray()) {
                                keywordsList.append(keyword.toString());
                            }
                        }

                        QString externalUrl;
                        if (imageResult.contains("id")) {
                            externalUrl = QString("https://www.shutterstock.com/pic-%1.html").arg(imageResult["id"].toString());
                        }

                        QString description;
                        if (imageResult.contains("description")) {
                            description = imageResult["description"].toString();
                        }

                        suggestionArtworks.emplace_back(new SuggestionArtwork(url, externalUrl, QString(""), description, keywordsList));
                    }
                }
            }
        }
    }

    QUrl ShutterstockQueryEngine::buildQuery(const QStringList &queryKeywords, QueryResultsType resultsType) const {
        QUrlQuery urlQuery;

        urlQuery.addQueryItem("language", "en");
        urlQuery.addQueryItem("view", "full");
        urlQuery.addQueryItem("per_page", "100");
        urlQuery.addQueryItem("sort", "popular");
        urlQuery.addQueryItem("query", queryKeywords.join(' '));

        if (resultsType != QueryResultsType::AllImages) {
            urlQuery.addQueryItem("image_type", resultsTypeToString(resultsType));
        }

        QUrl url;
        url.setUrl(QLatin1String("https://api.shutterstock.com/v2/images/search"));
        url.setQuery(urlQuery);
        return url;
    }

    QString ShutterstockQueryEngine::resultsTypeToString(QueryResultsType resultsType) const {
        switch (resultsType) {
        case QueryResultsType::Photos: return QLatin1String("photo");
        case QueryResultsType::Vectors: return QLatin1String("vector");
        case QueryResultsType::Illustrations: return QLatin1String("illustration");
        default: return QString();
        }
    }
}

