set(TZ_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tz" CACHE PATH "Time zone source directory")
set(TZ_DIR "${CMAKE_CURRENT_BINARY_DIR}/tz")
set(TZ_TMP_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/tmpsrc")
set(TZ_ZONEINFO_DIR "${TZ_DIR}/usr/share/zoneinfo" CACHE PATH "Time zone info data directory")
set(TZIF_LIST_FILE "${CMAKE_CURRENT_BINARY_DIR}/tzif_list.txt" CACHE PATH "List of zone info files")

find_program(GNU_MAKE make)
if ("${GNU_MAKE}" STREQUAL "GNU_MAKE-NOTFOUND")
    message(FATAL_ERROR "GNU make not found")
endif()

if (NOT EXISTS "${TZ_DIR}" OR NOT EXISTS "${TZIF_LIST_FILE}")
    # tz's makefile can only build in-tree, so copy the whole source tree to a
    # separate directory before building.
    file(COPY ${TZ_SOURCE_DIR}/ DESTINATION ${TZ_TMP_SOURCE_DIR})

    execute_process(
        COMMAND
            ${GNU_MAKE} DESTDIR=${TZ_DIR} install
        WORKING_DIRECTORY 
            ${TZ_TMP_SOURCE_DIR}
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
           ${CMAKE_COMMAND} -P ${PROJECT_SOURCE_DIR}/CMakeModules/list_directory.cmake false ON
        WORKING_DIRECTORY
            "${TZ_ZONEINFO_DIR}"
        OUTPUT_VARIABLE
            TZIF_SCAN
    )

    set(TZIF_LIST "")
    foreach(CANDIDATE ${TZIF_SCAN})
        if (CANDIDATE STREQUAL "\n")
            continue()
        endif()
        set(TZIF_FILE "${TZ_ZONEINFO_DIR}/${CANDIDATE}")
        file(READ "${TZIF_FILE}" HEADER LIMIT 4)
        string(SUBSTRING "${HEADER}" 0 4 HEADER) # Remove trailing newline
        if (HEADER STREQUAL "TZif")
            file(APPEND "${TZIF_LIST_FILE}" "${TZIF_FILE}\n")
        endif()
    endforeach()
endif()
