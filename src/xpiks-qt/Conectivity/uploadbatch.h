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

#ifndef UPLOADBATCH_H
#define UPLOADBATCH_H

#include <memory>
#include <QStringList>
#include "uploadcontext.h"

namespace Conectivity {
    class UploadBatch {
    public:
        UploadBatch(const std::shared_ptr<UploadContext> &context, const QStringList &filesList):
            m_FilesList(filesList),
            m_UploadContext(context)
        {}

        virtual ~UploadBatch() { }

    public:
        const QStringList &getFilesToUpload() const { return m_FilesList; }
        UploadContext *getContext() const { return m_UploadContext.get(); }

    private:
        QStringList m_FilesList;
        std::shared_ptr<UploadContext> m_UploadContext;
    };
}

#endif // UPLOADBATCH_H
