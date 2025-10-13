#ifndef INSTR_H
#define INSTR_H

#include "instrMapping.h"
#include "irDumper.h"
#include "type.h"
#include <cstddef>
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
                 std::vector<Instr *> &&users)
      : op_(op), prev_(nullptr), next_(nullptr), inputs_(std::move(inputs)),
        users_(std::move(users)), type_(type) {}

  explicit Instr(InstrOpcode op, TypeId type,
                 const std::vector<Instr *> &inputs,
                 const std::vector<Instr *> &users)
      : op_(op), prev_(nullptr), next_(nullptr), inputs_(inputs), users_(users),
        type_(type) {}

public:
  void SetInstrId(InstrId id) { id_ = id; }
  InstrId GetInstrId() const { return id_; }

  void SetPrevInstr(Instr *instr) { prev_ = instr; }
  void SetNextInstr(Instr *instr) { next_ = instr; }

  Instr *GetPrevInstr() const { return prev_; }
  Instr *GetNextInstr() const { return next_; }

  void SetInputs(const std::vector<Instr *> &&inputs) { inputs_ = inputs; }
  void SetUsers(const std::vector<Instr *> &&users) { users_ = users; }

  const std::vector<Instr *> &GetInputs() const { return inputs_; }
  const std::vector<Instr *> &GetUsers() const { return users_; }

  void AddInput(Instr *input) {
    inputs_.push_back(input);
    input->AddUser(this);
  }
  void AddUser(Instr *user) { users_.push_back(user); }

  void AddInput(Instr *input, size_t index) {
    inputs_.insert(inputs_.begin() + index, input);
    input->AddUser(this);
  }
  void AddUser(Instr *user, size_t index) {
    users_.insert(users_.begin() + index, user);
  }

  void RemoveInput(Instr *input) {
    for (auto it = inputs_.begin(); it != inputs_.end(); ++it) {
      if (*it == input) {
        inputs_.erase(it);
        if (*it != nullptr) {
          (*it)->RemoveUser(this);
        }
      }
    }
  }
  void RemoveUser(Instr *user) {
    for (auto it = users_.begin(); it != users_.end(); ++it) {
      if (*it == user) {
        users_.erase(it);
      }
    }
  }

  void RemoveInput(Instr *input, size_t index) {
    inputs_.erase(inputs_.begin() + index);
    if (input != nullptr) {
      input->RemoveUser(this);
    }
  }
  void RemoveUser(Instr *user, size_t index) {
    users_.erase(users_.begin() + index);
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
    return found;
  }
  bool ReplaceUser(Instr *user, Instr *newUser) {
    bool found = false;
    for (auto it = users_.begin(); it != users_.end(); ++it) {
      if (*it == user) {
        *it = newUser;
        found = true;
      }
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
  bool ReplaceUser(Instr *newUser, size_t index) {
    if (index < users_.size()) {
      users_[index] = newUser;
      return true;
    }
    return false;
  }

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
  std::vector<Instr *> users_;
  TypeId type_;
};

} // namespace ir::instr

#endif