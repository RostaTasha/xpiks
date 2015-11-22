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

#include "keywordspellsuggestions.h"

namespace SpellCheck {
    KeywordSpellSuggestions::KeywordSpellSuggestions(const QString &keyword, int originalIndex) :
        m_Word(keyword),
        m_ReplacementIndex(0),
        m_OriginalIndex(originalIndex),
        m_IsSelected(true)
    {
    }

    bool KeywordSpellSuggestions::setReplacementIndex(int value) {
        bool result = value != m_ReplacementIndex;

        if (result) {
            QModelIndex prev = this->index(m_ReplacementIndex);
            QModelIndex curr = this->index(value);
            m_ReplacementIndex = value;
            QVector<int> roles;
            roles << IsSelectedRole;
            emit dataChanged(prev, prev, roles);
            emit dataChanged(curr, curr, roles);
        }

        return result;
    }

    void KeywordSpellSuggestions::setSuggestions(const QStringList &suggestions) {
        beginResetModel();
        m_Suggestions.clear();
        m_Suggestions.append(suggestions);
        m_ReplacementIndex = 0;
        endResetModel();
    }

    int KeywordSpellSuggestions::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_Suggestions.length();
    }

    QVariant KeywordSpellSuggestions::data(const QModelIndex &index, int role) const {
        if (!index.isValid()) return QVariant();

        int row = index.row();

        switch (role) {
        case SuggestionRole:
            return m_Suggestions.at(row);
        case IsSelectedRole:
            return (m_ReplacementIndex == row);
        default:
            return QVariant();
        }
    }

    bool KeywordSpellSuggestions::setData(const QModelIndex &index, const QVariant &value, int role) {
        if (!index.isValid()) return false;

        bool result = false;

        switch (role) {
        case EditReplacementIndexRole:
            result = setReplacementIndex(value.toInt());
            break;
        default:
            return false;
        }

        return result;
    }

    QHash<int, QByteArray> KeywordSpellSuggestions::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[SuggestionRole] = "suggestion";
        roles[IsSelectedRole] = "isselected";
        roles[EditReplacementIndexRole] = "editreplacementindex";
        return roles;
    }
}