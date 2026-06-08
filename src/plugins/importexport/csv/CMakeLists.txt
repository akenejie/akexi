include_directories(${CMAKE_SOURCE_DIR}/src/core ${CMAKE_SOURCE_DIR}/src/widget)

# the main plugin
set(akexi_csvimportexportplugin_SRCS
   KexiCsvImportExportPlugin.cpp
   KexiCSVImportDialogModel.cpp
   kexicsvimportdialog.cpp
   kexicsvimportoptionsdlg.cpp
   kexicsvwidgets.cpp
   kexicsvexportwizard.cpp
   kexicsvexport.cpp
)

add_library(akexi_csvimportexportplugin MODULE ${akexi_csvimportexportplugin_SRCS})
kcoreaddons_desktop_to_json(akexi_csvimportexportplugin akexi_csvimportexportplugin.desktop)

target_link_libraries(akexi_csvimportexportplugin
    PRIVATE
        kexiextendedwidgets
        keximain

        KF5::Codecs
        KF5::KIOWidgets #for KIO::pixmapForUrl...
)

install(TARGETS akexi_csvimportexportplugin  DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})
