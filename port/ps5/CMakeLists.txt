# Xerenge PS5: what only the console build has. GPL-3.0-or-later.
add_library(xerenge-ps5 STATIC
    src/code_bytes_ps5.cpp
    src/filesystem_ps5.cpp
    src/heap_watch_ps5.cpp
    src/memory_ps5.cpp
    src/pad_ps5.cpp
    src/ps5_startup.cpp)
target_include_directories(xerenge-ps5 PRIVATE ../../rexglue/src)
target_link_libraries(xerenge-ps5 PRIVATE SDL3::SDL3)
