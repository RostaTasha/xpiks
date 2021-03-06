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

#ifndef IMAGECACHINGSERVICE_H
#define IMAGECACHINGSERVICE_H

#include <QObject>
#include <QString>
#include <QVector>

namespace Models {
    class ArtworkMetadata;
}

class QScreen;

namespace QMLExtensions {
    class ImageCachingWorker;

    class ImageCachingService : public QObject
    {
        Q_OBJECT
    public:
        explicit ImageCachingService(QObject *parent = 0);

    public:
        void startService();
        void stopService();

    public:
        void setScale(qreal scale);
        void cacheImage(const QString &key, const QSize &requestedSize, bool recache=false);
        void generatePreviews(const QVector<Models::ArtworkMetadata *> &items);
        bool tryGetCachedImage(const QString &key, const QSize &requestedSize, QString &cached, bool &needsUpdate);

    public slots:
        void screenChangedHandler(QScreen *screen);
        void dpiChanged(qreal someDPI);

    private:
        ImageCachingWorker *m_CachingWorker;
        volatile bool m_IsCancelled;
        qreal m_Scale;
    };
}

#endif // IMAGECACHINGSERVICE_H
