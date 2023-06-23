set(WITH_DIRECTORIES ${CMAKE_ARGV3})
set(RECURSE ${CMAKE_ARGV4})

set(HOW_TO_GLOB "GLOB")
if (RECURSE)
    set(HOW_TO_GLOB "GLOB_RECURSE")
endif()

file(${HOW_TO_GLOB} FILE_LIST LIST_DIRECTORIES ${WITH_DIRECTORIES} RELATIVE ${CMAKE_SOURCE_DIR} "*")
execute_process(COMMAND ${CMAKE_COMMAND} -E echo "${FILE_LIST};")