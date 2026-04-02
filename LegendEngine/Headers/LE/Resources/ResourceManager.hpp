#pragma once

#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <LE/Common/Assert.hpp>
#include <LE/Common/Defs.hpp>
#include <LE/Common/Types.hpp>
#include <LE/Common/UID.hpp>
#include <LE/IO/Logger.hpp>
#include <LE/Resources/Resource.hpp>

namespace le
{
    class ResourceManager
    {
    public:
        ResourceManager() = default;
        LE_NO_COPY(ResourceManager);

        template<typename T, typename... Args>
        Ref<T> CreateResource(Args&&... args)
        {
            Ref<T> resource = std::make_shared<T>(std::forward<Args>(args)...);

            LE_ASSERT(!m_resources.contains(resource->m_uid), "UID already exists");

            {
                std::unique_lock lock(m_resourcesMutex);
                m_resources[resource->m_uid] = resource;
            }

            LE_DEBUG("Resource created (type = {}, id = {})", typeid(T).name(), resource->m_uid.Get());

            return resource;
        }

        template<typename T>
        bool HasResource(const Resource::ID<T> id)
        {
            std::shared_lock lock(m_resourcesMutex);
            return m_resources.contains(id.Get()) && !m_resources.at(id)->m_deleted;
        }

        template<typename T>
        Ref<T> GetResource(const Resource::ID<T> id)
        {
            std::shared_lock lock(m_resourcesMutex);

            LE_ASSERT(HasResource<T>(id), "Tried to access resource (id = {}) which doesn't exist", id.Get());

            Ref<T> resource = std::static_pointer_cast<T>(m_resources.at(id.Get()));
            LE_ASSERT(!resource->m_deleted, "Tried to access deleted resource {}", id.Get());

            return resource;
        }

        template<typename T>
        void Delete(const Resource::ID<T> id)
        {
            LE_ASSERT(HasResource<T>(id), "Tried to delete nonexistent resource {}",
                id.Get());

            Ref<T> resource = GetResource<T>(id);

            LE_ASSERT(!resource->m_deleted, "Resource {} was deleted twice", id.Get());

            resource->m_deleted = true;
        }

        void DeleteAll();

        void ProcessDeletedResources();
    private:
        std::shared_mutex m_resourcesMutex;
        std::unordered_map<UID, Ref<Resource>> m_resources;
    };
}
