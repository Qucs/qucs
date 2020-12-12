# This file shows errors occured during cmake adjustment and how they are solved:

## No rule to make target '/home/martin/GITProjekte/qucs/qucs/qucs/components/componentdialog.h', needed by 'components/moc_componentdialog.cpp'.  Stop.
commit: 06cf440d22463f7e038bfecdde980aa697fc0a3a

## /home/martin/GITProjekte/qucs/qucs/qucs/components/optimizedialog.h:89: Error: Class declaration lacks Q_OBJECT macro.
commit: 01e825f78d4421356158746e906ba8bfbcd073b5

## qucs/qucs/qucs/components/../object.h:18:10: fatal error: io_trace.h: No such file or directory
commit: 0bca8683a1d1d1639466266a31d7308bc03f9c9c

## qucs/qucs/qucs/textdoc.cpp:141:44: error: expected ‘;’ before ‘PACKAGE_VERSION’ 141 |   stream << "Textfile settings file, Qucs " PACKAGE_VERSION "\n"
commit: aefc4692c2edaf307643dfc031d2e682c30b7a2f
Add compile definition

## qt_compat.h:25:10: fatal error: QDebug: No such file or directory
target_link_libraries(diagrams Qt5::Core) was missed for the target diagrams
commit: 317120e6826418d031d6b80a6323e9f17bcaa3d6

## searchdialog.cpp:25:10: fatal error: ui_searchdialog.h: No such file or directory | #include "ui_searchdialog.h"
searchdialog.h will be created from searchdialog.ui so enable UIC creation:
set(CMAKE_AUTOUIC ON)

for auto creation of ressource files:
set(CMAKE_AUTORCC ON)

## /usr/bin/ld: CMakeFiles/qucs.dir/home/martin/GITProjekte/qucs/qucs/main/main.cpp.o: in function `setSimulator(char const*)': /home/martin/GITProjekte/qucs/qucs/main/main.cpp:62: undefined reference to `simulator_dispatcher'
globals.cpp must be added to the qucs executable
target_sources(qucs PRIVATE
	${CMAKE_SOURCE_DIR}/../main/main.cpp
	qucs.cpp
	globals.cpp
	settings.cpp
	)
	
## Assertion raised:
QFile qfl(":/bitmaps/line.png");
assert(qfl.exists());
solution: fa809b3701fce5946ae91bae8ce31e905577e89a

## Legacy.so not found

## ../../../src/settings.h:7:10: fatal error: qt_compat.h: No such file or directory
target_include_directories() #add include directory!

##fatal error: qucs.h: No such file or directory 38 | #include "qucs.h"
in commit: 4dea239df439ca650615f67248b73a5105156fc1
solution: target_include_directories instead of target_link_directories must be used!

