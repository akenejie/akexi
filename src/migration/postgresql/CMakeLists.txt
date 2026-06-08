set(akeximigrate_postgresql_PART_SRCS
    PostgresqlMigrate.cpp
    akeximigrate_postgresql.json
    Messages.sh
)

build_and_install_akexi_migrate_driver(postgresql
    "${akeximigrate_postgresql_PART_SRCS}"
    "${POSTGRESQL_LIBRARIES}"
    "${PostgreSQL_INCLUDE_DIRS}"
    ""
)
