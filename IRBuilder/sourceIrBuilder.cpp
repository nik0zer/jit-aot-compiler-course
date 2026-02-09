#include "sourceIrBuilder.h"
#include "basicBlock.h"
#include "graph.h"
#include "instructions/instr.h"
#include "instructions/binaryOperationInstr.h"
#include "instructions/callInstr.h"
#include "instructions/castInstr.h"
#include "instructions/constantInstr.h"
#include "instructions/ifInstr.h"
#include "instructions/paramInstr.h"
#include "instructions/phiInstr.h"
#include "instructions/returnInstr.h"
#include <climits>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <regex>
#include <vector>

namespace IrBuilder {

std::optional<std::ifstream> ReadSource(const std::filesystem::path& source) {
    if (std::filesystem::is_regular_file(source)) {
        std::ifstream file(source, std::ios::in);
        if(file.is_open()) {
            return file;
        }
    }
    return std::nullopt;
}

struct Position {
    std::filesystem::path &file;
    size_t line;
};

template <typename T>
struct UnderProcessedConnection {
    T* objectWithUnresolvedConnection;
    size_t numOfInput;
    size_t inputObjectId;
    Position position;
};

using UnderProcessedPredessor = std::vector<UnderProcessedConnection<ir::BasicBlock>>;
using UnderProcessedSuccesor = std::vector<UnderProcessedConnection<ir::BasicBlock>>;
using UnderProcessedInput = std::vector<UnderProcessedConnection<ir::instr::Instr>>;

struct UnderProcessedConnections {
    UnderProcessedPredessor unresolvedPreds;
    UnderProcessedSuccesor unresolvedSuccs;
    UnderProcessedInput unresolvedInputs;
};



ir::BasicBlock *CheckIfBasicBlock(std::string_view line, ir::MethodGraph &graph, std::unordered_map<size_t, ir::BasicBlock *> &blocksMap,
                                  UnderProcessedConnections &underProcessedConnections, size_t lineNum,
                                  std::filesystem::path &file, DiagnosticsEngine &diagnosticEngine)
{
    std::string str(line);
    str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
    str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);

    std::regex bbRegex(R"((?:((?:bb \d+)(?:, ?(?:bb \d+))*) -> )?(bb \d+)(?: -> ((?:bb \d+)(?:, ?(?:bb \d+))*))?)");
    std::smatch match;

    if (std::regex_match(str, match, bbRegex)) {
        size_t currentBBId = std::stoul(std::string(match[2]).substr(3));

        if (blocksMap.count(currentBBId)) {
            diagnosticEngine.ThrowError("Basic block with id " + std::to_string(currentBBId) + " already exists", lineNum, file);
            return nullptr;
        }

        auto* newBB = graph.AllocateBlock();
        newBB->SetId(currentBBId);

        if (match[1].matched) {
            std::string predsStr = match[1];
            std::regex predRegex(R"(bb (\d+))");
            std::sregex_iterator it(predsStr.begin(), predsStr.end(), predRegex);
            std::sregex_iterator end;

            std::vector<ir::BasicBlock*> preds;
            for (; it != end; ++it) {
                size_t predId = std::stoul((*it)[1]);
                preds.push_back(nullptr);
                auto predIt = blocksMap.find(predId);
                if (predIt != blocksMap.end()) {
                    preds.back() = predIt->second;
                } else {
                    underProcessedConnections.unresolvedPreds.push_back({newBB, preds.size() - 1, predId, {file, lineNum}});
                }
            }
            newBB->SetPreds(std::move(preds));
        }

        if (match[3].matched) {
            std::string succsStr = match[3];
            std::regex succRegex(R"(bb (\d+))");
            std::sregex_iterator it(succsStr.begin(), succsStr.end(), succRegex);
            std::sregex_iterator end;

            std::array<ir::BasicBlock*, ir::MAX_NUM_OF_SUCCESSORS> succs;
            for (int i = 0; i < ir::MAX_NUM_OF_SUCCESSORS; i++) {
                succs[i] = nullptr;
            }
            size_t i = 0;
            for (; it != end; ++it) {
                size_t succId = std::stoul((*it)[1]);
                succs[i] = nullptr;
                auto succIt = blocksMap.find(succId);
                if (succIt != blocksMap.end()) {
                    succs[i] = succIt->second;
                } else {
                    underProcessedConnections.unresolvedSuccs.push_back({newBB, i, succId, {file, lineNum}});
                }
                i++;
            }
            newBB->SetSuccs(std::move(succs));
        }

        blocksMap[currentBBId] = newBB;
        return newBB;
    }

    return nullptr;
}

