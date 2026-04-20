#ifndef NEW_ARR_INSTR
#define NEW_ARR_INSTR

#include "instr.h"
#include "instructions/type.h"

namespace ir::instr {
class NewArrayInstr : public Instr {
public:
    NewArrayInstr(TypeId elementType, Instr *size) : Instr(InstrOpcode::NEW_ARR, TypeId::REF), elementType_(elementType)
    {
        AddInput(size);
        if (size != nullptr) {
            size->AddUser(this);
        }
    }

    TypeId GetElementType() const { return elementType_; }
    void SetElementType(TypeId elementType) { elementType_ = elementType; }

    void Dump(IrDumper &dumper, bool dumpLiveness = false) override;
private:
    TypeId elementType_;
};

}

#endif