include_directories(
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/widget
)

# the main plugin
set(akexi_reportplugin_SRCS
    kexireports.cpp
    kexireportpart.cpp
    kexireportview.cpp
    kexireportdesignview.cpp
    KexiDBReportDataSource.cpp
    kexisourceselector.cpp
    krscriptfunctions.cpp
)

if(SHOULD_BUILD_KEXI_DESKTOP_APP)
#TODO KEXI3
#  list(APPEND akexi_reportplugin_SRCS
#        keximigratereportdata.cpp
#  )
endif ()

#TODO KEXI3 qt5_wrap_cpp(akexi_reportplugin_SRCS ../scripting/kexiscripting/kexiscriptadaptor.h)

add_library(akexi_reportplugin MODULE ${akexi_reportplugin_SRCS})
kcoreaddons_desktop_to_json(akexi_reportplugin akexi_reportplugin.desktop)

target_link_libraries(akexi_reportplugin
    PRIVATE
        kexiguiutils
        kexiextendedwidgets

        KReport
)

if(SHOULD_BUILD_KEXI_DESKTOP_APP)
    target_link_libraries(akexi_reportplugin
      PRIVATE
        keximain
#TODO KEXI3       akeximigrate
    )
endif()

if(NOT KEXI_USE_KFILEWIDGET)
    target_link_libraries(akexi_reportplugin
        PRIVATE KF5::KIOFileWidgets) # KFileWidget::getStartUrl
endif()

install(TARGETS akexi_reportplugin DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})
