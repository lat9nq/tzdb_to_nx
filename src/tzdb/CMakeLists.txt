find_program(GIT_PROGRAM git)
if ("${GIT_PROGRAM}" STREQUAL "GIT_PROGRAM-NOTFOUND")
    message(FATAL_ERROR "git program not found")
endif()

find_program(GNU_DATE date)
if ("${GNU_DATE}" STREQUAL "GNU_DATE-NOTFOUND")
    message(FATAL_ERROR "date program not found")
endif()

set(NX_TZDB_DIR "${CMAKE_CURRENT_BINARY_DIR}/nx" CACHE PATH "Path to Switch-style time zone data")
set(NX_ZONEINFO_DIR "${NX_TZDB_DIR}/zoneinfo")

set(TZDB_VERSION_FILE ${TZ_SOURCE_DIR}/NEWS)

execute_process(
    COMMAND 
        ${GIT_PROGRAM} log --pretty=%at -n1 NEWS
    OUTPUT_VARIABLE
        TZ_COMMIT_TIME
    WORKING_DIRECTORY
        ${TZ_SOURCE_DIR}
    COMMAND_ERROR_IS_FATAL ANY)

string(REPLACE "\n" "" TZ_COMMIT_TIME "${TZ_COMMIT_TIME}")

execute_process(
    COMMAND
        ${GNU_DATE} +%y%m%d --date=@${TZ_COMMIT_TIME}
    OUTPUT_VARIABLE
        TZDB_VERSION
    COMMAND_ERROR_IS_FATAL ANY)

set(NX_VERSION_FILE ${NX_TZDB_DIR}/version.txt)
file(WRITE ${NX_VERSION_FILE} "${TZDB_VERSION}")

add_custom_target(x80e 
    ALL
    DEPENDS
        tzdb2nx
        ${NX_VERSION_FILE})

set(BINARY_LIST_TXT ${NX_TZDB_DIR}/binaryList.txt)
add_custom_target(binary_list
        bash ${CMAKE_CURRENT_SOURCE_DIR}/generate_binary_list_txt.sh ${BINARY_LIST_TXT}
    BYPRODUCTS
        ${BINARY_LIST_TXT}
    WORKING_DIRECTORY
        ${NX_ZONEINFO_DIR})
add_dependencies(x80e binary_list)

file(STRINGS "${TZIF_LIST_FILE}" TZ_FILES)
foreach(FILE ${TZ_FILES})
    file(RELATIVE_PATH TARG "${TZ_ZONEINFO_DIR}" "${FILE}")
    get_filename_component(TARG_PATH "${NX_ZONEINFO_DIR}/${TARG}" DIRECTORY)
    string(REGEX REPLACE "\/" "_" TARG_SANITIZED "${TARG}")
    add_custom_target(${TARG_SANITIZED}
        BYPRODUCTS
            ${NX_ZONEINFO_DIR}/${TARG}
        COMMAND
            mkdir -p ${TARG_PATH}
        COMMAND
            ${TZDB2NX_PATH} ${FILE} ${NX_ZONEINFO_DIR}/${TARG}
        DEPENDS
            tzdb2nx)

    add_dependencies(x80e ${TARG_SANITIZED})
    add_dependencies(binary_list ${TARG_SANITIZED})
endforeach()
