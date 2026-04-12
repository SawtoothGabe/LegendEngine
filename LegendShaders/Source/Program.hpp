#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <simdjson.h>
#include <slang-com-ptr.h>
#include <slang.h>

namespace le::sh
{
    enum class Features;
}

class ModuleRegistry;

class Program
{
public:
    static Program FromJson(ModuleRegistry& registry, std::string_view path, std::string_view json,
        simdjson::ondemand::parser& parser);
    static Program FromSlang(ModuleRegistry& registry, std::string_view path);

    Slang::ComPtr<slang::IComponentType> Link(slang::ISession* session) const;
    [[nodiscard]] std::string GetFilenameHash() const;
    const std::vector<Slang::ComPtr<slang::IEntryPoint>>& GetEntrypoints() const;
    le::sh::Features GetFeatures() const;
private:
    Program(std::string_view path, const Slang::ComPtr<slang::IModule>& module);
    Program(std::string_view path, le::sh::Features features, std::vector<Slang::ComPtr<slang::IModule>> modules);

    static std::string GetHashedName(std::string_view path);
    static le::sh::Features GetFeature(std::string_view feature);
    std::vector<Slang::ComPtr<slang::IEntryPoint>> MakeEntrypoints();

    std::string m_filenameHash;
    le::sh::Features m_features;
    std::vector<Slang::ComPtr<slang::IModule>> m_modules;
    std::vector<Slang::ComPtr<slang::IEntryPoint>> m_entrypoints;
};