namespace {
ir::instr::TypeId StringToTypeId(const std::string& type) {
    if (type == "u8") return ir::instr::TypeId::U8;
    if (type == "u16") return ir::instr::TypeId::U16;
    if (type == "u32") return ir::instr::TypeId::U32;
    if (type == "u64") return ir::instr::TypeId::U64;
    if (type == "i8") return ir::instr::TypeId::I8;
    if (type == "i16") return ir::instr::TypeId::I16;
    if (type == "i32") return ir::instr::TypeId::I32;
    if (type == "i64") return ir::instr::TypeId::I64;
    if (type == "f32") return ir::instr::TypeId::F32;
    if (type == "f64") return ir::instr::TypeId::F64;
    if (type == "void") return ir::instr::TypeId::VOID;
    return ir::instr::TypeId::NONE;
}

ir::instr::BinaryOperationType StringToBinaryOpType(const std::string& op) {
    if (op == "add") return ir::instr::BinaryOperationType::ADD;
    if (op == "sub") return ir::instr::BinaryOperationType::SUB;
    if (op == "mul") return ir::instr::BinaryOperationType::MUL;
    if (op == "div") return ir::instr::BinaryOperationType::DIV;
    if (op == "and") return ir::instr::BinaryOperationType::AND;
    if (op == "or") return ir::instr::BinaryOperationType::OR;
    return ir::instr::BinaryOperationType::ADD; // Should not happen
}

ir::instr::IfType StringToIfType(const std::string& type) {
    if (type == "eq") return ir::instr::IfType::EQ;
    if (type == "ne") return ir::instr::IfType::NE;
    if (type == "lt") return ir::instr::IfType::LT;
    if (type == "le") return ir::instr::IfType::LE;
    if (type == "gt") return ir::instr::IfType::GT;
    if (type == "ge") return ir::instr::IfType::GE;
    return ir::instr::IfType::EQ; // Should not happen
}
}

