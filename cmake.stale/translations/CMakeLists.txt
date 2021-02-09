#
# Handle Qucs translations

# TODO - get list of .cpp files to refresh/update the .ts files - evaluate need
# to merge the qtgeneric_XX.ts into the final .qm (see translations/Makefile.am)

#
# Qucs translations
#
file(GLOB TRANSLATIONS_FILES qucs_*.ts)

# WARNING: make clean might delete the source .ts files! Danger!
option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts" "OFF")

if(UPDATE_TRANSLATIONS)
  qt4_create_translation(QM_FILES ${FILES_TO_TRANSLATE} ${TRANSLATIONS_FILES})
else(UPDATE_TRANSLATIONS)
  qt4_add_translation(QM_FILES ${TRANSLATIONS_FILES})
endif(UPDATE_TRANSLATIONS)

add_custom_target(translations ALL DEPENDS ${QM_FILES})

if(UPDATE_TRANSLATIONS)
  add_custom_target(translations_src ALL DEPENDS ${Tano_QMS_SRC})
endif(UPDATE_TRANSLATIONS)

#
# Qt translation
#
# FILE(GLOB QtTRANSLATIONS qtgeneric_*.ts)

#
# install tranlations
install(FILES ${QM_FILES} DESTINATION share/qucs/lang)

# prevent the generated files from being deleted during make clean
# set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM true)
