# this will make sure that one is using the clang compiler
set(CMAKE_CXX_COMPILER  "/usr/bin/clang++")
set(CMAKE_C_COMPILER "/usr/bin/clang")

project(brooks)

set(CMAKE_BINARY_DIR          ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH    ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_TEMP_OUTPUT_DIR     ${CMAKE_SOURCE_DIR}/cmake)

set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_TEMP_OUTPUT_DIR})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -std=c11 -Wall -Werror")

cmake_minimum_required(
    VERSION 3.5
)

find_package(Doxygen REQUIRED)

include_directories(
    "include"
)

option(BUILD_DOCUMENTATION "API documentation requires Doxygen" ${DOXYGEN_FOUND})

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR})

set(SOURCE_FILES
        include/brooks/brooks_doc.h
        src/brooks/brooks_doc.c
        include/brooks/brooks_query.h
        src/brooks/brooks_query.c
        include/brooks/brooks.h
        include/brooks/brooks_pool.h
        src/brooks/brooks_pool.c
        include/brooks/brooks_misc.h
        src/brooks/brooks_misc.c include/brooks/query/brooks_operator.h include/brooks/query/brooks_cursor.h src/brooks/query/brooks_cursor.c src/brooks/query/brooks_operator.c include/brooks/query/operators/scans/brooks_scan_objects.h include/brooks/query/operators/scans/brooks_scan_arrays.h include/brooks/query/operators/scans/brooks_scan_strings.h src/brooks/query/operators/scans/brooks_scan_objects.c src/brooks/query/operators/scans/brooks_scan_array.c include/opendsb/odsb_datagen.h )


add_executable(
    opendsb
    include/opendsb/main.c
    ${SOURCE_FILES}
)

add_executable(
    samples-basics
    samples/sample-basics.c
    ${SOURCE_FILES}
)

if(DOXYGEN_FOUND)
    add_custom_target(
        doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif(DOXYGEN_FOUND)