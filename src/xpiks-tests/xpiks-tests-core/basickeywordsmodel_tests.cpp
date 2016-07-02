#include "basickeywordsmodel_tests.h"
#include <QSignalSpy>
#include "../../xpiks-qt/Common/basickeywordsmodel.h"
#include "../../xpiks-qt/Common/flags.h"

void BasicKeywordsModelTests::constructEmptyTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QCOMPARE(basicModel.isDescriptionEmpty(), true);
    QCOMPARE(basicModel.isTitleEmpty(), true);
    QCOMPARE(basicModel.isEmpty(), true);
    QCOMPARE(basicModel.getKeywordsCount(), 0);
    QVERIFY(basicModel.getSpellCheckInfo() == NULL);
}

void BasicKeywordsModelTests::simpleAddKeywordTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QSignalSpy addSignalSpy(&basicModel, SIGNAL(rowsInserted(QModelIndex,int,int)));

    bool result = basicModel.appendKeyword("test keyword");
    QCOMPARE(result, true);

    QCOMPARE(addSignalSpy.count(), 1);
    QCOMPARE(basicModel.getKeywordsCount(), 1);
    QList<QVariant> addedArguments = addSignalSpy.takeFirst();
    QCOMPARE(addedArguments.at(1).toInt(), 0);
    QCOMPARE(addedArguments.at(2).toInt(), 0);
}

void BasicKeywordsModelTests::addExistingKeywordTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    bool result = basicModel.appendKeyword("test keyword");
    QCOMPARE(result, true);

    QSignalSpy addSignalSpy(&basicModel, SIGNAL(rowsInserted(QModelIndex,int,int)));

    result = basicModel.appendKeyword("test keyword");
    QCOMPARE(result, false);

    QCOMPARE(addSignalSpy.count(), 0);
    QCOMPARE(basicModel.getKeywordsCount(), 1);
}

void BasicKeywordsModelTests::addSeveralKeywordsTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QSignalSpy addSignalSpy(&basicModel, SIGNAL(rowsInserted(QModelIndex,int,int)));

    QStringList keywords;
    keywords << "keyword1" << "keyword2" << "keyword3";

    int appendedCount = basicModel.appendKeywords(keywords);
    QCOMPARE(appendedCount, keywords.length());

    QCOMPARE(addSignalSpy.count(), 1);
    QCOMPARE(basicModel.getKeywordsCount(), keywords.length());
    QList<QVariant> addedArguments = addSignalSpy.takeFirst();
    QCOMPARE(addedArguments.at(1).toInt(), 0);
    QCOMPARE(addedArguments.at(2).toInt(), keywords.length() - 1);
}

void BasicKeywordsModelTests::removeExistingKeywordTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QString keyword = "test keyword";
    int appendedCount = basicModel.appendKeyword(keyword);
    QCOMPARE(appendedCount, 1);

    QSignalSpy removeSignalSpy(&basicModel, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    QString removedKeyword;
    bool result = basicModel.takeKeywordAt(0, removedKeyword);
    QCOMPARE(result, true);

    QCOMPARE(removedKeyword, keyword);

    QCOMPARE(basicModel.getKeywordsCount(), 0);
    QCOMPARE(removeSignalSpy.count(), 1);
    QList<QVariant> removedArguments = removeSignalSpy.takeFirst();
    QCOMPARE(removedArguments.at(1).toInt(), 0);
    QCOMPARE(removedArguments.at(2).toInt(), 0);
}

void BasicKeywordsModelTests::removeNonExistingKeywordTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    bool result = basicModel.appendKeyword("test keyword");
    QCOMPARE(result, true);

    QSignalSpy removeSignalSpy(&basicModel, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    QString removedKeyword;
    result = basicModel.takeKeywordAt(-1, removedKeyword);
    QCOMPARE(result, false);
    QCOMPARE(basicModel.getKeywordsCount(), 1);
    QCOMPARE(removeSignalSpy.count(), 0);

    result = basicModel.takeKeywordAt(1, removedKeyword);
    QCOMPARE(result, false);
    QCOMPARE(basicModel.getKeywordsCount(), 1);
    QCOMPARE(removeSignalSpy.count(), 0);
}

