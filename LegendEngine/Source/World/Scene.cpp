#include <ranges>
#include <LE/Application.hpp>
#include <LE/Graphics/GraphicsContext.hpp>
#include <LE/World/Entity.hpp>
#include <LE/World/Scene.hpp>

namespace le
{
    Scene::Scene()
        :
        m_resources(&Application::Get().GetGraphicsContext().GetResources()),
        m_handle(m_resources->CreateScene())
    {}

    Scene::Scene(GraphicsResources& resources)
        :
        m_resources(&resources),
        m_handle(resources.CreateScene())
    {}

    Scene::~Scene()
    {
        m_resources->DestroyScene(m_handle);
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
        if (row < archetype.entityIDs.size() - 1)
        {
            const UID movedEntity = archetype.entityIDs.back();
            archetype.entityIDs[row] = movedEntity;

            // If this entity isn't the last element in the archetype entities,
            // then update the moved entity's row
            m_entities.at(movedEntity).row = row;
        }

        archetype.entityIDs.pop_back();

        // Pop all the components
        for (const auto& storage : archetype.componentData | std::views::values)
            storage->Remove(row);

        ClearCachedArchetypeLookups();
    }

    void Scene::SetAmbientLight(const float level) const
    {
        m_resources->SetSceneAmbientLight(m_handle, level);
    }

    void Scene::Clear()
    {
        m_entities.clear();
        m_findArchetypeResults.clear();
        m_archetypes.clear();
    }

    SceneID Scene::GetHandle() const
    {
        return m_handle;
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
                if (!archetype.componentData.contains(componentID))
                    archetype.componentData.emplace(componentID, data->CloneEmpty());

                archetype.componentData.at(componentID)->MoveFrom(0, *data);
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
