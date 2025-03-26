#include "NetlistTests.h"

#ifndef QUCS_TEST_DIR
#define QUCS_TEST_DIR "../../tests"
#endif

void NetlistTests::testNetlistsLoading()
{
    QStringList files;
    app = getApp(files);
    ::doNetlist(
      SCHEMATIC,
      QString(SCHEMATIC)+QString(".new.v")
    );
    ::doNetlist(
      SCHEMATIC,
      QString(SCHEMATIC)+QString(".new.net")
    );
}

QTEST_MAIN(NetlistTests)