void BasicKeywordsModelTests::appendSameKeywordsTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QSignalSpy addSignalSpy(&basicModel, SIGNAL(rowsInserted(QModelIndex,int,int)));

    QString keyword = "test keyword";
    QStringList keywords;
    keywords << keyword << keyword << keyword << keyword;

    int appendedCount = basicModel.appendKeywords(keywords);
    QCOMPARE(appendedCount, 1);

    QCOMPARE(addSignalSpy.count(), 1);
    QCOMPARE(basicModel.getKeywordsCount(), 1);
    QList<QVariant> addedArguments = addSignalSpy.takeFirst();
    QCOMPARE(addedArguments.at(1).toInt(), 0);
    QCOMPARE(addedArguments.at(2).toInt(), 0);
}

void BasicKeywordsModelTests::appendSameChangedKeywordTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QSignalSpy addSignalSpy(&basicModel, SIGNAL(rowsInserted(QModelIndex,int,int)));

    QStringList keywords;
    keywords << "keyword" << "KEYWORD" << "kEyworD" << "keYword" << "  keyword" << "keyword  " << "   kEyworD  ";

    int appendedCount = basicModel.appendKeywords(keywords);
    QCOMPARE(appendedCount, 1);

    QCOMPARE(addSignalSpy.count(), 1);
    QCOMPARE(basicModel.getKeywordsCount(), 1);
    QList<QVariant> addedArguments = addSignalSpy.takeFirst();
    QCOMPARE(addedArguments.at(1).toInt(), 0);
    QCOMPARE(addedArguments.at(2).toInt(), 0);
}

void BasicKeywordsModelTests::appendNoKeywordsTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QSignalSpy addSignalSpy(&basicModel, SIGNAL(rowsInserted(QModelIndex,int,int)));

    QStringList keywords;

    int appendedCount = basicModel.appendKeywords(keywords);
    QCOMPARE(appendedCount, 0);

    QCOMPARE(addSignalSpy.count(), 0);
    QCOMPARE(basicModel.getKeywordsCount(), 0);
}

void BasicKeywordsModelTests::clearKeywordsTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "keyword1" << "keyword2" << "keyword3";

    QString description = "Some description";
    QString title = "Some title";
    basicModel.setDescription(description);
    basicModel.setTitle(title);
    basicModel.appendKeywords(keywords);

    QSignalSpy clearSignalSpy(&basicModel, SIGNAL(modelReset()));

    basicModel.clearKeywords();

    QCOMPARE(clearSignalSpy.count(), 1);
    QCOMPARE(basicModel.getKeywordsCount(), 0);
    QCOMPARE(basicModel.getTitle(), title);
    QCOMPARE(basicModel.getDescription(), description);
}

void BasicKeywordsModelTests::clearModelTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "keyword1" << "keyword2" << "keyword3";

    QString description = "Some description";
    QString title = "Some title";
    basicModel.setDescription(description);
    basicModel.setTitle(title);
    basicModel.appendKeywords(keywords);

    QSignalSpy clearSignalSpy(&basicModel, SIGNAL(modelReset()));

    basicModel.clearModel();

    QCOMPARE(clearSignalSpy.count(), 1);
    QCOMPARE(basicModel.getKeywordsCount(), 0);
    QVERIFY(basicModel.isDescriptionEmpty());
    QVERIFY(basicModel.isTitleEmpty());
}

void BasicKeywordsModelTests::containsKeywordTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "keyword1" << "keyword2" << "keyword3";
    basicModel.appendKeywords(keywords);

    QVERIFY(basicModel.containsKeyword("keyword1"));
}

