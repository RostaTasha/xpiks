/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
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

#ifndef SPELLCHECKWORKER_H
#define SPELLCHECKWORKER_H

#include <QString>
#include <QStringList>
#include <QReadWriteLock>
#include <QHash>
#include <QSet>
#include "../Common/itemprocessingworker.h"
#include "spellcheckitem.h"

class Hunspell;
class QTextCodec;

namespace SpellCheck {
    class SpellCheckWorker : public QObject, public Common::ItemProcessingWorker<SpellCheckItemBase>
    {
        Q_OBJECT
    public:
        SpellCheckWorker(QObject *parent=0);
        virtual ~SpellCheckWorker();

    public:
        QStringList retrieveCorrections(const QString &word);
        void addToUserWordlist(const QString &word);

    protected:
        virtual bool initWorker();
        virtual void processOneItem(std::shared_ptr<SpellCheckItemBase> &item);

    protected:
        virtual void notifyQueueIsEmpty() { emit queueIsEmpty(); }
        virtual void workerStopped() { emit stopped(); }

    public slots:
        void process() { doWork(); }
        void cancel() { stopWorking(); }

    signals:
        void stopped();
        void queueIsEmpty();

    private:
        void detectAffEncoding();
        QStringList suggestCorrections(const QString &word);
        bool checkWordSpelling(const std::shared_ptr<SpellCheckQueryItem> &queryItem);
        bool isHunspellSpellingCorrect(const QString &word) const;
        void findSuggestions(const QString &word);
        void putWord(const QString &word);
        void initFromUserDict();

    private:
        QHash<QString, QStringList> m_Suggestions;
        QSet<QString> m_WrongWords;
        QReadWriteLock m_SuggestionsLock;
        QString m_Encoding;
        Hunspell *m_Hunspell;
        // Coded does not need destruction
        QTextCodec *m_Codec;
        QString m_userDictionary;
    };
}

#endif // SPELLCHECKWORKER_H
