#pragma once

#include <filesystem>
#include <slang.h>
#include <string>
#include <unordered_map>

class ModuleRegistry
{
public:
    ModuleRegistry(slang::ISession* session);

    Slang::ComPtr<slang::IModule> TryCreate(const std::filesystem::path& path);
    Slang::ComPtr<slang::IModule> Get(const std::filesystem::path& path);
    slang::ISession* GetSession() const;
private:
    slang::ISession* m_session = nullptr;
    std::unordered_map<std::filesystem::path, Slang::ComPtr<slang::IModule>> m_modules;
};
