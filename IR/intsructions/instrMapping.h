#ifndef INSTR_MAPPING_H
#define INSTR_MAPPING_H

#define INSTR_MAPPING(_)                                                                \
    _(PARAM, ParamInstr)                                                                \
    _(CONSTANT, ConstantInstr)                                                          \
    _(CAST, CastInstr)                                                                  \
    _(BINARY_OPERATION, BinaryOperationInstr)                                           \
    _(RETURN, ReturnInstr)                                                              \
    _(CALL, CallInstr)                                                                  \
    _(JUMP, JumpInstr)                                                                  \
    _(IF, IfInstr)                                                                      \
    _(PHI, PhiInstr)

#endif