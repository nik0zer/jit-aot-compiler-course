#ifndef GRAPH_H
#define GRAPH_H

#include "basicBlock.h"
#include <cstddef>
#include <string>

namespace ir {

using MethodId = std::size_t;

class MethodGraph {
public:
  explicit MethodGraph(const std::string &&name, MethodId id)
      : name_(name), id_(id) {}

  void AddBlock(BasicBlock *block) { blocks_.push_back(block); }
  bool RemoveBlock(BasicBlock *block) {
    for (auto it = blocks_.begin(); it != blocks_.end(); ++it) {
      if (*it == block) {
        blocks_.erase(it);
        return true;
      }
    }
    return false;
  }

  template <typename... Args> BasicBlock *AllocateBlock(Args &&...args) {
    auto block = new BasicBlock(std::forward<Args>(args)...);
    blocks_.push_back(block);
    return block;
  }

  const std::vector<BasicBlock *> &GetBlocks() const { return blocks_; }
  void SetBlocks(const std::vector<BasicBlock *> &&blocks) { blocks_ = blocks; }

  const std::string &GetName() const { return name_; }

private:
  std::vector<BasicBlock *> blocks_;
  std::string name_;
  MethodId id_;
};

} // namespace ir

#endif