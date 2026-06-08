add_definitions(-DTRANSLATION_DOMAIN=\"akeximigrate_spreadsheet\")

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/libs
    ${KOMAIN_INCLUDES}
)


########### next target ###############

set(akeximigrate_spreadsheet_PART_SRCS
    spreadsheetmigrate.cpp
    Messages.sh
)


add_library(akeximigrate_spreadsheet MODULE ${akeximigrate_spreadsheet_PART_SRCS})

kcoreaddons_desktop_to_json(akeximigrate_spreadsheet akeximigrate_spreadsheet.desktop)

target_link_libraries(akeximigrate_spreadsheet
    akeximigrate
    kexiutils
    calligrasheetscommon

    Predicate
)

install(TARGETS akeximigrate_spreadsheet DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})
