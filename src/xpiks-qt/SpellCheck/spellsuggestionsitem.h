/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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

#ifndef KEYWORDSPELLSUGGESTIONS_H
#define KEYWORDSPELLSUGGESTIONS_H

#include <QString>
#include <QStringList>
#include <QAbstractListModel>

namespace SpellCheck {
    class ISpellCheckable;

    class SpellSuggestionsItem: public QAbstractListModel {
        Q_OBJECT
    public:
        SpellSuggestionsItem(const QString &word, const QString &origin);
        SpellSuggestionsItem(const QString &word);
    public:
        enum KeywordSpellSuggestionsRoles {
            SuggestionRole = Qt::UserRole + 1,
            // not same as inner m_IsSelected
            // used for selected replacement
            IsSelectedRole,
            EditReplacementIndexRole
        };

    public:
        const QString &getWord() const { return m_Word; }
        int getReplacementIndex() const { return m_ReplacementIndex; }

        bool setReplacementIndex(int value);

        const QString &getReplacement() const { return m_Suggestions.at(m_ReplacementIndex); }
        void setSuggestions(const QStringList &suggestions);

        const QString &getReplacementOrigin() const { return m_ReplacementOrigin; }

        bool getIsSelected() const { return m_IsSelected; }
        void setIsSelected(bool value) { m_IsSelected = value; }

    public:
        virtual void replaceToSuggested(ISpellCheckable *item) = 0;

    signals:
        void replacementIndexChanged();

    public:
        virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

    protected:
        virtual QHash<int, QByteArray> roleNames() const;

    private:
        QStringList m_Suggestions;
        QString m_Word;
        QString m_ReplacementOrigin;
        int m_ReplacementIndex;
        bool m_IsSelected;
    };

    class KeywordSpellSuggestions: public SpellSuggestionsItem
    {
    public:
        KeywordSpellSuggestions(const QString &keyword, int originalIndex, const QString &origin);
        KeywordSpellSuggestions(const QString &keyword, int originalIndex);

    public:
        int getOriginalIndex() const { return m_OriginalIndex; }
        virtual void replaceToSuggested(ISpellCheckable *item);

    private:
        int m_OriginalIndex;
    };

    class DescriptionSpellSuggestions: public SpellSuggestionsItem
    {
    public:
        DescriptionSpellSuggestions(const QString &word);

    public:
        virtual void replaceToSuggested(ISpellCheckable *item);
    };

    class TitleSpellSuggestions: public SpellSuggestionsItem
    {
    public:
        TitleSpellSuggestions(const QString &word);

    public:
        virtual void replaceToSuggested(ISpellCheckable *item);
    };
}

Q_DECLARE_METATYPE(SpellCheck::KeywordSpellSuggestions*)

#endif // KEYWORDSPELLSUGGESTIONS_H