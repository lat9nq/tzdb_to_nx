add_executable(tzdb2nx
    main.cpp
    tzif.cpp
    tzif.h)

add_compile_options(
    -Werror=all
    -Werror=extra
)

include_directories(.)
