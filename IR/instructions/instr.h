#ifndef INSTR_H
#define INSTR_H

#include "instrMapping.h"
#include "irDumper.h"
#include "type.h"
#include <cstddef>
#include <set>
#include <vector>

namespace ir {
class BasicBlock;
} // namespace ir

namespace ir::instr {
using InstrId = std::size_t;

#define DECLARE_INSTR_CLASSES(instrType, className) class className;
INSTR_MAPPING(DECLARE_INSTR_CLASSES)
#undef DECLARE_INSTR_CLASSES

enum class InstrOpcode {
#define DECLARE_INSTR_OPCODES(instrType, className) instrType,
  INSTR_MAPPING(DECLARE_INSTR_OPCODES)
#undef DECLARE_NODE_TYPES
};

class Instr {
protected:
  explicit Instr(InstrOpcode op, TypeId type)
      : op_(op), prev_(nullptr), next_(nullptr), inputs_(), users_(),
        type_(type) {}
  explicit Instr(InstrOpcode op, TypeId type, std::vector<Instr *> &&inputs,
                 std::set<Instr *> &&users)
      : op_(op), prev_(nullptr), next_(nullptr), inputs_(inputs), users_(users),
        type_(type) {}

  explicit Instr(InstrOpcode op, TypeId type,
                 const std::vector<Instr *> &inputs,
                 const std::set<Instr *> &users)
      : op_(op), prev_(nullptr), next_(nullptr), inputs_(inputs), users_(users),
        type_(type) {}

public:
  void SetInstrId(InstrId id) { id_ = id; }
  InstrId GetInstrId() const { return id_; }

  void SetPrevInstr(Instr *instr) { prev_ = instr; }
  void SetNextInstr(Instr *instr) { next_ = instr; }

  Instr *GetPrevInstr() const { return prev_; }
  Instr *GetNextInstr() const { return next_; }

  void SetInputs(const std::vector<Instr *> &&inputs) {
    inputs_ = inputs;
    for (auto input : inputs_) {
      if (input != nullptr) {
        input->AddUser(this);
      }
    }
  }

  const std::vector<Instr *> &GetInputs() const { return inputs_; }
  const std::set<Instr *> &GetUsers() const { return users_; }

  void AddInput(Instr *input) {
    inputs_.push_back(input);
    if (input != nullptr) {
      input->AddUser(this);
    }
  }

  void SetInput(Instr *input, size_t index) {
    inputs_.insert(inputs_.begin() + index, input);
    if (input != nullptr) {
      input->AddUser(this);
    }
  }

  bool RemoveInput(Instr *input) {
    if (input == nullptr) {
      return false;
    }
    bool found = false;
    for (auto it = inputs_.begin(); it != inputs_.end(); ++it) {
      if (*it == input) {
        *it = nullptr;
        if (!found) {
          (*it)->RemoveUser(this);
        }
        found = true;
      }
    }
    return found;
  }

  bool RemoveInput(size_t index) {
    if (index >= inputs_.size()) {
      return false;
    }
    auto input = inputs_[index];
    inputs_[index] = nullptr;
    if (input != nullptr) {
      input->RemoveUser(this);
      return true;
    }
    return false;
  }

  bool ReplaceInput(Instr *input, Instr *newInput) {
    bool found = false;
    for (auto it = inputs_.begin(); it != inputs_.end(); ++it) {
      if (*it == input) {
        *it = newInput;
        if (newInput != nullptr) {
          newInput->AddUser(this);
        }
        found = true;
      }
    }
    if (found && input != nullptr) {
      input->RemoveUser(this);
    }
    return found;
  }

  bool ReplaceInput(Instr *newInput, size_t index) {
    if (index < inputs_.size()) {
      inputs_[index] = newInput;
      if (newInput != nullptr) {
        newInput->AddUser(this);
      }
      return true;
    }
    return false;
  }

  void RemoveUser(Instr *user) { users_.erase(user); }
  void AddUser(Instr *user) { users_.insert(user); }

  InstrOpcode GetOpcode() const { return op_; }

  virtual bool IsControllFlow() { return false; }

  virtual void Dump(IrDumper &dumper) {
    dumper.Add(id_);
    dumper.Add(".");
    dumper.Add(TypeIdToString(type_).data());
  }

#define DECLARE_IS_CHECKS(instrType, className)                                \
  bool Is##className() const { return op_ == InstrOpcode::instrType; }
  INSTR_MAPPING(DECLARE_IS_CHECKS)
#undef DECLARE_IS_CHECKS

#define DECLARE_AS_CHECKS(instrType, className)                                \
  className *As##className() { return reinterpret_cast<className *>(this); }   \
  const className *As##className() const {                                     \
    return reinterpret_cast<const className *>(this);                          \
  }
  INSTR_MAPPING(DECLARE_AS_CHECKS)
#undef DECLARE_AS_CHECKS

  TypeId GetType() const { return type_; }

protected:
  inline void DumpInput(IrDumper &dumper, Instr *input) {
    if (input == nullptr) {
      return;
    }
    dumper.Add("v");
    dumper.Add(input->GetInstrId());
  }

  inline void DumpInputs(IrDumper &dumper) {
    for (auto input : inputs_) {
      DumpInput(dumper, input);
      dumper.Add(" ");
    }
  }

  InstrId id_;
  Instr *prev_{nullptr};
  Instr *next_{nullptr};
  InstrOpcode op_;
  std::vector<Instr *> inputs_;
  std::set<Instr *> users_;
  TypeId type_;
};

} // namespace ir::instr

#endif