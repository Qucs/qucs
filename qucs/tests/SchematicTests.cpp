
#include "qucs.h"
#include "schematic_doc.h"
#include "SchematicTests.h"

#include <QTest>
#include <QApplication>

void SchematicTests::testConstructor()
{

    QucsApp *app = new QucsApp();

    QString name;
    SchematicDoc *sch;

    name = QString("");
    sch = new SchematicDoc(app, name, nullptr);
    QCOMPARE(sch->isSymbolMode(), false);
    // go up to QucsDoc, empy name
    QCOMPARE(sch->docName(), name);

    name = QString("DocumentName");
    sch = new SchematicDoc(app, name, nullptr);
    // go up to QucsDoc, make up name with absolute path
    QFileInfo Info(name);
    QCOMPARE(sch->docName(), Info.absoluteFilePath());
}

// QTEST_MAIN(SchematicTests)
int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  SchematicTests t();
}

// vim:ts=8:sw=2:
