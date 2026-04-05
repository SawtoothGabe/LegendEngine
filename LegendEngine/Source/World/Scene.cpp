#include <ranges>
#include <LE/Application.hpp>
#include <LE/Graphics/GraphicsContext.hpp>
#include <LE/World/Entity.hpp>
#include <LE/World/Scene.hpp>

namespace le
{
    Scene::Scene()
    {}

    Scene::~Scene()
    {

    }

    Entity Scene::CreateEntity()
    {
        const Entity obj(*this);
        m_entities[obj.uid] = {};

        LE_DEBUG("Added object with uid {} to scene {:#x}",
            static_cast<uint64_t>(obj.uid), reinterpret_cast<size_t>(this));

        return obj;
    }

    bool Scene::HasEntity(const Entity& entity) const
    {
        return HasEntity(entity.uid);
    }

    void Scene::DeleteEntity(const Entity& entity)
    {
        return DeleteEntity(entity.uid);
    }

    bool Scene::HasEntity(const UID entity) const
    {
        return m_entities.contains(entity);
    }

    void Scene::DeleteEntity(const UID entity)
    {
        auto [archetypeID, row] = m_entities.at(entity);
        m_entities.erase(entity);

        // If an entity has no archetype, it has no components, so there is nothing to delete
        if (!m_archetypes.contains(archetypeID))
            return;

        Archetype& archetype = m_archetypes[archetypeID];

        // Swap and pop the entity ID
        archetype.entityIDs[row] = archetype.entityIDs.back();
        archetype.entityIDs.pop_back();

        // Pop all the components
        for (ComponentStorage& storage : archetype.componentData | std::views::values)
            storage.SwapAndPop(row);

        ClearCachedArchetypeLookups();
    }

    void Scene::SetAmbientLight(const float level)
    {
        m_storage.ambientLight = level;
    }

    void Scene::Clear()
    {
        m_entities.clear();
        m_findArchetypeResults.clear();
        m_archetypes.clear();
    }

    void Scene::ClearCachedArchetypeLookups()
    {
        m_findArchetypeResults.clear();
    }

    void Scene::EnqueueEntityCreation(EntityCreator&& creator)
    {
        std::scoped_lock lock(m_creationMutex);
        m_queuedCreations.push_back(std::move(creator));
    }

    void Scene::EnqueueEntityDeletion(const Entity& entity)
    {
        EnqueueEntityDeletion(entity.uid);
    }

    void Scene::EnqueueEntityDeletion(const UID entity)
    {
        std::scoped_lock lock(m_deletionMutex);
        m_deletions.push_back(entity);
    }

    void Scene::ProcessEntityChanges()
    {
        ProcessDeletions();
        ProcessCreations();
    }

    void Scene::ProcessCreations()
    {
        std::scoped_lock lock(m_creationMutex);

        for (const EntityCreator& creator : m_queuedCreations)
        {
            UID entityID = creator.GetUID();

            std::vector<size_t> sortedIDs;
            sortedIDs.reserve(creator.GetComponents().size());
            for (size_t id : creator.GetComponents() | std::views::keys)
                sortedIDs.push_back(id);

            std::ranges::sort(sortedIDs);

            // Find an archetype with the component IDs
            const size_t archetypeID = ECS::HashIDs(sortedIDs);
            Archetype& archetype = m_archetypes[archetypeID];

            ECS::EntityRecord record;
            record.row = archetype.entityIDs.size();
            record.archetypeID = archetypeID;

            archetype.entityIDs.push_back(entityID);
            archetype.componentIDs = sortedIDs;

            for (auto& [componentID, data] : creator.GetComponents())
            {
                archetype.componentData.try_emplace(componentID, ComponentStorage(data.GetElementSize()));
                memcpy(archetype.componentData.at(componentID).Allocate(), data.GetData(0), data.GetElementSize());
            }

            m_entities.emplace(entityID, record);
        }

        if (!m_queuedCreations.empty())
            ClearCachedArchetypeLookups();

        m_queuedCreations.clear();
    }

    void Scene::ProcessDeletions()
    {
        std::scoped_lock lock(m_deletionMutex);

        for (const UID entity : m_deletions)
            DeleteEntity(entity);

        m_deletions.clear();
    }
}
