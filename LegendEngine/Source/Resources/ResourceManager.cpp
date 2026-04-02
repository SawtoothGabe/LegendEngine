#include <LE/Resources/ResourceManager.hpp>

#include <ranges>
#include <LE/Application.hpp>

namespace le
{
    void ResourceManager::DeleteAll()
    {
        m_resources.clear();
    }

    void ResourceManager::ProcessDeletedResources()
    {
        std::unique_lock lock(m_resourcesMutex);

        for (const Ref resource: m_resources | std::views::values)
        {
            if (!resource->m_deleted)
                continue;

            resource->m_framesSinceDeletion++;

            if (resource->m_framesSinceDeletion >= Application::FRAMES_IN_FLIGHT)
                m_resources.erase(resource->m_uid);
        }
    }
}
