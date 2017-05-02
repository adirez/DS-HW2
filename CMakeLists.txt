cmake_minimum_required(VERSION 3.7)
project(Escapy)

set(CMAKE_C_STANDARD 99)

SET(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pedantic-errors -Werror -DNDEBUG")
SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

set(SOURCE_FILES main.c challenge.c challenge.h constants.h challenge_room_system_fields.h challenge_system.c challenge_system.h challenge_system_test_1.c system_additional_types.h visitor_room.c visitor_room.h)
add_executable(Escapy ${SOURCE_FILES})