void BasicKeywordsModelTests::containsKeywordStrictTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "something_keyword1" << "keyword2" << "keyword3";
    basicModel.appendKeywords(keywords);

    int flags = Common::SearchFlagExactMatch;
    QVERIFY(basicModel.containsKeyword("keyword1", flags) == false);
    QVERIFY(basicModel.containsKeyword("keyword2", flags) == true);
    QVERIFY(basicModel.containsKeyword("kEyworD3", flags) == false);
}

void BasicKeywordsModelTests::containsKeywordFuzzyTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "something_keyword1" << "keyword2" << "KeyworD3";
    basicModel.appendKeywords(keywords);

    bool fuzzyMatch = false;
    QVERIFY(basicModel.containsKeyword("keyword1", fuzzyMatch) == true);
    QVERIFY(basicModel.containsKeyword("keyword2", fuzzyMatch) == true);
    QVERIFY(basicModel.containsKeyword("keyword3", fuzzyMatch) == true);
}

void BasicKeywordsModelTests::doesNotContainKeywordTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "keyword1" << "keyword2" << "keyword3";
    basicModel.appendKeywords(keywords);

    QVERIFY(basicModel.containsKeyword("keyword4", true) == false);
    QVERIFY(basicModel.containsKeyword("keyword4", false) == false);
    QVERIFY(basicModel.containsKeyword("keyword11", true) == false);
    QVERIFY(basicModel.containsKeyword("keyword11", false) == false);
}

void BasicKeywordsModelTests::setSameTitleTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);
    bool result = basicModel.setTitle("new title");
    QCOMPARE(result, true);
    result = basicModel.setTitle("new title");
    QCOMPARE(result, false);
}

void BasicKeywordsModelTests::setSameDescriptionTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);
    bool result = basicModel.setDescription("new description");
    QCOMPARE(result, true);
    result = basicModel.setDescription("new description");
    QCOMPARE(result, false);
}

void BasicKeywordsModelTests::editKeywordToAnotherTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "keyword1" << "keyword2" << "keyword3";
    basicModel.appendKeywords(keywords);

    QSignalSpy modifiedSpy(&basicModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)));

    bool editResult = basicModel.editKeyword(0, "keyword4");
    QCOMPARE(editResult, true);
    QCOMPARE(modifiedSpy.count(), 1);
}

void BasicKeywordsModelTests::editKeywordToSameTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "keyword1" << "keyword2" << "keyword3";
    basicModel.appendKeywords(keywords);

    QSignalSpy modifiedSpy(&basicModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)));

    bool editResult = basicModel.editKeyword(0, "keyword3");
    QCOMPARE(editResult, false);
    QCOMPARE(modifiedSpy.count(), 0);
}

void BasicKeywordsModelTests::addRemoveAddUpperCaseWordTest() {
    const QString keyword = "Test";

    Common::BasicKeywordsModel basicModel(m_FakeHold);

    bool addResult = basicModel.appendKeyword(keyword);
    QCOMPARE(addResult, true);

    QString removed;
    bool removeResult = basicModel.takeKeywordAt(0, removed);
    QCOMPARE(removeResult, true);
    QCOMPARE(removed, keyword);

    bool addAgainResult = basicModel.appendKeyword(keyword);
    QCOMPARE(addAgainResult, true);
}

void BasicKeywordsModelTests::editToUpperCaseTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "keyword1" << "keyword2" << "keyword3";
    basicModel.appendKeywords(keywords);

    bool editSelfResult = basicModel.editKeyword(0, "Keyword1");
    QCOMPARE(editSelfResult, true);

    bool editOtherResult = basicModel.editKeyword(1, "Keyword1");
    QCOMPARE(editOtherResult, false);
}

