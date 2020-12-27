# everything related to install the qucs application
# any dependencies are installed here
if(UNIX AND NOT APPLE)
	set(DESKTOP ${CMAKE_CURRENT_SOURCE_DIR}/../../contrib/qucs.desktop)
	install(FILES ${DESKTOP}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
		
    set(BITMAPS_HICOLOR_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../bitmaps/hicolor)
    set(BITMAPS_HICOLOR_DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor)

	install(FILES ${BITMAPS_HICOLOR_PATH}/16x16/apps/qucs.png
		DESTINATION ${BITMAPS_HICOLOR_DESTINATION}/16x16/apps)
	install(FILES ${BITMAPS_HICOLOR_PATH}/22x22/apps/qucs.png
		DESTINATION ${BITMAPS_HICOLOR_DESTINATION}/22x22/apps)
	install(FILES ${BITMAPS_HICOLOR_PATH}/32x32/apps/qucs.png
		DESTINATION ${BITMAPS_HICOLOR_DESTINATION}/32x32/apps)
	install(FILES ${BITMAPS_HICOLOR_PATH}/48x48/apps/qucs.png
		DESTINATION ${BITMAPS_HICOLOR_DESTINATION}/48x48/apps)
	install(FILES ${BITMAPS_HICOLOR_PATH}/64x64/apps/qucs.png
		DESTINATION ${BITMAPS_HICOLOR_DESTINATION}/64x64/apps)
	install(FILES ${BITMAPS_HICOLOR_PATH}/128x128/apps/qucs.png
		DESTINATION ${BITMAPS_HICOLOR_DESTINATION}/128x128/apps)
	install(FILES ${BITMAPS_HICOLOR_PATH}/256x256/apps/qucs.png
		DESTINATION ${BITMAPS_HICOLOR_DESTINATION}/256x256/apps)
	install(FILES ${BITMAPS_HICOLOR_PATH}/512x512/apps/qucs.png
		DESTINATION ${BITMAPS_HICOLOR_DESTINATION}/512x512/apps)
	install(FILES ${BITMAPS_HICOLOR_PATH}/scalable/apps/qucs.svg
		DESTINATION ${BITMAPS_HICOLOR_DESTINATION}/scalable/apps)
endif()



##
## Prepare the installation
##
#set(plugin_dest_dir bin)
#set(qtconf_dest_dir bin)
#set(APPS "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}")
#if(APPLE)
#  set(plugin_dest_dir ${PROJECT_NAME}.app/Contents/MacOS)
#  set(qtconf_dest_dir ${PROJECT_NAME}.app/Contents/Resources)
#  set(APPS "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.app")
#endif(APPLE)

#if(WIN32)
#  set(APPS "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.exe")
#endif(WIN32)

#
# Install the Qucs application, on Apple, the bundle is installed as on other
# platforms it'll go into the bin directory.
#
install(
  TARGETS qucs
  BUNDLE DESTINATION bin COMPONENT Runtime
  RUNTIME DESTINATION bin COMPONENT Runtime)

##
## Install needed Qt plugins by copying directories from the qt installation One
## can cull what gets copied by using 'REGEX "..." EXCLUDE'
##
#if(APPLE)
#  install(
#    DIRECTORY "${QT_PLUGINS_DIR}/imageformats"
#    DESTINATION bin/${plugin_dest_dir}/plugins
#    COMPONENT Runtime)
#endif()
##
## install a qt.conf file this inserts some cmake code into the install script to
## write the file
##
#if(APPLE)
#  install(CODE "
#    file(WRITE \"\${CMAKE_INSTALL_PREFIX}/bin/${qtconf_dest_dir}/qt.conf\" \"\")
#    " COMPONENT Runtime)
#endif()

## ------------------------------------------------------------------------------
## --
## Use BundleUtilities to get all other dependencies for the application to work.
## It takes a bundle or executable along with possible plugins and inspects it
## for dependencies.  If they are not system dependencies, they are copied.

## directories to look for dependencies
#if(APPLE)
#  set(DIRS ${QT_LIBRARY_DIRS})
#endif()

## Now the work of copying dependencies into the bundle/package The quotes are
## escaped and variables to use at install time have their $ escaped An
## alternative is the do a configure_file() on a script and use install(SCRIPT
## ...). Note that the image plugins depend on QtSvg and QtXml, and it got those
## copied over.
#if(APPLE)
#  install(
#    CODE "
#    file(GLOB_RECURSE QTPLUGINS
#      \"\${CMAKE_INSTALL_PREFIX}/bin/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
#    set(BU_CHMOD_BUNDLE_ITEMS ON)
#    include(BundleUtilities)
#    fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
#    "
#    COMPONENT Runtime)
#endif()

## Install wrapper scripts
#if(WIN32)
#  set(SCRIPTS qucsdigi.bat qucsveri.bat qucsdigilib.bat)
#else()
#  set(SCRIPTS qucsdigi qucsveri qucsdigilib)
#endif()
#install(FILES ${SCRIPTS} DESTINATION bin/)

## To Create a package, one can run "cpack -G DragNDrop CPackConfig.cmake" on Mac
## OS X where CPackConfig.cmake is created by including CPack And then there's
## ways to customize this as well set(CPACK_BINARY_DRAGNDROP ON) include(CPack)
