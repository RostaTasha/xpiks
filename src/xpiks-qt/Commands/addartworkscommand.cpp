/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QFileInfo>
#include <QVector>
#include <QHash>
#include <memory>
#include "addartworkscommand.h"
#include "commandmanager.h"
#include "../Models/artworksrepository.h"
#include "../Models/artworkmetadata.h"
#include "../Models/artitemsmodel.h"
#include "../UndoRedo/addartworksitem.h"
#include "../Common/defines.h"
#include "../Helpers/filehelpers.h"
#include "../Models/imageartwork.h"
#include "../MetadataIO/artworkssnapshot.h"

int findAndAttachVectors(const MetadataIO::WeakArtworksSnapshot &artworksList, QVector<int> &modifiedIndices) {
    LOG_DEBUG << "#";
    int attachedCount = 0;
    size_t size = artworksList.size();
    modifiedIndices.reserve(size);

    for (size_t i = 0; i < size; ++i) {
        Models::ArtworkMetadata *artwork = artworksList.at(i);
        Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork *>(artwork);

        if (image == NULL) { continue; }

        if (image->hasVectorAttached()) {
            attachedCount++;
            modifiedIndices.append(i);
            continue;
        }

        const QString &filepath = image->getFilepath();
        QStringList vectors = Helpers::convertToVectorFilenames(filepath);

        foreach (const QString &item, vectors) {
            if (QFileInfo(item).exists()) {
                image->attachVector(item);
                attachedCount++;
                modifiedIndices.append(i);
                break;
            }
        }
    }

    return attachedCount;
}

void accountVectors(Models::ArtworksRepository *artworksRepository, const MetadataIO::WeakArtworksSnapshot &artworks) {
    LOG_DEBUG << "#";

    size_t size = artworks.size();
    for (size_t i = 0; i < size; ++i) {
        Models::ArtworkMetadata *metadata = artworks.at(i);
        Models::ImageArtwork *imageArtwork = dynamic_cast<Models::ImageArtwork *>(metadata);
        if ((imageArtwork != nullptr) && imageArtwork->hasVectorAttached()) {
            artworksRepository->accountVector(imageArtwork->getAttachedVectorPath());
        }
    }
}

Commands::AddArtworksCommand::~AddArtworksCommand() {
    LOG_DEBUG << "#";
}

std::shared_ptr<Commands::ICommandResult> Commands::AddArtworksCommand::execute(const ICommandManager *commandManagerInterface) const {
    LOG_INFO << m_FilePathes.length() << "images," << m_VectorsPathes.length() << "vectors";
    CommandManager *commandManager = (CommandManager*)commandManagerInterface;

    Models::ArtworksRepository *artworksRepository = commandManager->getArtworksRepository();
    Models::ArtItemsModel *artItemsModel = commandManager->getArtItemsModel();

    const int newFilesCount = artworksRepository->getNewFilesCount(m_FilePathes);
    const int initialCount = artItemsModel->rowCount();
    const bool filesWereAccounted = artworksRepository->beginAccountingFiles(m_FilePathes);

    MetadataIO::ArtworksSnapshot artworksToImport;
    artworksToImport.reserve(newFilesCount);
    QStringList filesToWatch;
    filesToWatch.reserve(newFilesCount);

    if (newFilesCount > 0) {
        LOG_INFO << newFilesCount << "new files found";
        LOG_INFO << "Current files count is" << initialCount;
        artItemsModel->beginAccountingFiles(newFilesCount);

        const int count = m_FilePathes.count();

        for (int i = 0; i < count; ++i) {
            const QString &filename = m_FilePathes[i];
            qint64 directoryID = 0;

            if (artworksRepository->accountFile(filename, directoryID, m_IsFullDirectory)) {
                Models::ArtworkMetadata *metadata = artItemsModel->createMetadata(filename, directoryID);
                commandManager->connectArtworkSignals(metadata);

                LOG_INTEGRATION_TESTS << "Added file:" << filename;

                artItemsModel->appendArtwork(metadata);
                artworksToImport.append(metadata);
                filesToWatch.append(filename);
            } else {
                LOG_INFO << "Rejected file:" << filename;
            }
        }

        artItemsModel->endAccountingFiles();
    }

    artworksRepository->endAccountingFiles(filesWereAccounted);
    artworksRepository->watchFilePaths(filesToWatch);
    artworksRepository->updateFilesCounts();

    QHash<QString, QHash<QString, QString> > vectorsHash;
    decomposeVectors(vectorsHash);
    QVector<int> modifiedIndices;

    int attachedCount = artItemsModel->attachVectors(vectorsHash, modifiedIndices);

    if (m_AutoDetectVectors) {
        QVector<int> autoAttachedIndices;
        attachedCount = findAndAttachVectors(artworksToImport.getWeakSnapshot(), autoAttachedIndices);

        foreach (int index, autoAttachedIndices) {
            modifiedIndices.append(initialCount + index);
        }
    }

    if (newFilesCount > 0) {
        commandManager->readMetadata(artworksToImport);
        accountVectors(artworksRepository, artworksToImport.getWeakSnapshot());
        artworksRepository->updateCountsForExistingDirectories();

        std::unique_ptr<UndoRedo::IHistoryItem> addArtworksItem(new UndoRedo::AddArtworksHistoryItem(getCommandID(), initialCount, newFilesCount));
        commandManager->recordHistoryItem(addArtworksItem);

        // Generating previews was in the metadata io coordinator
        // called _after_ the reading to make reading (in Xpiks)
        // as fast as possible. Not needed if using only exiftool now
        commandManager->generatePreviews(artworksToImport);
        commandManager->addToRecentFiles(filesToWatch);
        commandManager->saveSessionInBackground();
    }

    artItemsModel->raiseArtworksAdded(newFilesCount, attachedCount);
    artItemsModel->updateItems(modifiedIndices, QVector<int>() << Models::ArtItemsModel::HasVectorAttachedRole);

    std::shared_ptr<AddArtworksCommandResult> result(new AddArtworksCommandResult(newFilesCount));
    return result;
}

void Commands::AddArtworksCommand::decomposeVectors(QHash<QString, QHash<QString, QString> > &vectors) const {
    int size = m_VectorsPathes.size();
    LOG_DEBUG << size << "item(s)";

    for (int i = 0; i < size; ++i) {
        const QString &path = m_VectorsPathes.at(i);
        QFileInfo fi(path);
        const QString &absolutePath = fi.absolutePath();

        if (!vectors.contains(absolutePath)) {
            vectors.insert(absolutePath, QHash<QString, QString>());
        }

        vectors[absolutePath].insert(fi.baseName().toLower(), path);
    }
}
