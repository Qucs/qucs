
#include "qucs.h"
#include "schematicview.h"
#include "SchematicTests.h"

#include <QTest>
#include <QApplication>

void SchematicTests::testConstructor()
{

    QucsApp *app = new QucsApp();

    QString name;
    SchematicView *sch;

    name = QString("");
    sch = new SchematicView(app, name);
    QCOMPARE(sch->symbolMode, false);
    // go up to QucsDoc, empy name
    QCOMPARE(sch->DocName, name);

    name = QString("DocumentName");
    sch = new SchematicView(app, name);
    // go up to QucsDoc, make up name with absolute path
    QFileInfo Info(name);
    QCOMPARE(sch->DocName, Info.absoluteFilePath());
}

// QTEST_MAIN(SchematicTests)
int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  SchematicTests t();
}

// vim:ts=8:sw=2:
