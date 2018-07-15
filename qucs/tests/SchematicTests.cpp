
#include "qucs.h"
#include "schematic.h"
#include "SchematicTests.h"

#include <QTest>

void SchematicTests::testConstructor()
{

    QucsApp *app = new QucsApp();

    QString name;
    Schematic *sch;

    name = QString("");
    sch = new Schematic(app, name);
    QCOMPARE(sch->symbolMode, false);
    // go up to QucsDoc, empy name
    QCOMPARE(sch->DocName, name);

    name = QString("DocumentName");
    sch = new Schematic(app, name);
    // go up to QucsDoc, make up name with absolute path
    QFileInfo Info(name);
    QCOMPARE(sch->DocName, Info.absoluteFilePath());
}

QTEST_MAIN(SchematicTests)

// vim:ts=8:sw=2:
