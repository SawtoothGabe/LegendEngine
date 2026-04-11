#include "ModuleRegistry.hpp"

#include <fstream>
#include <slang-com-ptr.h>
#include <SlangUtils.hpp>

ModuleRegistry::ModuleRegistry(slang::ISession* session)
    :
    m_session(session)
{}

Slang::ComPtr<slang::IModule> ModuleRegistry::TryCreate(const std::filesystem::path& path)
{
    if (m_modules.contains(path))
        return m_modules[path];

    const std::filesystem::path absolute = std::filesystem::absolute(path);
    const auto size = std::filesystem::file_size(absolute);
    std::string data(size, '\0');

    std::ifstream inputFile(absolute, std::ios::binary);
    inputFile.read(data.data(), static_cast<std::streamsize>(size));

    Slang::ComPtr<slang::IBlob> diagnostics;
    Slang::ComPtr<slang::IModule> module;
    module = m_session->loadModuleFromSourceString(
        path.stem().string().data(),
        path.string().data(), data.data(),
            diagnostics.writeRef());
    SlangUtils::Diagnose(diagnostics);

    return m_modules[path] = module;
}

Slang::ComPtr<slang::IModule> ModuleRegistry::Get(const std::filesystem::path& path)
{
    return m_modules.at(std::filesystem::absolute(path));
}

slang::ISession* ModuleRegistry::GetSession() const
{
    return m_session;
}
