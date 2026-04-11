#pragma once

#include <string_view>
#include <vector>

#include <simdjson.h>
#include <slang.h>

class ModuleRegistry;

class Program
{
public:
    enum class Feature
    {
        TEXTURED
    };

    static Program FromJson(ModuleRegistry& registry, std::string_view path, std::string_view json,
        simdjson::ondemand::parser& parser);
    static Program FromSlang(ModuleRegistry& registry, std::string_view path);

    Slang::ComPtr<slang::IComponentType> Link(slang::ISession* session);
private:
    Program() = default;

    void AddFeature(std::string_view feature);
    std::vector<Slang::ComPtr<slang::IEntryPoint>> GetEntrypoints();

    std::vector<Feature> m_features;
    std::vector<Slang::ComPtr<slang::IModule>> m_modules;
};
