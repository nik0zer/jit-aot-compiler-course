set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(UTIL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test_utils)

set (TESTS_LIB_DIRS
    ${UTIL_DIR}
)

set (TESTS_LIB_SRCS
    ${UTIL_DIR}/util.cpp
)

set(RUN_MODE "RUN")
set(EXPECTED_MODE "EXPECTED")
set(NONE_SOURCE_FILE "_none_")

set(TESTS_DATA
    #"test_name test_file test_mode"
    "ir_build_factorial ir_build_factorial.cpp ${EXPECTED_MODE} ${NONE_SOURCE_FILE}"
    "find_dominators find_dominators.cpp ${EXPECTED_MODE} ${NONE_SOURCE_FILE}"
    "find_loops find_loops.cpp ${EXPECTED_MODE} ${NONE_SOURCE_FILE}"
    "constant_folding constant_folding.cpp ${EXPECTED_MODE} ${NONE_SOURCE_FILE}"
    "peepholes peepholes.cpp ${EXPECTED_MODE} ${NONE_SOURCE_FILE}"
    "source_ir_builder_1 source_ir_builder_1.cpp ${EXPECTED_MODE} ./unknownFile.fa"
    "source_ir_builder_2 source_ir_builder_2.cpp ${EXPECTED_MODE} ./source_ir_builder_2.fa"
    "source_ir_builder_3 source_ir_builder_3.cpp ${EXPECTED_MODE} ./source_ir_builder_3.fa"
    "source_ir_builder_4 source_ir_builder_4.cpp ${EXPECTED_MODE} ./source_ir_builder_4.fa"
    "source_ir_builder_5 source_ir_builder_5.cpp ${EXPECTED_MODE} ./source_ir_builder_5.fa"
)

foreach(TEST_DATA IN ITEMS ${TESTS_DATA})
    string(REPLACE " " ";" TEST_DATA_ELEM "${TEST_DATA}")
    list(GET TEST_DATA_ELEM 0 TEST_NAME)
    list(GET TEST_DATA_ELEM 1 TEST_SRC_FILE)
    list(GET TEST_DATA_ELEM 2 MODE)

    add_executable(${TEST_NAME} ${TEST_SRC_FILE} ${IR_SOURCES} ${PASSES_SOURCES} ${ANALYZERS_SOURCES} ${TESTS_LIB_SRCS}
    ${IR_BUILDER_SOURCES})
    target_include_directories(${TEST_NAME} PRIVATE ${IR_LIB_DIRS})
    target_include_directories(${TEST_NAME} PRIVATE ${ANALYZERS_LIB_DIRS})
    target_include_directories(${TEST_NAME} PRIVATE ${PASSES_LIB_DIRS})
    target_include_directories(${TEST_NAME} PRIVATE ${TESTS_LIB_DIRS})
    target_include_directories(${TEST_NAME} PRIVATE ${IR_BUILDER_LIB_DIRS})
endforeach()

add_custom_target(tests_all)

foreach(TEST_DATA IN ITEMS ${TESTS_DATA})
    string(REPLACE " " ";" TEST_DATA_ELEM "${TEST_DATA}")
    list(GET TEST_DATA_ELEM 0 TEST_NAME)
    list(GET TEST_DATA_ELEM 1 TEST_SRC_FILE)
    list(GET TEST_DATA_ELEM 2 MODE)
    list(GET TEST_DATA_ELEM 3 SOURCE_FILE_PATH)
    add_custom_target(${TEST_NAME}-test
        DEPENDS ${TEST_NAME}
    )

    add_custom_target(run-${TEST_NAME}
        COMMAND ${CMAKE_COMMAND} -E env "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/lib"
                ./${TEST_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE_PATH}
                >
                ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.out
        DEPENDS ${TEST_NAME}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    )
    add_dependencies(${TEST_NAME}-test run-${TEST_NAME})
    if(MODE STREQUAL ${EXPECTED_MODE})
        add_custom_target(check-test-expected-${TEST_NAME}
            COMMAND ${CMAKE_COMMAND} -E compare_files
                    "${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.out"
                    "${CMAKE_CURRENT_SOURCE_DIR}/${TEST_NAME}-expected.txt"
        )
        add_dependencies(check-test-expected-${TEST_NAME} run-${TEST_NAME})
        add_dependencies(${TEST_NAME}-test check-test-expected-${TEST_NAME})
    endif()
    add_dependencies(tests_all ${TEST_NAME}-test)
endforeach()