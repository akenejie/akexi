include_directories( ${CMAKE_SOURCE_DIR}/src/core ${CMAKE_SOURCE_DIR}/src/widget ${CMAKE_SOURCE_DIR}/src/migration)

set(akexi_migrationplugin_PART_SRCS keximigrationpart.cpp)

add_library(akexi_migrationplugin MODULE ${akexi_migrationplugin_PART_SRCS})
kcoreaddons_desktop_to_json(akexi_migrationplugin akexi_migrationplugin.desktop)

target_link_libraries(akexi_migrationplugin
    PRIVATE
        akeximigrate
)

install(TARGETS akexi_migrationplugin DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})
