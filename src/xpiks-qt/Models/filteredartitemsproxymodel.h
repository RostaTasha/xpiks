/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FILTEREDARTITEMSPROXYMODEL_H
#define FILTEREDARTITEMSPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QString>
#include <QList>
#include <functional>
#include "../Common/flags.h"
#include "../Common/baseentity.h"
#include "../MetadataIO/artworkssnapshot.h"

namespace Models {
    class ArtworkMetadata;
    class MetadataElement;
    class PreviewMetadataElement;
    class ArtItemsModel;

    class FilteredArtItemsProxyModel:
        public QSortFilterProxyModel, public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(QString searchTerm READ getSearchTerm WRITE setSearchTerm NOTIFY searchTermChanged)
        Q_PROPERTY(int selectedArtworksCount READ getSelectedArtworksCount NOTIFY selectedArtworksCountChanged)

    public:
        FilteredArtItemsProxyModel(QObject *parent=0);

    public:
        const QString &getSearchTerm() const { return m_SearchTerm; }
        void setSearchTerm(const QString &value);

        int getSelectedArtworksCount() const { return m_SelectedArtworksCount; }
        void spellCheckAllItems();

        std::vector<MetadataElement> getSearchableOriginalItems(const QString &searchTerm, Common::SearchFlags flags) const;

        std::vector<PreviewMetadataElement> getSearchablePreviewOriginalItems(const QString &searchTerm, Common::SearchFlags flags) const;

#ifdef CORE_TESTS
        int retrieveNumberOfSelectedItems();
#endif

    public:
        Q_INVOKABLE int getOriginalIndex(int index) const;
        Q_INVOKABLE int getDerivedIndex(int originalIndex) const;

        Q_INVOKABLE void invertSelectionArtworks() { invertFilteredItemsSelected(); }
        Q_INVOKABLE void selectFilteredArtworks() { setFilteredItemsSelected(true); }
        Q_INVOKABLE void unselectFilteredArtworks() { setFilteredItemsSelected(false); }
        Q_INVOKABLE void selectDirectory(int directoryIndex);
        Q_INVOKABLE void combineSelectedArtworks();
        Q_INVOKABLE void setSelectedItemsSaved();
        Q_INVOKABLE void removeSelectedArtworks();
        Q_INVOKABLE void updateSelectedArtworks();
        /*Q_INVOKABLE*/ void updateSelectedArtworksEx(const QVector<int> &roles);
        Q_INVOKABLE void saveSelectedArtworks(bool overwriteAll, bool useBackups);
        Q_INVOKABLE void setSelectedForUpload();
        Q_INVOKABLE void setSelectedForZipping();
        Q_INVOKABLE bool areSelectedArtworksSaved();
        Q_INVOKABLE void spellCheckSelected();
        Q_INVOKABLE int getModifiedSelectedCount(bool overwriteAll=false);
        Q_INVOKABLE void removeArtworksDirectory(int index);
        Q_INVOKABLE void deleteKeywordsFromSelected();

        Q_INVOKABLE int getItemsCount() const { return rowCount(); }
        Q_INVOKABLE void reimportMetadataForSelected();
        Q_INVOKABLE int findSelectedItemIndex() const;
        Q_INVOKABLE void removeMetadataInSelected() const;
        Q_INVOKABLE void clearKeywords(int index);

        Q_INVOKABLE void updateFilter() { invalidateFilter(); emit afterInvalidateFilter(); }
        Q_INVOKABLE void focusNextItem(int index);
        Q_INVOKABLE void focusPreviousItem(int index);
        Q_INVOKABLE void spellCheckDescription(int index);
        Q_INVOKABLE void spellCheckTitle(int index);
        Q_INVOKABLE void toggleSorted();
        Q_INVOKABLE void detachVectorFromSelected();
        Q_INVOKABLE QObject *getArtworkMetadata(int index);
        Q_INVOKABLE QObject *getBasicModel(int index);

        Q_INVOKABLE bool hasTitleWordSpellError(int index, const QString &word);
        Q_INVOKABLE bool hasDescriptionWordSpellError(int index, const QString &word);

        Q_INVOKABLE void registerCurrentItem(int index) const;
        Q_INVOKABLE void copyToQuickBuffer(int index) const;
        Q_INVOKABLE void fillFromQuickBuffer(int index) const;
        Q_INVOKABLE void suggestCorrectionsForSelected() const;
        Q_INVOKABLE void removeDuplicatesFromSelected() const;


    public slots:
        void itemSelectedChanged(bool value);
        void onSelectedArtworksRemoved(int value);
        void onSpellCheckerAvailable(bool afterRestart);
        void onSettingsUpdated();

    signals:
        void searchTermChanged(const QString &searchTerm);
        void selectedArtworksCountChanged();
        void afterInvalidateFilter();
        void allItemsSelectedChanged();
        void forceUnselected();

    private:
        void removeMetadataInItems(std::vector<MetadataElement> &itemsToClear, Common::CombinedEditFlags flags) const;
        void removeKeywordsInItem(ArtworkMetadata *metadata, int originalIndex);
        void setFilteredItemsSelected(bool selected);
        void invertFilteredItemsSelected();

        QVector<ArtworkMetadata *> getSelectedOriginalItems() const;
        MetadataIO::ArtworksSnapshot::Container getSelectedArtworksSnapshot() const;
        std::vector<MetadataElement> getSelectedOriginalItemsWithIndices() const;

        std::vector<MetadataElement> getAllItemsWithIndices() const;

        template<typename T>
        std::vector<T> getFilteredOriginalItems(std::function<bool (ArtworkMetadata *)> pred,
                                                std::function<T(ArtworkMetadata *, int, int)> mapper) const;

        QVector<ArtworkMetadata *> getAllOriginalItems() const;

        QVector<int> getSelectedOriginalIndices() const;
        QVector<int> getSelectedIndices() const;
        void forceUnselectAllItems();
        ArtItemsModel *getArtItemsModel() const;

        void updateSearchFlags();

    protected:
        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
        virtual bool lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const override;

    private:
        // ignore default regexp from proxymodel
        QString m_SearchTerm;
        Common::SearchFlags m_SearchFlags;
        volatile int m_SelectedArtworksCount;
        volatile bool m_SortingEnabled;
    };
}

#endif // FILTEREDARTITEMSPROXYMODEL_H
