#include "Program.hpp"

#include <filesystem>
#include <fstream>
#include <future>
#include <ModuleRegistry.hpp>
#include <print>
#include <simdjson.h>
#include <slang-com-ptr.h>
#include <slang.h>
#include <SlangUtils.hpp>

using namespace simdjson;

Program Program::FromJson(ModuleRegistry& registry, const std::string_view path, const std::string_view json, ondemand::parser& parser)
{
    const std::filesystem::path jsonPath(path);
    Program program;

    const padded_string paddedJson = json;
    ondemand::document doc = parser.iterate(paddedJson);

    std::vector<std::filesystem::path> modulePaths;
    for (auto field : doc.get_object())
    {
        const std::string_view key = field.escaped_key();
        if (key == "features")
            for (auto el : field.value().get_array())
                program.AddFeature(el.get_string().value());
        else if (key == "modules")
            for (auto el : field.value().get_array())
                modulePaths.emplace_back(el.get_string().value());
        else
            std::println("{}: warning: unused field \"{}\"",
                std::filesystem::path(path).filename().string(), key);
    }

    for (const auto& module : modulePaths)
    {
        const std::filesystem::path relative = jsonPath.parent_path() / module;
        program.m_modules.emplace_back(registry.TryCreate(relative));
    }

    return program;
}

Program Program::FromSlang(ModuleRegistry& registry, const std::string_view path)
{
    Program program;
    program.m_modules.emplace_back(registry.TryCreate(path));

    return program;
}

Slang::ComPtr<slang::IComponentType> Program::Link(slang::ISession* session)
{
    std::vector<slang::IComponentType*> components;
    for (auto module : m_modules)
        components.emplace_back(module);

    std::vector<Slang::ComPtr<slang::IEntryPoint>> entrypoints = GetEntrypoints();
    for (const auto& entrypoint : entrypoints)
        components.emplace_back(entrypoint);

    Slang::ComPtr<slang::IBlob> diagnostics;
    Slang::ComPtr<slang::IComponentType> composedProgram;
    session->createCompositeComponentType(
        components.data(),
        components.size(),
        composedProgram.writeRef(),
        diagnostics.writeRef()
    );
    SlangUtils::Diagnose(diagnostics);

    Slang::ComPtr<slang::IComponentType> linkedProgram;
    composedProgram->link(
        linkedProgram.writeRef(),
        diagnostics.writeRef()
    );
    SlangUtils::Diagnose(diagnostics);

    return linkedProgram;
}

void Program::AddFeature(const std::string_view feature)
{
    if (feature == "textured")
    {
        m_features.emplace_back(Feature::TEXTURED);
        return;
    }
}

std::vector<Slang::ComPtr<slang::IEntryPoint>> Program::GetEntrypoints()
{
    std::vector<Slang::ComPtr<slang::IEntryPoint>> entrypoints;
    for (Slang::ComPtr<slang::IModule>& module : m_modules)
    {
        uint32_t entrypointCount = module->getDefinedEntryPointCount();
        for (uint32_t i = 0; i < entrypointCount; i++)
        {
            Slang::ComPtr<slang::IEntryPoint> pEntrypoint = nullptr;
            SlangResult result = module->getDefinedEntryPoint(i, pEntrypoint.writeRef());

            if (!pEntrypoint || SLANG_FAILED(result))
                continue;

            entrypoints.emplace_back(pEntrypoint);
        }
    }

    return entrypoints;
}
