/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef BASICMETADATAMODEL_H
#define BASICMETADATAMODEL_H

#include "basickeywordsmodel.h"
#include <QStringList>
#include <QHash>
#include <QReadWriteLock>
#include "../Common/flags.h"

namespace SpellCheck {
    class SpellCheckQueryItem;
    class KeywordSpellSuggestions;
    class SpellCheckItem;
    class SpellCheckItemInfo;
}

namespace Common {
    class BasicMetadataModel :
            public BasicKeywordsModel,
            public Common::IMetadataOperator
    {
        Q_OBJECT
    public:
        BasicMetadataModel(Common::Hold &hold, QObject *parent=0);

    public:
        void setSpellCheckInfo(SpellCheck::SpellCheckItemInfo *info) { m_SpellCheckInfo = info; }
        SpellCheck::SpellCheckItemInfo *getSpellCheckInfo() const { return m_SpellCheckInfo; }
        QString getDescription();
        QString getTitle();

    public:
#ifdef CORE_TESTS
        void initialize(const QString &title, const QString &description, const QString &rawKeywords);
#endif

    public:
        virtual void setSpellCheckResults(const QHash<QString, Common::WordAnalysisResult> &results, SpellCheckFlags flags);
        virtual std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > createDescriptionSuggestionsList() override;
        virtual std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > createTitleSuggestionsList() override;
        virtual bool fixDescriptionSpelling(const QString &word, const QString &replacement) override;
        virtual bool fixTitleSpelling(const QString &word, const QString &replacement) override;
        virtual void setKeywordsSpellCheckResults(const std::vector<std::shared_ptr<SpellCheck::SpellCheckQueryItem> > &items) override;
        virtual bool processFailedKeywordReplacements(const std::vector<std::shared_ptr<SpellCheck::KeywordSpellSuggestions> > &candidatesForRemoval) override;
        virtual std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > createKeywordsSuggestionsList() override;
        virtual Common::KeywordReplaceResult fixKeywordSpelling(int index, const QString &existing, const QString &replacement) override;
        virtual void afterReplaceCallback() override;
        virtual Common::BasicKeywordsModel *getBasicKeywordsModel() override;
        virtual QStringList getDescriptionWords();
        virtual QStringList getTitleWords();
        virtual bool expandPreset(int keywordIndex, const QStringList &presetList) override;
        virtual bool appendPreset(const QStringList &presetList) override;
        virtual const QHash<QString, QStringList> getDuplicatesModel() override;

    private:
        bool replaceInDescription(const QString &replaceWhat, const QString &replaceTo,
                                  Common::SearchFlags flags);
        bool replaceInTitle(const QString &replaceWhat, const QString &replaceTo,
                            SearchFlags flags);

    public:
        virtual bool replace(const QString &replaceWhat, const QString &replaceTo, Common::SearchFlags flags) override;
        bool hasDescriptionSpellError();
        bool hasTitleSpellError();
        bool hasDescriptionWordSpellError(const QString &word);
        bool hasTitleWordSpellError(const QString &word);
        virtual bool hasSpellErrors() override;

    public:
        // IMetadataOperator
        // c++ is still not capable of picking abstract implementations from Base class
        virtual bool editKeyword(int index, const QString &replacement) override { return BasicKeywordsModel::editKeyword(index, replacement); }
        virtual bool removeKeywordAt(int index, QString &removedKeyword) override { return BasicKeywordsModel::removeKeywordAt(index, removedKeyword); }
        virtual bool removeLastKeyword(QString &removedKeyword) override { return BasicKeywordsModel::removeLastKeyword(removedKeyword); }
        virtual bool appendKeyword(const QString &keyword) override { return BasicKeywordsModel::appendKeyword(keyword); }
        virtual int appendKeywords(const QStringList &keywordsList) override { return BasicKeywordsModel::appendKeywords(keywordsList); }
        virtual bool clearKeywords() override { return BasicKeywordsModel::clearKeywords(); }
        virtual QString getKeywordsString() override { return BasicKeywordsModel::getKeywordsString(); }
        virtual void setKeywords(const QStringList &keywords) override { return BasicKeywordsModel::setKeywords(keywords); }
        virtual bool removeKeywords(const QSet<QString> &keywords, bool caseSensitive) override { return BasicKeywordsModel::removeKeywords(keywords, caseSensitive); }
        virtual bool hasKeywords(const QStringList &keywordsList) override { return BasicKeywordsModel::hasKeywords(keywordsList); }
        virtual void requestBackup() override { /* bump */ }

    public:
        virtual bool setDescription(const QString &value) override;
        virtual bool setTitle(const QString &value) override;
        virtual bool isEmpty() override;
        bool isTitleEmpty();
        bool isDescriptionEmpty();
        void clearModel();

    public:
        void notifyDescriptionSpellCheck();
        void notifyTitleSpellCheck();

    private:
        void updateDescriptionSpellErrors(const QHash<QString, Common::WordAnalysisResult> &results);
        void updateTitleSpellErrors(const QHash<QString, Common::WordAnalysisResult> &results);

    private:
        QReadWriteLock m_DescriptionLock;
        QReadWriteLock m_TitleLock;
        SpellCheck::SpellCheckItemInfo *m_SpellCheckInfo;
        QString m_Description;
        QString m_Title;
    };
}

#endif // BASICMETADATAMODEL_H
