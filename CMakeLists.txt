cmake_minimum_required(VERSION 2.8)
project(ftpupload)

if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set (CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS}")
    endif ()
else ()
    set (CMAKE_C_STANDARD 99)
endif ()

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c notify.h notify.c def.h config.h config.c curlftp.h curlftp.c log.h log.c)

add_executable(ftpupload ${SOURCE_FILES})
target_link_libraries(ftpupload curl)
target_link_libraries(ftpupload pthread)
