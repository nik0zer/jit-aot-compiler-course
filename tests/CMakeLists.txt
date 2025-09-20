add_executable(ir_build_factorial ir_build_factorial.cpp ${IR_SOURCES})
target_include_directories(ir_build_factorial PRIVATE ${IR_LIB_DIRS})