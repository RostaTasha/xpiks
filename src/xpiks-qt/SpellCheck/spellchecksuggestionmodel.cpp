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

#include "spellchecksuggestionmodel.h"
#include <QQmlEngine>
#include <QHash>
#include <QString>
#include "spellsuggestionsitem.h"
#include "../Models/artworkmetadata.h"
#include "spellcheckerservice.h"
#include "../Commands/commandmanager.h"
#include "../Common/flags.h"
#include "../Common/defines.h"
#include "../Common/basickeywordsmodel.h"

namespace SpellCheck {

    std::vector<std::shared_ptr<SpellSuggestionsItem> > combineSuggestionRequests(const SuggestionsVector &items) {
        QHash<QString, SuggestionsVector > dict;

        size_t size = items.size();
        for (size_t i = 0; i < size; ++i) {
            auto &item = items.at(i);
            const QString &word = item->getWord();
            if (!dict.contains(word)) {
                dict.insert(word, SuggestionsVector());
            }

            dict[word].emplace_back(item);
        }

        SuggestionsVector result;
        result.reserve(size);

        QHash<QString, SuggestionsVector >::iterator i = dict.begin();
        QHash<QString, SuggestionsVector >::iterator end = dict.end();
        for (; i != end; ++i) {
            SuggestionsVector &vector = i.value();

            if (vector.size() > 1) {
                result.emplace_back(new CombinedSpellSuggestions(i.key(), vector));
            } else {
                result.emplace_back(vector.front());
            }
        }

        return result;
    }

    QHash<Common::IMetadataOperator *, KeywordsSuggestionsVector > combinedFailedReplacements(const SuggestionsVector &failedReplacements) {
        QHash<Common::IMetadataOperator *, KeywordsSuggestionsVector > candidatesToRemove;
        size_t size = failedReplacements.size();
        candidatesToRemove.reserve((int)size);

        for (size_t i = 0; i < size; ++i) {
            auto &item = failedReplacements.at(i);
            std::shared_ptr<KeywordSpellSuggestions> keywordsItem = std::dynamic_pointer_cast<KeywordSpellSuggestions>(item);

            if (keywordsItem) {
                auto *item = keywordsItem->getMetadataOperator();

                if (keywordsItem->isPotentialDuplicate()) {
                    if (!candidatesToRemove.contains(item)) {
                        candidatesToRemove.insert(item, KeywordsSuggestionsVector());
                    }

                    candidatesToRemove[item].emplace_back(keywordsItem);
                }
            } else {
                std::shared_ptr<CombinedSpellSuggestions> combinedItem = std::dynamic_pointer_cast<CombinedSpellSuggestions>(item);
                if (combinedItem) {
                    auto keywordsItems = combinedItem->getKeywordsDuplicateSuggestions();

                    for (auto &keywordsCombinedItem: keywordsItems) {
                        auto *item = keywordsCombinedItem->getMetadataOperator();

                        if (!candidatesToRemove.contains(item)) {
                            candidatesToRemove.insert(item, KeywordsSuggestionsVector());
                        }

                        candidatesToRemove[item].emplace_back(keywordsCombinedItem);
                    }
                } else {
                    LOG_WARNING << "Unsupported failed suggestion type";
                }
            }
        }

        return candidatesToRemove;
    }

    SpellCheckSuggestionModel::SpellCheckSuggestionModel():
        QAbstractListModel(),
        Common::BaseEntity()
    {
    }

    SpellCheckSuggestionModel::~SpellCheckSuggestionModel() {
    }

    QObject *SpellCheckSuggestionModel::getSuggestionItself(int index) const {
        SpellSuggestionsItem *item = NULL;

        if (0 <= index && index < (int)m_SuggestionsList.size()) {
            item = m_SuggestionsList.at(index).get();
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
        }

        return item;
    }

    void SpellCheckSuggestionModel::clearModel() {
        beginResetModel();
        m_SuggestionsList.clear();
        m_ItemsPairs.clear();
        endResetModel();
        emit artworksCountChanged();
    }

    void SpellCheckSuggestionModel::submitCorrections() const {
        LOG_DEBUG << "#";
        bool anyChanged = false;

        SuggestionsVector failedItems;

        for (auto &item: m_SuggestionsList) {
            if (item->anyReplacementSelected()) {
                item->replaceToSuggested();

                if (!item->getReplacementSucceeded()) {
                    failedItems.push_back(item);
                } else {
                    anyChanged = true;
                }
            }
        }

        if (processFailedReplacements(failedItems)) {
            anyChanged = true;
        }

        if (anyChanged) {
            for (auto &pair: m_ItemsPairs) {
                auto *item = pair.first;
                item->afterReplaceCallback();
                m_CommandManager->submitItemForSpellCheck(item->getBasicKeywordsModel());
            }
        }

        updateItems();
    }

    void SpellCheckSuggestionModel::resetAllSuggestions() {
        LOG_DEBUG << "#";
        for (auto &item: m_SuggestionsList) {
            item->setReplacementIndex(-1);
        }
    }

