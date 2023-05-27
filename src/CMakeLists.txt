add_compile_options(
    -Werror=all
    -Werror=extra

    -Werror=shadow
)

add_executable(tzdb2nx
    main.cpp
    tzif.cpp
    tzif.h)

include_directories(.)
