#include <filesystem>
#include <fstream>
#include <future>
#include <ModuleRegistry.hpp>
#include <OutputGenerator.hpp>
#include <print>

#include "Options.hpp"
#include "Program.hpp"

#include <simdjson.h>

using namespace simdjson;

struct JsonFile
{
    std::string path;
    std::future<std::string> content;
};

bool OutputDirectoryExists(const Options& options)
{
    const std::filesystem::path path = options.output;
    if (path.parent_path().empty())
        return true;

    return std::filesystem::exists(path.parent_path());
}

void SortFiles(const Options& options, std::vector<JsonFile>& jsonFiles,
    std::vector<std::string>& slangFiles)
{
    jsonFiles.reserve(options.inputs.size());
    for (std::string_view inputPath : options.inputs)
    {
        if (!inputPath.ends_with(".json"))
        {
            if (!inputPath.ends_with(".slang"))
            {
                std::println(stderr, "error: the file \"{}\" is not a valid slang shader or JSON config",
                    inputPath);
                throw std::runtime_error("error: the file is not a valid slang shader or JSON config");
            }

            slangFiles.emplace_back(inputPath);
            continue;
        }

        if (!std::filesystem::exists(inputPath))
        {
            std::println(stderr, "error: The file \"{}\" does not exist", inputPath);
            throw std::runtime_error("error: The file does not exist");
        }

        jsonFiles.emplace_back(std::string(inputPath),
            std::async(std::launch::async, [inputPath]
        {
            const auto size = std::filesystem::file_size(inputPath);
            std::string data(size, '\0');

            std::ifstream inputFile(inputPath.data(), std::ios::binary);
            inputFile.read(data.data(), static_cast<std::streamsize>(size));

            return data;
        }));
    }
}

std::vector<Program> CompilePrograms(ModuleRegistry& registry, std::vector<JsonFile>& jsonFiles,
    const std::vector<std::string>& slangFiles)
{
    ondemand::parser p;
    std::vector<Program> programs;
    programs.reserve(jsonFiles.size() + slangFiles.size());

    for (const std::string_view file : slangFiles)
        programs.emplace_back(Program::FromSlang(registry, file));

    for (auto& [path, content] : jsonFiles)
        programs.emplace_back(Program::FromJson(registry, path, content.get(), p));

    return programs;
}

Slang::ComPtr<slang::ISession> CreateSession(const Options& options, const Slang::ComPtr<slang::IGlobalSession>& globalSession)
{
    std::vector<slang::TargetDesc> targets;
    std::vector<slang::CompilerOptionEntry> compilerOptions;
    options.PopulateSessionInfo(globalSession, targets, compilerOptions);

    slang::SessionDesc sessionDesc;
    sessionDesc.searchPathCount = static_cast<SlangInt>(options.includeDirs.size());
    sessionDesc.searchPaths = options.includeDirs.data();
    sessionDesc.targetCount = static_cast<SlangInt>(targets.size());
    sessionDesc.targets = targets.data();
    sessionDesc.compilerOptionEntryCount = static_cast<SlangInt>(compilerOptions.size());
    sessionDesc.compilerOptionEntries = compilerOptions.data();

    Slang::ComPtr<slang::ISession> session;
    globalSession->createSession(sessionDesc, session.writeRef());

    return session;
}

int main(const int argc, char* argv[])
{
    const Options options(argc, argv);

    if (!OutputDirectoryExists(options))
    {
        std::println(stderr, "error: the parent directory of the output file does not exist");
        return -1;
    }

    if (!options.flags)
    {
        std::println(stderr, "error: no compilation targets specified (--dxil --spirv --glsl --wgsl)");
        return -1;
    }

    if (options.inputs.empty())
    {
        std::println(stderr, "error: no input files specified");
        return -1;
    }

    try
    {
        Slang::ComPtr<slang::IGlobalSession> globalSession;
        slang::createGlobalSession(globalSession.writeRef());

        const Slang::ComPtr<slang::ISession> session = CreateSession(options, globalSession);

        std::vector<std::string> slangFiles;
        std::vector<JsonFile> jsonFiles;
        SortFiles(options, jsonFiles, slangFiles);

        ModuleRegistry registry(session);
        std::vector<Program> programs = CompilePrograms(registry, jsonFiles, slangFiles);

        std::string cCode = OutputGenerator::LinkProgramsAndMakeOutput(options, session, programs);
        std::ofstream output(options.output.data());
        output << cCode;
    }
    catch (const std::exception& e)
    {
        std::println(stderr, "error: {}", e.what());
    }

    return 0;
}
