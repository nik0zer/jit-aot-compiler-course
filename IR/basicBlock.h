#ifndef BASICBLOCK_H
#define BASICBLOCK_H

#include <vector>
#include "intsructions/instr.h"
namespace ir {

class BasicBlock {
public:
    explicit BasicBlock() {}

    template <typename InstrType, typename... Args>
    instr::Instr *AllocateInstr(Args &&...args) {
        auto instr = new InstrType(std::forward<Args>(args)...);
        if(!instrs_.empty()) {
            instr->SetPrevInstr(instrs_.back());
            instrs_.back()->SetNextInstr(instr);
        }
        instrs_.push_back(instr);
        return instr;
    }
    const std::vector<instr::Instr *> &GetInstrs() const { return instrs_; }

    ~BasicBlock() {
        for (auto instr : instrs_) {
            if(instr != nullptr) {
                delete instr;
            }
        }
    }

private:
    std::vector<instr::Instr *> instrs_;
};

} // namespace ir

#endif