    void SpellCheckSuggestionModel::setupModel(Common::IMetadataOperator *item, int index, Common::SuggestionFlags flags) {
        Q_ASSERT(item != NULL);
        LOG_DEBUG << "#";

        std::vector<std::pair<Common::IMetadataOperator *, int> > items;
        items.emplace_back(item, index);
        this->setupModel(items, flags);
    }

    void SpellCheckSuggestionModel::setupModel(std::vector<std::pair<Common::IMetadataOperator *, int> > &items, Common::SuggestionFlags flags) {
        LOG_INFO << "flags =" << (int)flags;
        m_ItemsPairs = std::move(items);

        auto requests = createSuggestionsRequests(flags);

        auto combinedRequests = combineSuggestionRequests(requests);
        LOG_INFO << combinedRequests.size() << "combined request(s)";

        auto executedRequests = setupSuggestions(combinedRequests);
        LOG_INFO << executedRequests.size() << "executed request(s)";

#if defined(CORE_TESTS) || defined(INTEGRATION_TESTS)
        for (auto &executedItem: executedRequests) {
            LOG_INFO << executedItem->toDebugString();
        }
#endif

        beginResetModel();
        m_SuggestionsList.clear();
        m_SuggestionsList = executedRequests;
        endResetModel();

        emit artworksCountChanged();
    }

    SuggestionsVector SpellCheckSuggestionModel::createSuggestionsRequests(Common::SuggestionFlags flags) {
        SuggestionsVector requests;

        using namespace Common;

        if (Common::HasFlag(flags, SuggestionFlags::Keywords)) {
            for (auto &pair: m_ItemsPairs) {
                auto *item = pair.first;
                auto subrequests = item->createKeywordsSuggestionsList();
                for (auto &req: subrequests) { req->setMetadataOperator(item); }
                requests.insert(requests.end(), subrequests.begin(), subrequests.end());
                LOG_DEBUG << subrequests.size() << "keywords requests";
            }
        }

        if (Common::HasFlag(flags, SuggestionFlags::Title)) {
            for (auto &pair: m_ItemsPairs) {
                auto *item = pair.first;
                auto subrequests = item->createTitleSuggestionsList();
                for (auto &req: subrequests) { req->setMetadataOperator(item); }
                requests.insert(requests.end(), subrequests.begin(), subrequests.end());
                LOG_DEBUG << subrequests.size() << "title requests";
            }
        }

        if (Common::HasFlag(flags, SuggestionFlags::Description)) {
            for (auto &pair: m_ItemsPairs) {
                auto *item = pair.first;
                auto subrequests = item->createDescriptionSuggestionsList();
                for (auto &req: subrequests) { req->setMetadataOperator(item); }
                requests.insert(requests.end(), subrequests.begin(), subrequests.end());
                LOG_DEBUG << subrequests.size() << "description requests";
            }
        }

        return requests;
    }

    bool SpellCheckSuggestionModel::processFailedReplacements(const SuggestionsVector &failedReplacements) const {
        LOG_INFO << failedReplacements.size() << "failed items";

        auto candidatesToRemove = combinedFailedReplacements(failedReplacements);

        auto it = candidatesToRemove.begin();
        auto itEnd = candidatesToRemove.end();

        bool anyReplaced = false;
        for (; it != itEnd; ++it) {
            auto *item = it.key();

            if (item->processFailedKeywordReplacements(it.value())) {
                anyReplaced = true;
            }
        }

        return anyReplaced;
    }

    SuggestionsVector SpellCheckSuggestionModel::setupSuggestions(const SuggestionsVector &items) {
        LOG_INFO << items.size() << "item(s)";
        SpellCheckerService *service = m_CommandManager->getSpellCheckerService();
        // another vector for requests with available suggestions
        SuggestionsVector executedRequests;
        executedRequests.reserve(items.size());

        for (auto &item: items) {
            QStringList suggestions = service->suggestCorrections(item->getWord());
            if (!suggestions.isEmpty()) {
                item->setSuggestions(suggestions);
                executedRequests.push_back(item);
            }
        }

        return executedRequests;
    }

    int SpellCheckSuggestionModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)m_SuggestionsList.size();
    }

    QVariant SpellCheckSuggestionModel::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if (row < 0 || row >= (int)m_SuggestionsList.size()) { return QVariant(); }

        auto &item = m_SuggestionsList.at(row);

        switch (role) {
        case WordRole:
            return item->getWord();
        case ReplacementIndexRole:
            return item->getReplacementIndex();
        case ReplacementOriginRole:
            return item->getReplacementOrigin();
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> SpellCheckSuggestionModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[WordRole] = "word";
        roles[ReplacementIndexRole] = "replacementindex";
        roles[ReplacementOriginRole] = "replacementorigin";
        return roles;
    }

    void SpellCheckSuggestionModel::updateItems() const {
        QVector<int> indices;
        indices.reserve((int)m_ItemsPairs.size());

        for (auto &pair: m_ItemsPairs) {
            int index = pair.second;
            if (index != -1) {
                indices.push_back(index);
            }
        }

        m_CommandManager->updateArtworks(indices);
    }
}
