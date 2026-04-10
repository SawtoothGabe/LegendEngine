#include "Options.hpp"

#include <print>

Options::Options(const int argc, char* argv[])
{
    auto last = LastArg::OTHER;
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (last != LastArg::OTHER)
        {
            switch (last)
            {
                case LastArg::OTHER: break;
                case LastArg::INCLUDE: includeDirs.push_back(arg); break;
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

        inputs.push_back(arg);
    }
}

void Options::Print()
{
    std::println("Options:");

    std::println("\tflags =");
    if (flags & CompileFlagBits::DXIL)
        std::println("\t\tCompileFlagBits::DXIL");
    if (flags & CompileFlagBits::SPIRV)
        std::println("\t\tCompileFlagBits::SPIRV");
    if (flags & CompileFlagBits::GLSL)
        std::println("\t\tCompileFlagBits::GLSL");
    if (flags & CompileFlagBits::WGSL)
        std::println("\t\tCompileFlagBits::WGSL");

    std::println("\tincludeDirs =");
    for (const std::string& string : includeDirs)
        std::println("\t\t\"{}\"", string);

    std::println("\toutput = \"{}\"", output);

    std::println("\tinputs =");
    for (const std::string& string : inputs)
        std::println("\t\t\"{}\"", string);
}

bool Options::ParseFlags(LastArg& last, const std::string& arg)
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

bool Options::ParseInclude(LastArg& last, const std::string& arg)
{
    if (!arg.starts_with("-I"))
        return false;

    if (arg.length() == 2)
    {
        last = LastArg::INCLUDE;
        return true;
    }

    const std::string path = arg.substr(2, arg.length() - 1);
    includeDirs.push_back(path);

    return true;
}

bool Options::ParseOutput(LastArg& last, const std::string& arg)
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
