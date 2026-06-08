set(akeximigrate_mysql_PART_SRCS
    mysqlmigrate.cpp
    akeximigrate_mysql.json
    Messages.sh
)

build_and_install_akexi_migrate_driver(mysql
    "${akeximigrate_mysql_PART_SRCS}"
    "${MYSQL_LIBRARIES}"
    "${MYSQL_INCLUDE_DIR}"
    ""
)
