#include "SignalTests.h"

void SignalTests::testSchematicsLoading()
{
    QStringList files;
    files.append(SCHEMATIC);
    app = getApp(files);
    qInfo() << "Successfully loaded: " << SCHEMATIC;
}

QTEST_MAIN(SignalTests)