ir::instr::Instr *CheckIfInstruction(std::string_view line, std::unordered_map<size_t, ir::instr::Instr *> &instrMap,
                                       UnderProcessedConnections &underProcessedConnections, size_t lineNum,
                                       std::filesystem::path &file, DiagnosticsEngine &diagnosticEngine)
{
    std::string str(line);
    // trim
    str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
    str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);

    auto addInstr = [&](ir::instr::Instr* instr, size_t id) {
        if (instrMap.count(id)) {
            diagnosticEngine.ThrowError("Instruction with id " + std::to_string(id) + " already exists", lineNum, file);
            return (ir::instr::Instr*)nullptr;
        }
        instr->SetInstrId(id);
        instrMap[id] = instr;
        return instr;
    };

    std::smatch match;

    // Param
    std::regex paramRegex(R"((\d+)\.(u\d+|void)\s+param (\d+))");
    if (std::regex_match(str, match, paramRegex)) {
        size_t instrId = std::stoul(match[1]);
        ir::instr::TypeId type = StringToTypeId(match[2]);
        size_t paramNum = std::stoul(match[3]);
        auto newInstr = new ir::instr::ParamInstr(type, paramNum);
        return addInstr(newInstr, instrId);
    }

    // Constant
    std::regex constRegex(R"((\d+)\.(u\d+|void)\s+const (\d+))");
    if (std::regex_match(str, match, constRegex)) {
        size_t instrId = std::stoul(match[1]);
        ir::instr::TypeId type = StringToTypeId(match[2]);
        uint64_t val = std::stoull(match[3]);
        auto newInstr = new ir::instr::ConstantInstr(type, val);
        return addInstr(newInstr, instrId);
    }

    // Cast
    std::regex castRegex(R"((\d+)\.(u\d+|void)\s+cast v(\d+))");
    if (std::regex_match(str, match, castRegex)) {
        size_t instrId = std::stoul(match[1]);
        ir::instr::TypeId type = StringToTypeId(match[2]);
        size_t inputId = std::stoul(match[3]);
        ir::instr::Instr* input = nullptr;
        auto inputIt = instrMap.find(inputId);
        if (inputIt != instrMap.end()) {
            input = inputIt->second;
        }

        auto newInstr = new ir::instr::CastInstr(type, input);
        if (input == nullptr) {
            underProcessedConnections.unresolvedInputs.push_back({newInstr, 0, inputId, {file, lineNum}});
        }
        return addInstr(newInstr, instrId);
    }

    // Binary Operation
    std::regex binOpRegex(R"((\d+)\.(u\d+|void)\s+(add|sub|mul|div|and|or) v(\d+) v(\d+))");
    if (std::regex_match(str, match, binOpRegex)) {
        size_t instrId = std::stoul(match[1]);
        ir::instr::TypeId type = StringToTypeId(match[2]);
        ir::instr::BinaryOperationType op = StringToBinaryOpType(match[3]);
        size_t input1Id = std::stoul(match[4]);
        size_t input2Id = std::stoul(match[5]);

        ir::instr::Instr* input1 = nullptr;
        auto input1It = instrMap.find(input1Id);
        if (input1It != instrMap.end()) {
            input1 = input1It->second;
        }

        ir::instr::Instr* input2 = nullptr;
        auto input2It = instrMap.find(input2Id);
        if (input2It != instrMap.end()) {
            input2 = input2It->second;
        }

        auto newInstr = new ir::instr::BinaryOperationInstr(type, op, input1, input2);

        if (input1 == nullptr) {
            underProcessedConnections.unresolvedInputs.push_back({newInstr, 0, input1Id, {file, lineNum}});
        }
        if (input2 == nullptr) {
            underProcessedConnections.unresolvedInputs.push_back({newInstr, 1, input2Id, {file, lineNum}});
        }

        return addInstr(newInstr, instrId);
    }

    // Return
    std::regex retRegex(R"((\d+)\.(u\d+|void)\s+return(?: v(\d+))?)");
    if (std::regex_match(str, match, retRegex)) {
        size_t instrId = std::stoul(match[1]);
        ir::instr::TypeId type = StringToTypeId(match[2]);
        ir::instr::Instr* input = nullptr;
        if (match[3].matched) {
            size_t inputId = std::stoul(match[3]);
            auto inputIt = instrMap.find(inputId);
            if (inputIt != instrMap.end()) {
                input = inputIt->second;
            }
        }
        auto newInstr = new ir::instr::ReturnInstr(type, input);
        if (input == nullptr && match[3].matched) {
            underProcessedConnections.unresolvedInputs.push_back({newInstr, 0, std::stoul(match[3]), {file, lineNum}});
        }
        return addInstr(newInstr, instrId);
    }

    // Call
    std::regex callRegex(R"((\d+)\.(u\d+|void)\s+call v(\d+)((?: v\d+)*))");
    if (std::regex_match(str, match, callRegex)) {
        size_t instrId = std::stoul(match[1]);
        ir::instr::TypeId type = StringToTypeId(match[2]);
        size_t methodId = std::stoul(match[3]);

        std::vector<ir::instr::Instr*> args;
        std::string argsStr = match[4];
        std::regex argRegex(R"(v(\d+))");
        std::sregex_iterator it(argsStr.begin(), argsStr.end(), argRegex);
        std::sregex_iterator end;
        size_t inputIdx = 0;
        for (; it != end; ++it, ++inputIdx) {
            size_t argId = std::stoul((*it)[1]);
            auto argIt = instrMap.find(argId);
            if (argIt != instrMap.end()) {
                args.push_back(argIt->second);
            } else {
                args.push_back(nullptr);
                underProcessedConnections.unresolvedInputs.push_back({nullptr, inputIdx, argId, {file, lineNum}});
            }
        }

        auto newInstr = new ir::instr::CallInstr(type, methodId, std::move(args));

        // a bit of a hack to resolve inputs later
        for (auto& unresolved : underProcessedConnections.unresolvedInputs) {
            if (unresolved.objectWithUnresolvedConnection == nullptr) {
                unresolved.objectWithUnresolvedConnection = newInstr;
            }
        }

        return addInstr(newInstr, instrId);
    }

    // Phi
    std::regex phiRegex(R"((\d+)\.(u\d+|void)\s+phi((?: v\d+)*))");
    if (std::regex_match(str, match, phiRegex)) {
        size_t instrId = std::stoul(match[1]);
        ir::instr::TypeId type = StringToTypeId(match[2]);
        std::vector<ir::instr::Instr*> inputs;
        std::string argsStr = match[3];
        std::regex argRegex(R"(v(\d+))");
        std::sregex_iterator it(argsStr.begin(), argsStr.end(), argRegex);
        std::sregex_iterator end;
        size_t inputIdx = 0;
        for (; it != end; ++it, ++inputIdx) {
            size_t argId = std::stoul((*it)[1]);
            auto argIt = instrMap.find(argId);
            if (argIt != instrMap.end()) {
                inputs.push_back(argIt->second);
            } else {
                inputs.push_back(nullptr);
                underProcessedConnections.unresolvedInputs.push_back({nullptr, inputIdx, argId, {file, lineNum}});
            }
        }

        auto newInstr = new ir::instr::PhiInstr(type, std::move(inputs));
        for (auto& unresolved : underProcessedConnections.unresolvedInputs) {
            if (unresolved.objectWithUnresolvedConnection == nullptr) {
                unresolved.objectWithUnresolvedConnection = newInstr;
            }
        }
        return addInstr(newInstr, instrId);
    }

    // If (no result)
    std::regex ifRegex(R"(if\.(gt|lt|ge|le|eq|ne) v(\d+) v(\d+))");
    if (std::regex_match(str, match, ifRegex)) {
        ir::instr::IfType type = StringToIfType(match[1]);
        size_t input1Id = std::stoul(match[2]);
        size_t input2Id = std::stoul(match[3]);

        ir::instr::Instr* input1 = nullptr;
        auto input1It = instrMap.find(input1Id);
        if (input1It != instrMap.end()) {
            input1 = input1It->second;
        }

        ir::instr::Instr* input2 = nullptr;
        auto input2It = instrMap.find(input2Id);
        if (input2It != instrMap.end()) {
            input2 = input2It->second;
        }

        auto newInstr = new ir::instr::IfInstr(input1, input2, type);

        if (input1 == nullptr) {
            underProcessedConnections.unresolvedInputs.push_back({newInstr, 0, input1Id, {file, lineNum}});
        }

        if (input2 == nullptr) {
            underProcessedConnections.unresolvedInputs.push_back({newInstr, 1, input2Id, {file, lineNum}});
        }

        return newInstr;
    }


    return nullptr;
}

