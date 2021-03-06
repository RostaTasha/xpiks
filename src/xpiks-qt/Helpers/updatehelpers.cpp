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

#include "updatehelpers.h"
#include <QProcess>
#include <QStringList>
#include <QDir>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDirIterator>
#include <QFile>
#include <QtConcurrent>
#include "../Common/defines.h"

#ifdef Q_OS_OSX

void launchOSXdmg(const QString &dmgPath) {
    QStringList arguments;
    arguments << dmgPath;

    QProcess::startDetached("open", arguments);
}

#endif

#ifdef Q_OS_WIN

QString getLogFilenameForMinistaller() {
    QString appDataPath = XPIKS_USERDATA_PATH;
    if (appDataPath.isEmpty()) {
        appDataPath = QDir::currentPath();
    }

    const QString &logFileDir = QDir::cleanPath(appDataPath + QDir::separator() + "logs");
    QDir logsDir(logFileDir);
    Q_ASSERT(logsDir.exists());

    QString time = QDateTime::currentDateTimeUtc().toString("ddMMyyyy-hhmmss-zzz");
    QString logFilename = QString("ministaller-%1.log").arg(time);

    QString logFilePath = logsDir.filePath(logFilename);
    return logFilePath;
}

void launchWindowsInstaller(const QString &pathToUpdate) {
    LOG_DEBUG << "#";

    const QString appDirPath = QCoreApplication::applicationDirPath();
    QDir appDir(appDirPath);
    QString ministallerPath = appDir.filePath("ministaller.exe");

    if (!QFileInfo(ministallerPath).exists()) {
        LOG_WARNING << "Updater not found!" << ministallerPath;
        return;
    }

    QString installerLogPath = getLogFilenameForMinistaller();
    QStringList arguments;
    arguments << "-force-update" << "-gui" <<
                 "-install-path" << appDirPath <<
                 "-l" << installerLogPath <<
                 "-launch-exe" << "xpiks-qt.exe" <<
                 "-package-path" << pathToUpdate <<
                 "-stdout";

    QProcess::startDetached(ministallerPath, arguments);
}

void cleanupUpdateArtifacts() {
    QString appDirPath = QCoreApplication::applicationDirPath();
    QDirIterator dirIt(appDirPath, QStringList() << "*.bak", QDir::Files, QDirIterator::Subdirectories);
    int count = 0;

    while (dirIt.hasNext()) {
        QString filePath = dirIt.next();
        QFile backedUpFile(filePath);

        if (backedUpFile.remove()) {
            LOG_INFO << "Removed prev update leftover:" << filePath;
        } else {
            LOG_WARNING << "Failed to remove leftover:" << filePath;
        }

        count++;
    }

    if (count == 0) {
        LOG_DEBUG << "No leftovers found";
    }
}

#endif

namespace Helpers {
    void installUpdate(const QString &updatePath) {
#if defined(Q_OS_OSX)
        launchOSXdmg(updatePath);
#elif defined(Q_OS_WIN)
        launchWindowsInstaller(updatePath);
#endif
    }

    void cleanupUpdateArtifactsAsync() {
#ifdef Q_OS_WIN
        QtConcurrent::run(cleanupUpdateArtifacts);
#endif
    }
}