void BasicKeywordsModelTests::hasKeywordTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "keyword1" << "keyword2" << "keyword3";
    basicModel.appendKeywords(keywords);

    QVERIFY(basicModel.hasKeyword("keyword1"));
    QVERIFY(basicModel.hasKeyword("keYwoRd1  "));
    QVERIFY(basicModel.hasKeyword("   kEyword1"));
    QVERIFY(basicModel.hasKeyword("KEYWORD1"));

    QVERIFY(!basicModel.hasKeyword("keyword1+"));
    QVERIFY(!basicModel.hasKeyword("keyword4+"));
}

void BasicKeywordsModelTests::simpleReplaceTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QString originalTitle = "Test title here";
    QString originalDescription = "Test description";
    QStringList originalKeywords;
    originalKeywords << "Test keyword1" << "keywTestord2" << "keyword3 Test";

    basicModel.setTitle(originalTitle);
    basicModel.setDescription(originalDescription);
    basicModel.appendKeywords(originalKeywords);

    int flags = Common::SearchFlagSearchMetadata;

    bool replaceSucceeded = basicModel.replace("Test", "Replaced", flags);
    QVERIFY(replaceSucceeded);
    QCOMPARE(basicModel.getTitle(), QLatin1String("Replaced title here"));
    QCOMPARE(basicModel.getDescription(), QLatin1String("Replaced description"));
    QCOMPARE(basicModel.getKeywordAt(0), QLatin1String("Replaced keyword1"));
    QCOMPARE(basicModel.getKeywordAt(1), QLatin1String("keywReplacedord2"));
    QCOMPARE(basicModel.getKeywordAt(2), QLatin1String("keyword3 Replaced"));
}

void BasicKeywordsModelTests::descriptionReplaceTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QString originalTitle = "Test title here";
    QString originalDescription = "Test description";
    QStringList originalKeywords;
    originalKeywords << "Test keyword1" << "keywTestord2" << "keyword3 Test";

    basicModel.setTitle(originalTitle);
    basicModel.setDescription(originalDescription);
    basicModel.appendKeywords(originalKeywords);

    int flags = Common::SearchFlagSearchDescription;

    bool replaceSucceeded = basicModel.replace("Test", "Replaced", flags);
    QVERIFY(replaceSucceeded);
    QCOMPARE(basicModel.getTitle(), originalTitle);
    QCOMPARE(basicModel.getDescription(), QLatin1String("Replaced description"));
    QCOMPARE(basicModel.getKeywordAt(0), originalKeywords[0]);
    QCOMPARE(basicModel.getKeywordAt(1), originalKeywords[1]);
    QCOMPARE(basicModel.getKeywordAt(2), originalKeywords[2]);
}

void BasicKeywordsModelTests::titleReplaceTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QString originalTitle = "Test title here";
    QString originalDescription = "Test description";
    QStringList originalKeywords;
    originalKeywords << "Test keyword1" << "keywTestord2" << "keyword3 Test";

    basicModel.setTitle(originalTitle);
    basicModel.setDescription(originalDescription);
    basicModel.appendKeywords(originalKeywords);

    int flags = Common::SearchFlagSearchTitle;

    bool replaceSucceeded = basicModel.replace("Test", "Replaced", flags);
    QVERIFY(replaceSucceeded);
    QCOMPARE(basicModel.getTitle(), QLatin1String("Replaced title here"));
    QCOMPARE(basicModel.getDescription(), originalDescription);
    QCOMPARE(basicModel.getKeywordAt(0), originalKeywords[0]);
    QCOMPARE(basicModel.getKeywordAt(1), originalKeywords[1]);
    QCOMPARE(basicModel.getKeywordAt(2), originalKeywords[2]);
}

