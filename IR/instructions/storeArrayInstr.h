#ifndef ST_ARR_INSTR
#define ST_ARR_INSTR

#include "instr.h"
#include "instructions/type.h"

namespace ir::instr {
class StoreArrayInstr : public Instr {
public:
    StoreArrayInstr(Instr *array, Instr *index, Instr *value) : Instr(InstrOpcode::ST_ARR, TypeId::VOID)
    {
        AddInput(array);
        AddInput(index);
        AddInput(value);

        if (array != nullptr) {
            array->AddUser(this);
        }
        if (index != nullptr) {
            index->AddUser(this);
        }
        if (value != nullptr) {
            value->AddUser(this);
        }
    }

    void Dump(IrDumper &dumper, bool dumpLiveness = false) override;
};

}

#endif