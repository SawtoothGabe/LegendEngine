#include "OutputGenerator.hpp"

#include <SlangUtils.hpp>

std::string OutputGenerator::LinkProgramsAndMakeOutput(const Options& options, slang::ISession* session,
                                                       std::vector<Program>& programs)
{
    std::string output;
    for (Program& program : programs)
    {
        const Slang::ComPtr<slang::IComponentType> linkedProgram = program.Link(session);

        Slang::ComPtr<slang::IBlob> diagnostics;
        Slang::ComPtr<slang::IBlob> code;
        linkedProgram->getTargetCode(0, code.writeRef(), diagnostics.writeRef());
        SlangUtils::Diagnose(diagnostics);

        output += static_cast<const char*>(code->getBufferPointer());
    }

    return output;
}
