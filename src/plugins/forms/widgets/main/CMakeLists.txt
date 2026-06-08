# the main form widgets plugin

set(akexiforms_mainwidgetsplugin_SRCS
    KexiStandardFormWidgets.cpp
    KexiStandardContainerFormWidgets.cpp
    KexiMainFormWidgetsFactory.cpp
    akexiforms_mainwidgetsplugin.json
)

add_library(akenejie.akexi-project.form.widgets.main MODULE ${akexiforms_mainwidgetsplugin_SRCS})

target_link_libraries(akenejie.akexi-project.form.widgets.main
    PRIVATE
        kformdesigner
        kexiformutils
        kexiguiutils
        kexidataviewcommon
        kexidatatable
        kexiextendedwidgets
)

install(TARGETS akenejie.akexi-project.form.widgets.main DESTINATION ${KEXI_FORM_WIDGETS_PLUGIN_INSTALL_DIR})

add_subdirectory(pics)
