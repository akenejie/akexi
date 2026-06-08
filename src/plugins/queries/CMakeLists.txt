include_directories(${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/widget ${CMAKE_SOURCE_DIR}/src/widget/tableview)

# the main plugin
set(akexi_queryplugin_SRCS
   kexiquerypart.cpp
   kexiquerydesignersql.cpp
   kexiquerydesignerguieditor.cpp
   kexiqueryview.cpp
)

add_library(akexi_queryplugin MODULE ${akexi_queryplugin_SRCS})
kcoreaddons_desktop_to_json(akexi_queryplugin akexi_queryplugin.desktop)

target_link_libraries(akexi_queryplugin
    PRIVATE
        kexiextendedwidgets
        kexidataviewcommon
        kexidatatable
        kexirelationsview

        Qt5::Xml
)

install(TARGETS akexi_queryplugin  DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})
