find_program(GNU_MAKE make)
if ("${GNU_MAKE}" STREQUAL "GNU_MAKE-NOTFOUND")
    message(ERROR "make program not found")
endif()

set(TZDB_ARCHIVE "tzdb-${TZDB_VERSION}.tar.lz")
set(TZDB_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/tzdb-${TZDB_VERSION}")
if (NOT (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${TZDB_ARCHIVE}" AND EXISTS "${TZDB_LOCATION}"))
    file(DOWNLOAD "https://data.iana.org/time-zones/releases/${TZDB_ARCHIVE}" "${CMAKE_CURRENT_BINARY_DIR}/${TZDB_ARCHIVE}")
    file(ARCHIVE_EXTRACT INPUT "${CMAKE_CURRENT_BINARY_DIR}/${TZDB_ARCHIVE}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()

set(TZDB_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/build")
set(TZDB_ZONEINFO "${TZDB_BUILD_DIR}/usr/share/zoneinfo")
set(TZIF_LIST_FILE "${CMAKE_CURRENT_BINARY_DIR}/tzif_list.txt")

set(NX_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/nx")

add_custom_target(x80e 
    ALL
    DEPENDS
        tzdb2nx)

if (NOT EXISTS "${TZDB_BUILD_DIR}" OR NOT EXISTS ${TZIF_LIST_FILE})
    execute_process(
        COMMAND
            "${GNU_MAKE}" DESTDIR=${TZDB_BUILD_DIR} install
        WORKING_DIRECTORY 
            "${TZDB_LOCATION}"
        COMMAND_ERROR_IS_FATAL ANY
    )

    # execute_process(
    #     COMMAND
    #         "${TZDB_LOCATION}/zic" -b fat -d ${TZDB_ZONEINFO} africa antarctica asia australasia europe northamerica southamerica etcetera backward factory
    #     WORKING_DIRECTORY
    #         "${TZDB_LOCATION}"
    #     COMMAND_ERROR_IS_FATAL ANY
    # )

    execute_process(
        COMMAND
            /bin/bash "${CMAKE_CURRENT_SOURCE_DIR}/find_tzif.sh" "${TZDB_ZONEINFO}" "${TZIF_LIST_FILE}"
        COMMAND_ERROR_IS_FATAL ANY
    )
endif()

file(STRINGS "${TZIF_LIST_FILE}" TZ_FILES)
foreach(FILE ${TZ_FILES})
    file(RELATIVE_PATH TARG "${TZDB_ZONEINFO}" "${FILE}")
    get_filename_component(TARG_PATH "${NX_BUILD_DIR}/${TARG}" DIRECTORY)
    string(REGEX REPLACE "\/" "_" TARG_SANITIZED "${TARG}")
    add_custom_target(${TARG_SANITIZED}
        BYPRODUCTS
            ${NX_BUILD_DIR}/${TARG}
        COMMAND
            mkdir -p ${TARG_PATH}
        COMMAND
            "${PROJECT_BINARY_DIR}/src/tzdb2nx/tzdb2nx" ${FILE} ${NX_BUILD_DIR}/${TARG}
        DEPENDS
            tzdb2nx
    )

    add_dependencies(x80e ${TARG_SANITIZED})
endforeach()
