#ifndef GRAPH_H
#define GRAPH_H

#include <cstddef>
#include <string>
#include "basicBlock.h"

namespace ir {

class MethodGraph
{
public:
    explicit MethodGraph(const std::string &&name) : name_(name) {}

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

    template<typename... Args>
    BasicBlock *AllocateBlock(Args &&...args) {
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
};

} // namespace ir

#endif