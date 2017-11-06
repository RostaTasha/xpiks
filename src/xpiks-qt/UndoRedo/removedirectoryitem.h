/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef REMOVEDIRECTORYITEM_H
#define REMOVEDIRECTORYITEM_H

#include "historyitem.h"
#include "removeartworksitem.h"
#include "../Helpers/filehelpers.h"
#include "../Common/defines.h"
#include <QFileInfo>

namespace UndoRedo {
    class RemoveDirectoryItem:
        public RemoveArtworksHistoryItem
    {
    public:
        RemoveDirectoryItem(int commandID, const QString &absolutePath, int startFilesIndex, int startDirectoryIndex, bool isDirectorySelected):
            RemoveArtworksHistoryItem(commandID, {}, {}, {}, true),
            m_AbsolutePath(absolutePath),
            m_StartFilesIndex(startFilesIndex),
            m_StartDirectoryIndex(startDirectoryIndex),
            m_IsDirectorySelected(isDirectorySelected)
        {
            QStringList files, vectorFiles;
            QStringList rawFilenames;
            Helpers::extractFiles(m_AbsolutePath, rawFilenames);
            Helpers::extractImagesAndVectors(rawFilenames, files, vectorFiles);
            size_t vectorSize = vectorFiles.length();

            QHash<QString, QString> vectorsHash;
            for (int i = 0; i < vectorSize; ++i) {
                const QString &path = vectorFiles.at(i);
                QFileInfo fi(path);
                vectorsHash.insert(fi.baseName().toLower(), path);
            }

            QStringList vectors;

            for (const auto &file : files) {
                QFileInfo fi(file);
                vectors.push_back({});
                if (vectorsHash.contains(fi.baseName().toLower())) {
                    vectors.last() = vectorsHash[fi.baseName().toLower()];
                }
            }

            QVector<int> indices;
            int size = files.length();
            indices.reserve(size);
            for (int i = 0; i < size; i++) {
                indices.push_back(startFilesIndex + i);
            }

            setRemovedArtworksIndices(indices);
            setRemovedArtworksPathes(files);
            setRemovedAttachedVectors(vectors);
        }

        virtual ~RemoveDirectoryItem() {}

    public:
        virtual void undo(const Commands::ICommandManager *commandManagerInterface) const override;

    public:
        virtual QString getDescription() const override {
            return QObject::tr("%1 directory removed").arg(m_AbsolutePath);
        }

    private:
        QString m_AbsolutePath;
        int m_StartFilesIndex;
        int m_StartDirectoryIndex;
        bool m_IsDirectorySelected;
    };
}

#endif // REMOVEDIRECTORYITEM_H
