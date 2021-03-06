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

#include "updateservice.h"
#include "updatescheckerworker.h"
#include <QFile>
#include <QString>
#include "../Common/defines.h"
#include "../Models/settingsmodel.h"
#include "../Helpers/appsettings.h"
#include "../Common/version.h"

namespace Conectivity {
    UpdateService::UpdateService(Models::SettingsModel *settingsModel):
        m_UpdatesCheckerWorker(nullptr),
        m_SettingsModel(settingsModel),
        m_AvailableVersion(0),
        m_UpdateAvailable(false)
    {
        Q_ASSERT(settingsModel != nullptr);
    }

    void UpdateService::startChecking() {
        const bool startWorker = m_SettingsModel->getCheckForUpdates();

        if (startWorker) {
            updateSettings();
            doStartChecking();
        } else {
            LOG_INFO << "Update checking disabled";
        }
    }

    void UpdateService::stopChecking() {
        LOG_DEBUG << "#";
        emit cancelRequested();
    }

    void UpdateService::doStartChecking() {
        LOG_DEBUG << "#";

        m_UpdatesCheckerWorker = new UpdatesCheckerWorker(m_SettingsModel, m_PathToUpdate);
        QThread *thread = new QThread();
        m_UpdatesCheckerWorker->moveToThread(thread);

        QObject::connect(thread, SIGNAL(started()), m_UpdatesCheckerWorker, SLOT(process()));
        QObject::connect(m_UpdatesCheckerWorker, SIGNAL(stopped()), thread, SLOT(quit()));

        QObject::connect(m_UpdatesCheckerWorker, SIGNAL(stopped()), m_UpdatesCheckerWorker, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        QObject::connect(this, SIGNAL(cancelRequested()),
                         m_UpdatesCheckerWorker, SIGNAL(cancelRequested()));

        QObject::connect(m_UpdatesCheckerWorker, SIGNAL(updateAvailable(QString)),
                         this, SIGNAL(updateAvailable(QString)));
        QObject::connect(m_UpdatesCheckerWorker, SIGNAL(updateDownloaded(QString, int)),
                         this, SLOT(updateDownloadedHandler(QString, int)));

        QObject::connect(m_UpdatesCheckerWorker, SIGNAL(stopped()),
                         this, SLOT(workerFinished()));

        thread->start();
    }

    void UpdateService::updateSettings() {
        LOG_DEBUG << "#";
        Helpers::AppSettings appSettings;

        int availableValue = appSettings.intValue(appSettings.getAvailableUpdateVersionKey(), 0);

        if ((0 < availableValue) && (availableValue <= XPIKS_VERSION_INT)) {
            LOG_DEBUG << "Flushing available update settings values";
            appSettings.setValue(appSettings.getPathToUpdateKey(), "");
            appSettings.setValue(appSettings.getAvailableUpdateVersionKey(), 0);
        } else {
            m_PathToUpdate = appSettings.value(appSettings.getPathToUpdateKey()).toString();
            m_AvailableVersion = appSettings.intValue(appSettings.getAvailableUpdateVersionKey(), 0);
            LOG_INFO << "Available:" << m_PathToUpdate << "version:" << m_AvailableVersion;
        }
    }

    void UpdateService::workerFinished() {
        LOG_DEBUG << "#";
    }

    void UpdateService::updateDownloadedHandler(const QString &updatePath, int version) {
        LOG_DEBUG << "#";
        m_UpdateAvailable = true;
        m_PathToUpdate = updatePath;
        m_AvailableVersion = version;

        saveUpdateInfo();

        emit updateDownloaded(m_PathToUpdate);
    }

    void UpdateService::saveUpdateInfo() const {
        Q_ASSERT(m_UpdateAvailable);

        Helpers::AppSettings appSettings;
        appSettings.setValue(appSettings.getAvailableUpdateVersionKey(), m_AvailableVersion);
        appSettings.setValue(appSettings.getPathToUpdateKey(), m_PathToUpdate);
    }
}
