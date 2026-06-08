include_directories(
    ${CMAKE_SOURCE_DIR}/src/formeditor
    ${CMAKE_SOURCE_DIR}/src/core
    ${MARBLE_INCLUDE_DIR}
)

set(akexiforms_mapwidgetplugin_SRCS
    MapBrowserWidget.cpp
    MapBrowserFactory.cpp
    Messages.sh
)

add_library(akexiforms_mapwidgetplugin MODULE ${akexiforms_mapwidgetplugin_SRCS})
kcoreaddons_desktop_to_json(akexiforms_mapwidgetplugin akexiforms_mapwidgetplugin.desktop)

target_link_libraries(akexiforms_mapwidgetplugin
    kformdesigner
    kexiguiutils
    kexidatatable
    kexiextendedwidgets
    kexiformutils
    kexidataviewcommon
    ${MARBLE_LIBRARIES}

    Qt5::Xml
)

install(TARGETS akexiforms_mapwidgetplugin DESTINATION ${KEXI_FORM_WIDGETS_PLUGIN_INSTALL_DIR})

ecm_install_icons(ICONS
    16-actions-map_browser.png
    22-actions-map_browser.png
    DESTINATION ${DATA_INSTALL_DIR}/kexi/icons
    THEME hicolor
)
