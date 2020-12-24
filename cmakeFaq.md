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

## /usr/bin/ld: QucsSchematicLibrary/libqucsschematic.so: undefined reference to `PackageDialog::extractPackage()'
qucs_app CMakeLists.txt. In qucs_lib the error message does not show up

## /usr/bin/ld: QucsSchematicLibrary/libqucsschematic.so: undefined reference to `QPrintDialog::QPrintDialog(QPrinter*, QWidget*)'
qucs_app CMakeLists.txt. In qucs_lib the error message does not show up. But building the CMakeLists.txt of QucsSchematicLibrary in the qucs_app project the same error occurs

solution: in main.cpp there war printerwriter included, but not used. So the header must be removed or the source file must be included into the qucs app

## /usr/bin/ld: QucsSchematicLibrary/libqucsschematic.so: undefined reference to `DiagramDialog::DiagramDialog(QucsDoc*)'
solution: 2bc5a07e6a380c65829fc824e82be58181a3ea1b
add diagrammdialog.cpp to MakeList as src

## /usr/bin/ld: QucsSchematicLibrary/libqucsschematic.so: undefined reference to `QPrinter::QPrinter(QPrinter::PrinterMode)'
I have to link again Printsupport in qucs_app target. Don't understand why

## Error message: /home/martin/GIT/QucsApplicationDebug/libraries/legacy/libqucsatorInterface.so: undefined symbol: _ZTV15QucsatorProcess
Problem was that not all libraries where linkable correctly. For this testapplications can be created, so it is easier to find out why the library cannot be loaded.
See commits: 9800732d32947296955166c88b32c6fe88372931, 18cd0dd65e703f29bf9d7e63d39d3b5184599165, be7e8d48d8c23e35e592d5b23d7a3d4c99d05791 and 45fb573bf19fbb75261aa6b7d9dddc701d228473

