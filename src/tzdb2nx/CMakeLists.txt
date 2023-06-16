add_executable(tzdb2nx
    main.cpp
    tzif.cpp
    tzif.h)

set(TZDB2NX_PATH "${CMAKE_CURRENT_BINARY_DIR}/tzdb2nx" CACHE PATH "Path to tzdb2nx path")
set_target_properties(tzdb2nx PROPERTIES CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TZDB2NX_PATH})
