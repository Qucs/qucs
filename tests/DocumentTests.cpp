#include "DocumentTests.h"

#ifndef QUCS_TEST_DIR
#define QUCS_TEST_DIR "../../tests"
#endif

void DocumentTests::loadDocument(QString name)
{
    qInfo() << name;
    QStringList files;
    app = new QucsAppTest(files);
    app->loadPRJ(name);
    /*
    Schematic *sch;
    QTextStream out(stdout);
    sch = new Schematic(app, name);
    Q_ASSERT(sch!=NULL);
    QCOMPARE(sch->symbolMode, false);
    out << "Opening " << sch->DocName << " successful" << Qt::endl;
    */
}

void DocumentTests::testDocumentLoading()
{
  loadDocument(PROJECT);
}

QTEST_MAIN(DocumentTests)
