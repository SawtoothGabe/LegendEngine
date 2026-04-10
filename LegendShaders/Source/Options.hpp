#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct Options
{
    enum class CompileFlagBits
    {
        DXIL  = 1 << 0,
        SPIRV = 1 << 1,
        GLSL  = 1 << 2,
        WGSL  = 1 << 3,
    };

    using CompileFlags = uint8_t;

    Options(int argc, char* argv[]);
    void Print();

    CompileFlags flags = 0;
    std::vector<std::string> includeDirs;
    std::vector<std::string> inputs;
    std::string output = "shaders.c";
private:
    enum class LastArg
    {
        OTHER,
        INCLUDE,
        OUTPUT
    };

    bool ParseFlags(LastArg& last, const std::string& arg);
    bool ParseInclude(LastArg& last, const std::string& arg);
    bool ParseOutput(LastArg& last, const std::string& arg);
};

uint8_t operator|=(uint8_t& lhs, const Options::CompileFlagBits& rhs);
uint8_t operator&(uint8_t lhs, const Options::CompileFlagBits& rhs);