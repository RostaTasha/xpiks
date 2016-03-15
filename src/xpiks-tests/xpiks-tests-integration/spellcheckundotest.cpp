#include "spellcheckundotest.h"
#include <QUrl>
#include <QFileInfo>
#include <QThread>
#include <QStringList>
#include "integrationtestbase.h"
#include "signalwaiter.h"
#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
#include "../../xpiks-qt/Models/artworkmetadata.h"
#include "../../xpiks-qt/Models/settingsmodel.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/SpellCheck/spellchecksuggestionmodel.h"
#include "../../xpiks-qt/SpellCheck/spellsuggestionsitem.h"
#include "../../xpiks-qt/UndoRedo/undoredomanager.h"

QString SpellCheckUndoTest::testName() {
    return QLatin1String("SpellcheckUndoTest");
}

void SpellCheckUndoTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
    settingsModel->setUseSpellCheck(true);
}

int SpellCheckUndoTest::doTest() {
    Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();
    QList<QUrl> files;
    files << QUrl::fromLocalFile(QFileInfo("images-for-tests/vector/026.jpg").absoluteFilePath());

    int addedCount = artItemsModel->addLocalArtworks(files);

    VERIFY(addedCount == files.length(), "Failed to add file");

    MetadataIO::MetadataIOCoordinator *ioCoordinator = m_CommandManager->getMetadataIOCoordinator();
    SignalWaiter waiter;
    QObject::connect(ioCoordinator, SIGNAL(metadataReadingFinished()), &waiter, SIGNAL(finished()));

    ioCoordinator->continueReading(true);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for reading metadata.");
    }

    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while reading");

    Models::ArtworkMetadata *metadata = artItemsModel->getArtwork(0);

    QString wrongWord = "abbreviatioe";
    metadata->setDescription(metadata->getDescription() + ' ' + wrongWord);
    metadata->setTitle(metadata->getTitle() + ' ' + wrongWord);
    metadata->appendKeyword("correct part " + wrongWord);
    metadata->setIsSelected(true);

    // wait for after-add spellchecking
    QThread::sleep(1);

    Models::FilteredArtItemsProxyModel *filteredModel = m_CommandManager->getFilteredArtItemsModel();
    QObject::connect(metadata, SIGNAL(spellCheckErrorsChanged()), &waiter, SIGNAL(finished()));

    filteredModel->spellCheckSelected();

    if (!waiter.wait(5)) {
        VERIFY(false, "Timeout for waiting for first spellcheck results");
    }

    // wait for finding suggestions
    QThread::sleep(1);

    VERIFY(metadata->hasDescriptionSpellError(), "Description spell error not detected");
    VERIFY(metadata->hasTitleSpellError(), "Title spell error not detected");
    VERIFY(metadata->hasKeywordsSpellError(), "Keywords spell error not detected");

    filteredModel->clearKeywords(0);

    UndoRedo::UndoRedoManager *undoRedoManager = m_CommandManager->getUndoRedoManager();
    undoRedoManager->undoLastAction();

    if (!waiter.wait(5)) {
        VERIFY(false, "Timeout for waiting for second spellcheck results");
    }

    // wait for finding suggestions
    QThread::sleep(1);

    VERIFY(metadata->hasDescriptionSpellError(), "Description spell error not detected on the second time");
    VERIFY(metadata->hasTitleSpellError(), "Title spell error not detected on the second time");
    VERIFY(metadata->hasKeywordsSpellError(), "Keywords spell error not detected on the second time");

    return 0;
}



