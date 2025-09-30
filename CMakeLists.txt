project(jit-aot-compiler)
cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(IR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/IR)
set(INSTR_DIR ${IR_DIR}/instructions)
set(UTIL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/utils)

set(IR_SOURCES
${INSTR_DIR}/binaryOperationInstr.cpp
${INSTR_DIR}/callInstr.cpp
${INSTR_DIR}/castInstr.cpp
${INSTR_DIR}/constantInstr.cpp
${INSTR_DIR}/ifInstr.cpp
${INSTR_DIR}/paramInstr.cpp
${INSTR_DIR}/phiInstr.cpp
${INSTR_DIR}/returnInstr.cpp
${IR_DIR}/graph.cpp
${IR_DIR}/irDumper.cpp
${IR_DIR}/basicBlock.cpp
)

set(IR_LIB_DIRS
${IR_DIR}
${UTIL_DIR}
)

add_subdirectory(tests)

find_program(CLANG_FORMAT clang-format)

if(CLANG_FORMAT)
    file(GLOB_RECURSE ALL_SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    )

    add_custom_target(clang-force-format
        COMMAND ${CLANG_FORMAT} -i -style=file ${ALL_SOURCE_FILES}
        COMMENT "Formatting all C/C++ source and header files with clang-format"
    )
    message(STATUS "clang-format found. 'clang-force-format' target has been added.")
else()
    message(WARNING "clang-format not found. The 'clang-force-format' target will not be available.")
endif()