void BasicKeywordsModelTests::keywordsReplaceTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QString originalTitle = "Test title here";
    QString originalDescription = "Test description";
    QStringList originalKeywords;
    originalKeywords << "Test keyword1" << "keywTestord2" << "keyword3 Test";

    basicModel.setTitle(originalTitle);
    basicModel.setDescription(originalDescription);
    basicModel.appendKeywords(originalKeywords);

    int flags = Common::SearchFlagSearchKeywords;

    bool replaceSucceeded = basicModel.replace("Test", "Replaced", flags);
    QVERIFY(replaceSucceeded);
    QCOMPARE(basicModel.getTitle(), originalTitle);
    QCOMPARE(basicModel.getDescription(), originalDescription);
    QCOMPARE(basicModel.getKeywordAt(0), QLatin1String("Replaced keyword1"));
    QCOMPARE(basicModel.getKeywordAt(1), QLatin1String("keywReplacedord2"));
    QCOMPARE(basicModel.getKeywordAt(2), QLatin1String("keyword3 Replaced"));
}

void BasicKeywordsModelTests::noReplaceCaseSensitiveTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QString originalTitle = "Test title here";
    QString originalDescription = "Test description";
    QStringList originalKeywords;
    originalKeywords << "Test keyword1" << "keywTestord2" << "keyword3 Test";

    basicModel.setTitle(originalTitle);
    basicModel.setDescription(originalDescription);
    basicModel.appendKeywords(originalKeywords);

    int flags = Common::SearchFlagSearchMetadata | Common::SearchFlagCaseSensitive;

    bool replaceSucceeded = basicModel.replace("test", "Replaced", flags);
    QVERIFY(!replaceSucceeded);
}

void BasicKeywordsModelTests::replaceCaseSensitiveTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QString originalTitle = "Test title test here";
    QString originalDescription = "Test description test";
    QStringList originalKeywords;
    originalKeywords << "Test keyword1" << "keywtestord2" << "keyTestword3 test";

    basicModel.setTitle(originalTitle);
    basicModel.setDescription(originalDescription);
    basicModel.appendKeywords(originalKeywords);

    int flags = Common::SearchFlagSearchMetadata | Common::SearchFlagCaseSensitive;

    bool replaceSucceeded = basicModel.replace("test", "Replaced", flags);
    QVERIFY(replaceSucceeded);
    QCOMPARE(basicModel.getTitle(), QLatin1String("Test title Replaced here"));
    QCOMPARE(basicModel.getDescription(), QLatin1String("Test description Replaced"));
    QCOMPARE(basicModel.getKeywordAt(0), originalKeywords[0]);
    QCOMPARE(basicModel.getKeywordAt(1), QLatin1String("keywReplacedord2"));
    QCOMPARE(basicModel.getKeywordAt(2), QLatin1String("keyTestword3 Replaced"));
}

void BasicKeywordsModelTests::replaceKeywordsWithRemoveTest() {
    Common::BasicKeywordsModel basicModel(m_FakeHold);

    QString originalTitle = "Test title here";
    QString originalDescription = "Test description";
    QStringList originalKeywords;
    originalKeywords << "Test keyword1" << "keywTestord2" << "keyword3 Test" << "Replaced keyword1";

    basicModel.setTitle(originalTitle);
    basicModel.setDescription(originalDescription);
    basicModel.appendKeywords(originalKeywords);

    int flags = Common::SearchFlagSearchMetadata;

    bool replaceSucceeded = basicModel.replace("Test", "Replaced", flags);
    QVERIFY(replaceSucceeded);
    QCOMPARE(basicModel.getTitle(), QLatin1String("Replaced title here"));
    QCOMPARE(basicModel.getDescription(), QLatin1String("Replaced description"));
    QCOMPARE(basicModel.getKeywordsCount(), originalKeywords.length() - 1);
    QCOMPARE(basicModel.getKeywordAt(0), QLatin1String("keywReplacedord2"));
    QCOMPARE(basicModel.getKeywordAt(1), QLatin1String("keyword3 Replaced"));
    QCOMPARE(basicModel.getKeywordAt(2), QLatin1String("Replaced keyword1"));
}

