cmake_minimum_required(VERSION 3.0)
project (pw_bot)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

file(GLOB_RECURSE SRC_FILES
    "src/*.h"
    "src/*.cc"
)

add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${CMAKE_PROJECT_NAME} ${CONAN_LIBS})
