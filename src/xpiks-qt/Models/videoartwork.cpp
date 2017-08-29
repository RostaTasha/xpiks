/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "videoartwork.h"
#include <QFileInfo>
#include "../Common/defines.h"

namespace Models {
    VideoArtwork::VideoArtwork(const QString &filepath, qint64 ID, qint64 directoryID):
        ArtworkMetadata(filepath, ID, directoryID),
        m_VideoFlags(0),
        m_BitRate(0),
        m_FrameRate(0.0)
    {
        m_ThumbnailPath = ":/Graphics/video-icon.svg";
    }

    void VideoArtwork::setThumbnailPath(const QString &filepath) {
        LOG_INFO << filepath;

        Q_ASSERT(!filepath.isEmpty());

        if (QFileInfo(filepath).exists()) {
            m_ThumbnailPath = filepath;
            setThumbnailGeneratedFlag(true);
            emit thumbnailUpdated();
        } else {
            LOG_WARNING << "Wrong thumbnail filepath";
        }
    }

    void VideoArtwork::initializeThumbnailPath(const QString &filepath) {
        if (getThumbnailGeneratedFlag()) { return; }
        if (filepath.isEmpty()) { return; }

        if (QFileInfo(filepath).exists()) {
            m_ThumbnailPath = filepath;
        } else {
            LOG_WARNING << "Wrong thumbnail filepath";
        }
    }

    bool VideoArtwork::initFromOriginUnsafe(const MetadataIO::OriginalMetadata &originalMetadata) {

    }

    bool VideoArtwork::initFromStorageUnsafe(const MetadataIO::CachedArtwork &cachedArtwork) {

    }
}