ir::MethodGraph *SourceIrBuilder::Build(std::ostream &diagnosticOutput) {
    auto absolutePath = std::filesystem::absolute(sourcePath_);
    auto source = ReadSource(absolutePath);
    auto graph = new ir::MethodGraph(sourcePath_.filename().string(), 0);
    auto diagnosticEngine = DiagnosticsEngine();

    std::unordered_map<size_t, ir::instr::Instr *> instrMap {};
    std::unordered_map<size_t, ir::BasicBlock *> blocksMap {};

    UnderProcessedConnections underProcessedConnections {};

    if (!source) {
        diagnosticEngine.ThrowError("File not found", 0, sourcePath_);
        diagnosticEngine.Flush(diagnosticOutput);
        delete graph;
        return nullptr;
    }

    std::string line;
    ir::BasicBlock *currentBlock = nullptr;
    bool firstNonPhiProcessed = false;
    size_t currentLine = 0;
    while (std::getline(*source, line)) {
        currentLine++;
        if (line.empty()) {
            continue;
        }
        if (auto block = CheckIfBasicBlock(line, *graph, blocksMap, underProcessedConnections, currentLine, sourcePath_, diagnosticEngine); block != nullptr) {
            currentBlock = block;
            continue;
        }
        if (auto instr = CheckIfInstruction(line, instrMap, underProcessedConnections, currentLine, sourcePath_, diagnosticEngine); instr != nullptr)
        {
            if (currentBlock == nullptr) {
                diagnosticEngine.ThrowError("Instruction outside of a basic block", currentLine, sourcePath_);
                continue;
            }
            if (firstNonPhiProcessed && instr->GetOpcode() == ir::instr::InstrOpcode::PHI) {
                diagnosticEngine.ThrowError("Phi instructions must go first", currentLine, sourcePath_);
            }
            if(!firstNonPhiProcessed && instr->GetOpcode() != ir::instr::InstrOpcode::PHI) {
                firstNonPhiProcessed = true;
            }
            currentBlock->AppendInstr(instr);
            continue;
        }
        diagnosticEngine.ThrowError("Syntax error", currentLine, sourcePath_);
    }

    // Fill UnderProcessedConnections
    auto checkIfUnresolved = [&](auto &it, auto &map, auto &connection) {
        if (it == map.end()) {
            diagnosticEngine.ThrowError("Unresolved object", connection.position.line, connection.position.file);
            return false;
        }
        return true;
    };
    for (auto &predConnection : underProcessedConnections.unresolvedPreds) {
        auto bbIt = blocksMap.find(predConnection.inputObjectId);
        if (!checkIfUnresolved(bbIt, blocksMap, predConnection)) {
            continue;
        }
        predConnection.objectWithUnresolvedConnection->SetPred(bbIt->second, predConnection.numOfInput);
    }
    for (auto &succConnection : underProcessedConnections.unresolvedSuccs) {
        auto bbIt = blocksMap.find(succConnection.inputObjectId);
        if (!checkIfUnresolved(bbIt, blocksMap, succConnection)) {
            continue;
        }
        succConnection.objectWithUnresolvedConnection->SetSucc(bbIt->second, succConnection.numOfInput);
    }
    for (auto &input : underProcessedConnections.unresolvedInputs) {
        auto instrIt = instrMap.find(input.inputObjectId);
        if (!checkIfUnresolved(instrIt, instrMap, input)) {
            continue;
        }
        input.objectWithUnresolvedConnection->SetInput(instrIt->second, input.inputObjectId);
    }


    source->close();
    auto hasErrors = diagnosticEngine.HasErrors();
    diagnosticEngine.Flush(diagnosticOutput);
    if(hasErrors) {
        delete graph;
        return nullptr;
    }
    return graph;
}

}