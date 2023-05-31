# tzdb_to_nx

This is a CMake/C++ project to convert RFC 8536 time zone data to the Nintendo Switch's format.
This makes use a lot of Unix system calls as well as a bash script to convert the data, so it likely requires a bit of work to port to a non-POSIX platform, such as Windows.

Intended for use with the [yuzu Emulator](https://yuzu-emu.org/) project, but the project in the future likely won't ship synthesized Switch archives.
That leaves this project in a place where it is not likely to be used, but will remain here as a reference.
