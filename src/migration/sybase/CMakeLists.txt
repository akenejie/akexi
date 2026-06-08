include_directories( ${FreeTDS_INCLUDE_DIR} )

########### next target ###############

set(akeximigrate_sybase_PART_SRCS
    sybasemigrate.cpp
    Messages.sh
)

add_library(akeximigrate_sybase MODULE ${akeximigrate_sybase_PART_SRCS})

target_link_libraries(akeximigrate_sybase ${FreeTDS_LIBRARIES}
                      kexiutils akeximigrate KDb)

install(TARGETS akeximigrate_sybase  DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})
