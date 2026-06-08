# the web browser form widgets plugin

include_directories(
    ${CMAKE_SOURCE_DIR}/src/formeditor
    ${CMAKE_SOURCE_DIR}/src/core
)

set(akexiforms_webbrowserwidgetplugin_SRCS
    WebBrowserWidget.cpp
    WebBrowserFactory.cpp
    akexiforms_webbrowserwidgetplugin.json
    Messages.sh
)

add_library(akenejie.akexi-project.form.widgets.web-browser MODULE ${akexiforms_webbrowserwidgetplugin_SRCS})

set (QT_USE_QTWEBKIT TRUE)

target_link_libraries(akenejie.akexi-project.form.widgets.web-browser
    kformdesigner
    kexiguiutils
    kexidatatable
    kexiextendedwidgets
    kexidataviewcommon
    kexiformutils

    Qt5::WebKitWidgets
    Qt5::Xml
)

install(TARGETS akenejie.akexi-project.form.widgets.web-browser DESTINATION ${KEXI_FORM_WIDGETS_PLUGIN_INSTALL_DIR})

add_subdirectory(pics)
