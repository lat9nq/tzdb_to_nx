set(TZ_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tz")
set(TZ_DIR "${CMAKE_CURRENT_BINARY_DIR}/tz")
set(TZ_ZONEINFO_DIR "${TZ_DIR}/usr/share/zoneinfo" CACHE PATH "Time zone info data directory")
set(TZIF_LIST_FILE "${CMAKE_CURRENT_BINARY_DIR}/tzif_list.txt" CACHE PATH "List of zone info files")

find_program(GNU_MAKE make)
if ("${GNU_MAKE}" STREQUAL "GNU_MAKE-NOTFOUND")
    message(FATAL_ERROR "GNU make not found")
endif()

if (NOT EXISTS "${TZ_DIR}" OR NOT EXISTS "${TZIF_LIST_FILE}")
    execute_process(
        COMMAND
            ${GNU_MAKE} DESTDIR=${TZ_DIR} install
        WORKING_DIRECTORY 
            ${TZ_SOURCE_DIR}
        COMMAND_ERROR_IS_FATAL ANY
    )

    # Step taken by Arch Linux packaging, but Nintendo apparently skips it
    # execute_process(
    #     COMMAND
    #         "${TZDB_LOCATION}/zic" -b fat -d ${TZDB_ZONEINFO} africa antarctica asia australasia europe northamerica southamerica etcetera backward factory
    #     WORKING_DIRECTORY
    #         "${TZDB_LOCATION}"
    #     COMMAND_ERROR_IS_FATAL ANY
    # )

    execute_process(
        COMMAND
            /bin/bash "${CMAKE_CURRENT_SOURCE_DIR}/find_tzif.sh" "${TZ_ZONEINFO_DIR}" "${TZIF_LIST_FILE}"
        COMMAND_ERROR_IS_FATAL ANY
    )
endif()
