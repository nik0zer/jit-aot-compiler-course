project(jit-aot-compiler)
cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(IR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/IR)

set(IR_SOURCES

)

set(IR_LIB_DIRS
${IR_DIR}
)

add_subdirectory(tests)