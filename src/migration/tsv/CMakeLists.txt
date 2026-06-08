set(akeximigrate_tsv_PART_SRCS
    TsvMigrate.cpp
    akeximigrate_tsv.json
    Messages.sh
)

build_and_install_akexi_migrate_driver(tsv
    "${akeximigrate_tsv_PART_SRCS}"
    ""
    ""
    ""
)
