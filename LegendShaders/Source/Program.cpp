#include <LE/Graphics/ShaderInfo.hpp>

#include "Program.hpp"

#include <filesystem>
#include <future>
#include <ModuleRegistry.hpp>
#include <print>
#include <simdjson.h>
#include <slang-com-ptr.h>
#include <slang.h>
#include <SlangUtils.hpp>
#include <utility>

using namespace simdjson;

le::Features operator|=(le::Features& lhs, const le::Features& rhs)
{
    return lhs = static_cast<le::Features>(static_cast<size_t>(lhs) | static_cast<size_t>(rhs));
}

Program Program::FromJson(ModuleRegistry& registry, const std::string_view path, const std::string_view json, ondemand::parser& parser)
{
    if (json.empty())
        std::println("warn: input JSON is empty");

    const std::filesystem::path jsonPath(path);
    const padded_string paddedJson = json;
    le::Features features = {};
    std::vector<Slang::ComPtr<slang::IModule>> modules;

    ondemand::document doc = parser.iterate(paddedJson);
    std::vector<std::filesystem::path> modulePaths;
    for (auto field : doc.get_object())
    {
        const std::string_view key = field.escaped_key();
        if (key == "features")
            for (auto el : field.value().get_array())
                features |= GetFeature(el.get_string().value());
        else if (key == "modules")
            for (auto el : field.value().get_array())
                modulePaths.emplace_back(el.get_string().value());
        else
            std::println("{}: warning: unused field \"{}\"",
                std::filesystem::path(path).filename().string(), key);
    }

    if (modulePaths.empty())
        throw std::runtime_error(std::format("shader config file \"{}\" references no modules", path));

    for (const auto& module : modulePaths)
    {
        const std::filesystem::path relative = jsonPath.parent_path() / module;
        modules.push_back(registry.TryCreate(relative));
    }

    return {path, features, modules};
}

Program Program::FromSlang(ModuleRegistry& registry, const std::string_view path)
{
    return { path, registry.TryCreate(path) };
}

Slang::ComPtr<slang::IComponentType> Program::Link(slang::ISession* session) const
{
    std::vector<slang::IComponentType*> components;
    for (const auto& module : m_modules)
        components.push_back(module.get());

    for (const auto& entrypoint : m_entrypoints)
        components.emplace_back(entrypoint);

    Slang::ComPtr<slang::IBlob> diagnostics;
    Slang::ComPtr<slang::IComponentType> composedProgram;
    session->createCompositeComponentType(
        components.data(),
        static_cast<SlangInt>(components.size()),
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

std::string Program::GetFilenameHash() const
{
    return m_filenameHash;
}

std::string Program::GetName() const
{
    return m_name;
}

const std::vector<Slang::ComPtr<slang::IEntryPoint>>& Program::GetEntrypoints() const
{
    return m_entrypoints;
}

le::Features Program::GetFeatures() const
{
    return m_features;
}

Program::Program(const std::string_view path, const Slang::ComPtr<slang::IModule>& module)
    :
    m_name(GetName(path)),
    m_filenameHash(GetHashedName())
{
    m_modules.emplace_back(module);

    m_entrypoints = MakeEntrypoints();

    if (m_entrypoints.empty())
        throw std::runtime_error(std::format("shader program \"{}\" has no entrypoints", path));
}

Program::Program(const std::string_view path, le::Features features,
                 std::vector<Slang::ComPtr<slang::IModule>> modules)
    :
    m_name(GetName(path)),
    m_filenameHash(GetHashedName()),
    m_features(features),
    m_modules(std::move(modules))
{
    m_entrypoints = MakeEntrypoints();

    if (m_entrypoints.empty())
        throw std::runtime_error(std::format("shader program \"{}\" has no entrypoints", path));
}

std::string Program::GetName(const std::string_view path)
{
    const std::string filename = std::filesystem::path(path).stem().string();
    if (filename.empty())
        throw std::runtime_error("shader filename cannot be empty");

    return filename;
}

std::string Program::GetHashedName() const
{
    constexpr std::hash<std::string> hasher;

    size_t hash = hasher(m_name);
    return std::format("{:x}", hash);
}

le::Features Program::GetFeature(const std::string_view feature)
{
    if (feature == "solid_color")
        return le::Features::SOLID_COLOR;
    if (feature == "textured")
        return le::Features::TEXTURED;

    throw std::runtime_error(std::format("Invalid feature \"{}\"", feature));
}

std::vector<Slang::ComPtr<slang::IEntryPoint>> Program::MakeEntrypoints()
{
    std::vector<Slang::ComPtr<slang::IEntryPoint>> entrypoints;
    for (Slang::ComPtr<slang::IModule>& module : m_modules)
    {
        uint32_t entrypointCount = module->getDefinedEntryPointCount();
        for (uint32_t i = 0; i < entrypointCount; i++)
        {
            Slang::ComPtr<slang::IEntryPoint> pEntrypoint = nullptr;
            SlangResult result = module->getDefinedEntryPoint(static_cast<SlangInt32>(i), pEntrypoint.writeRef());

            if (!pEntrypoint || SLANG_FAILED(result))
                continue;

            entrypoints.emplace_back(pEntrypoint);
        }
    }

    return entrypoints;
}
