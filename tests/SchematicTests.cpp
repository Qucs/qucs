#include "SchematicTests.h"

#ifndef QUCS_TEST_DIR
#define QUCS_TEST_DIR "../../tests"
#endif

void SchematicTests::testCreatingSchematics()
{
    QStringList files;
    app = getApp(files);
    Schematic *sch;
    QString name = QString("");
    sch = new Schematic(app, name);
    Q_ASSERT(sch!=NULL);
    QCOMPARE(sch->symbolMode, false);
    // go up to QucsDoc, empy name
    QCOMPARE(sch->DocName, name);

    name = QString("DocumentName");
    sch = new Schematic(app, name);
    Q_ASSERT(sch!=NULL);
    // go up to QucsDoc, make up name with absolute path
    QFileInfo Info(name);

    QCOMPARE(sch->DocName, Info.absoluteFilePath());
}

void SchematicTests::testSchematicsLoading()
{
    QStringList files;
    app = getApp(files);
    //files.clear();
    //files.append(QDir(dirs.filePath(project_name)).filePath(name));
    //app = new QucsAppTest(files);
    ::openSchematic(SCHEMATIC);
}

QTEST_MAIN(SchematicTests)
