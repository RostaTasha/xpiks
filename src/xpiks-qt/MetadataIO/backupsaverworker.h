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

#ifndef BACKUPSAVERWORKER_H
#define BACKUPSAVERWORKER_H

#include <QObject>
#include "../Common/itemprocessingworker.h"
#include "saverworkerjobitem.h"

namespace MetadataIO {
    class BackupSaverWorker : public QObject, public Common::ItemProcessingWorker<SaverWorkerJobItem>
    {
        Q_OBJECT
    protected:
        virtual bool initWorker() override;
        virtual void processOneItem(std::shared_ptr<SaverWorkerJobItem> &item) override;

    protected:
        virtual void notifyQueueIsEmpty() override { emit queueIsEmpty(); }
        virtual void workerStopped() override { emit stopped(); }

    public slots:
        void process() { doWork(); }
        void cancel() { stopWorking(); }

    signals:
        void stopped();
        void queueIsEmpty();
    };
}

#endif // BACKUPSAVERWORKER_H
