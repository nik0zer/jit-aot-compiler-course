#ifndef NEW_STR_INSTR
#define NEW_STR_INSTR

#include "instr.h"
#include "instructions/type.h"

#include <string>

namespace ir::instr {
class NewStringInstr : public Instr {
public:
    NewStringInstr(std::string value)
        : Instr(InstrOpcode::NEW_STR, TypeId::REF), value_(value) {}

    std::string GetValue() const {
        return value_;
    }

    void Dump(IrDumper &dumper, bool dumpLiveness = false) override;

private:
    std::string value_;
};

}

#endif