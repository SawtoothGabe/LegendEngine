#pragma once

#include <string>
#include <vector>

#include <slang-com-ptr.h>
#include <slang.h>

struct Options
{
    enum class CompileFlagBits
    {
        DXIL  = 1 << 0,
        GLSL  = 1 << 1,
        SPIRV = 1 << 2,
        WGSL  = 1 << 3,
    };

    using CompileFlags = uint8_t;

    Options(int argc, char* argv[]);
    void PopulateSessionInfo(
        const Slang::ComPtr<slang::IGlobalSession>& globalSession,
        std::vector<slang::TargetDesc>& targets,
        std::vector<slang::CompilerOptionEntry>& compilerOptions) const;

    CompileFlags flags = 0;
    std::vector<const char*> includeDirs;
    std::vector<std::string_view> inputs;
    std::string_view output = "shaders.c";
private:
    enum class LastArg
    {
        OTHER,
        INCLUDE,
        OUTPUT
    };

    bool ParseFlags(LastArg& last, std::string_view arg);
    bool ParseInclude(LastArg& last, std::string_view arg);
    bool ParseOutput(LastArg& last, std::string_view arg);
};

uint8_t operator|=(uint8_t& lhs, const Options::CompileFlagBits& rhs);
uint8_t operator&(uint8_t lhs, const Options::CompileFlagBits& rhs);