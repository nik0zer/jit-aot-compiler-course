#ifndef INSTR_MAPPING_H
#define INSTR_MAPPING_H

#define INSTR_MAPPING(_)                                                       \
  _(PARAM, ParamInstr)                                                         \
  _(CONSTANT, ConstantInstr)                                                   \
  _(CAST, CastInstr)                                                           \
  _(BINARY_OPERATION, BinaryOperationInstr)                                    \
  _(RETURN, ReturnInstr)                                                       \
  _(CALL_STATIC, CallStaticInstr)                                              \
  _(IF, IfInstr)                                                               \
  _(NEW_ARR, NewArrayInstr)                                                    \
  _(PHI, PhiInstr)                                                             \
  _(LD_ARR, LoadArrayInstr)                                                    \
  _(ST_ARR, StoreArrayInstr)                                                   \
  _(NEW_STR, NewStringInstr)                                                   \
  _(NULL_INSTR, NullInstr)                                                     \
  _(CHECK_NULL, CheckNullInstr)                                                \
  _(CHECK_BOUNDS, CheckBoundsInstr)

#endif