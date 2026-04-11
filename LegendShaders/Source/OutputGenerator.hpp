#pragma once

#include <string>
#include <vector>

#include "Options.hpp"
#include "Program.hpp"

namespace OutputGenerator
{
    std::string LinkProgramsAndMakeOutput(const Options& options, slang::ISession* session,
        std::vector<Program>& programs);
}