include_directories(${XBase_INCLUDE_DIR})

########### next target ###############

set(akeximigrate_xbase_PART_SRCS
    xbasemigrate.cpp
    Messages.sh
)

add_library(akeximigrate_xbase MODULE ${akeximigrate_xbase_PART_SRCS})


target_link_libraries(akeximigrate_xbase
    akeximigrate
    kexiutils

    KDb

    ${XBase_LIBRARIES}
)

install(TARGETS akeximigrate_xbase DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})
