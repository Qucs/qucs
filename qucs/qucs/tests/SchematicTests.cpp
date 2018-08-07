
#include "qucs.h"
#include "schematic.h"
#include "SchematicTests.h"

#include <QTest>
#include <QApplication>

void SchematicTests::testConstructor()
{

    QucsApp *app = new QucsApp();

    QString name;
    Schematic *sch;

    name = QString("");
    sch = new Schematic(app, name);
    QCOMPARE(sch->isSymbolMode(), false);
    // go up to QucsDoc, empy name
    QCOMPARE(sch->docName(), name);

    name = QString("DocumentName");
    sch = new Schematic(app, name);
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
