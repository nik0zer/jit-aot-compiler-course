#ifndef SOURCE_IR_BUILDER_H
#define SOURCE_IR_BUILDER_H

#include "irBuilder.h"
#include <algorithm>
#include <filesystem>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace IrBuilder {

class DiagnosticsEngine {
public:
    enum class DiagnosticType {
        Error,
        Warning,
        Info
    };

    struct Diagnostic {
    public:
        DiagnosticType type;
        std::string message;
        size_t line;
        std::filesystem::path path;
    };


    struct Position {
        std::filesystem::path &file;
        size_t line;
    };

    DiagnosticsEngine() {}

    void ThrowError(std::string message, size_t line, std::filesystem::path &path) {
        ThrowDiagnostic(Diagnostic {DiagnosticType::Error, message, line, path});
    }
    void ThrowError(std::string message, Position &position) {
        ThrowDiagnostic(Diagnostic {DiagnosticType::Error, message, position.line, position.file});
    }
    void ThrowDiagnostic(Diagnostic diagnostic) {
        diagnostics_.push_back(diagnostic);
    }

    bool HasErrors()
    {
        return std::find_if(diagnostics_.begin(), diagnostics_.end(),
        [](Diagnostic diagnostic) { return diagnostic.type == DiagnosticType::Error; }) != diagnostics_.end();
    }

    std::string ToString() {
        std::stringstream result;
        for (auto diagnostic : diagnostics_) {
            result << diagnostic.path.string() + ":" + std::to_string(diagnostic.line) + ": " + diagnostic.message + "\n";
        }
        return result.str();
    }

    void Flush(std::ostream &os)
    {
        os << ToString();
        diagnostics_.clear();
    }

    void Flush() { Flush(std::cout); }

    void Clear() { diagnostics_.clear(); }

private:
    std::vector<Diagnostic> diagnostics_;
};

class SourceIrBuilder : public IrBuilder {
public:
    SourceIrBuilder(std::string_view source) : sourcePath_(source) {}
    void SetSource(std::string_view source) { sourcePath_ = source; }

    ir::MethodGraph *Build(std::ostream &diagnosticOutput) override;

private:
    std::filesystem::path sourcePath_;
};

}

#endif