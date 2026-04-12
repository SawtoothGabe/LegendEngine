#include "Options.hpp"
#include <print>

Options::Options(const int argc, char* argv[])
{
    auto last = LastArg::OTHER;
    for (int i = 1; i < argc; i++)
    {
        const std::string_view arg = argv[i];

        if (last != LastArg::OTHER)
        {
            switch (last)
            {
                case LastArg::OTHER: break;
                case LastArg::INCLUDE: includeDirs.push_back(arg.data()); break;
                case LastArg::OUTPUT: output = arg; break;
            }

            last = LastArg::OTHER;
            continue;
        }

        if (ParseFlags(last, arg))
            continue;

        if (ParseInclude(last, arg))
            continue;

        if (ParseOutput(last, arg))
            continue;

        inputs.emplace_back(arg);
    }
}

void Options::PopulateSessionInfo(
    const Slang::ComPtr<slang::IGlobalSession>& globalSession,
    std::vector<slang::TargetDesc>& targets,
    std::vector<slang::CompilerOptionEntry>& compilerOptions) const
{
    if (flags & CompileFlagBits::DXIL)
    {
        targets.push_back({
            .format = SLANG_DXIL,
            .profile = globalSession->findProfile("sm_6_0")
        });
    }

    if (flags & CompileFlagBits::SPIRV)
    {
        targets.push_back({
            .format = SLANG_SPIRV,
            .profile = globalSession->findProfile("spirv_1_3")
        });

        compilerOptions.push_back({
            slang::CompilerOptionName::EmitSpirvDirectly,
            {slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr}
        });
    }

    if (flags & CompileFlagBits::GLSL)
    {
        targets.push_back({
            .format = SLANG_GLSL,
            .profile = globalSession->findProfile("glsl_400")
        });
    }

    if (flags & CompileFlagBits::WGSL)
    {
        targets.push_back({
            .format = SLANG_WGSL,
            .profile = globalSession->findProfile("wgsl_400")
        });
    }
}

bool Options::ParseFlags(LastArg& last, const std::string_view arg)
{
    last = LastArg::OTHER;

    if (arg == "--dxil")
    {
        flags |= CompileFlagBits::DXIL;
        return true;
    }

    if (arg == "--spirv")
    {
        flags |= CompileFlagBits::SPIRV;
        return true;
    }

    if (arg == "--glsl")
    {
        flags |= CompileFlagBits::GLSL;
        return true;
    }

    if (arg == "--wgsl")
    {
        flags |= CompileFlagBits::WGSL;
        return true;
    }

    return false;
}

bool Options::ParseInclude(LastArg& last, const std::string_view arg)
{
    if (!arg.starts_with("-I"))
        return false;

    if (arg.length() == 2)
    {
        last = LastArg::INCLUDE;
        return true;
    }

    includeDirs.push_back(arg.substr(2, arg.length() - 1).data());
    return true;
}

bool Options::ParseOutput(LastArg& last, const std::string_view arg)
{
    if (!arg.starts_with("-o"))
        return false;

    if (arg.length() == 2)
    {
        last = LastArg::OUTPUT;
        return true;
    }

    output = arg.substr(2, arg.length() - 1);
    return true;
}

uint8_t operator|=(uint8_t& lhs, const Options::CompileFlagBits& rhs)
{
    return lhs = lhs | static_cast<uint8_t>(rhs);
}

uint8_t operator&(const uint8_t lhs, const Options::CompileFlagBits& rhs)
{
    return lhs & static_cast<uint8_t>(rhs);
}
