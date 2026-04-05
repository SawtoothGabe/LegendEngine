#pragma once

#include <unordered_map>
#include <vector>
#include <LE/Common/Defs.hpp>
#include <LE/Common/UID.hpp>
#include <LE/Math/Math.hpp>
#include <LE/World/Archetype.hpp>
#include <LE/World/ECS.hpp>

#include <LE/World/EntityCreator.hpp>

namespace le
{
    struct PointLight;
    class GraphicsResources;
    class GraphicsContext;
    class Entity;
    class Scene final
    {
        friend Entity;
    public:
        Scene();
        ~Scene();
        LE_NO_COPY(Scene);

        Entity CreateEntity();
        void EnqueueEntityCreation(EntityCreator&& creator);
        void EnqueueEntityDeletion(const Entity& entity);
        void EnqueueEntityDeletion(UID entity);

        // These just call the functions with UID
        bool HasEntity(const Entity& entity) const;
        void DeleteEntity(const Entity& entity);

        bool HasEntity(UID entity) const;
        void DeleteEntity(UID entity);

        template <typename T, typename... Args>
            requires std::is_base_of_v<Component, T>
        void AddComponent(const UID entity, Args&&... args)
        {
            ECS::EntityRecord& record = m_entities.at(entity);

            if (!m_archetypes.contains(record.archetypeID))
            {
                AddComponentToEmptyEntity<T>(record, entity, std::forward<Args>(args)...);
                return;
            }

            const size_t componentID = typeid(T).hash_code();

            Archetype& oldArchetype = m_archetypes[record.archetypeID];

            // Make sure the entity doesn't already have the component
            for (const size_t id : oldArchetype.componentIDs)
                LE_ASSERT(id != componentID, "Component added twice");

            // The entity does have components, so it needs to go in a new archetype
            // Get a list of component IDs with the new one added, hash it, check if it exists.

            std::vector<size_t> newComponentIDs = ECS::AddComponentToSortedIDList(oldArchetype, componentID);

            const size_t newArchetypeID = ECS::HashIDs(newComponentIDs);
            const bool createdNewArchetype = !m_archetypes.contains(newArchetypeID);
            Archetype& newArchetype = m_archetypes[newArchetypeID];

            // The new archetype now exists. It may or may not have components.
            // Add the entity to it, remove the entity from the old archetype.

            newArchetype.MoveComponentsFrom(oldArchetype, record.row);

            if (createdNewArchetype)
            {
                // Copy component IDs to the new archetype
                newArchetype.componentIDs = newComponentIDs;
                // Same for entity IDs
                newArchetype.entityIDs = oldArchetype.entityIDs;

                // Ensure the storage exists for the new component
                newArchetype.componentData.try_emplace(componentID, sizeof(T));
            }

            // Update the record
            record.archetypeID = newArchetypeID;
            record.row = newArchetype.entityIDs.size();

            // Add this entity ID to the new archetype.
            // The new component is being added to the end of the components vector,
            // so pushing it back will align it correctly with the components
            newArchetype.entityIDs.push_back(entity);

            // Finally, create the new component
            newArchetype.componentData.at(componentID).Push<T>(args...);

            ClearCachedArchetypeLookups();
        }

        template <typename T>
            requires std::is_base_of_v<Component, T>
        void RemoveComponent(const UID entity)
        {
            auto& [archetypeID, row] = m_entities.at(entity);
            if (!m_archetypes.contains(archetypeID))
                return; // The entity has no components

            Archetype& archetype = m_archetypes[archetypeID];
            const size_t componentID = typeid(T).hash_code();

            if (archetype.componentIDs.empty())
                return;

            if (!archetype.componentData.contains(componentID))
                return; // This really shouldn't happen

            // If removing this component leaves the entity without components, then this is easy
            if (archetype.componentIDs.size() == 1)
            {
                // Just swap and pop the ID
                archetype.entityIDs[row] = archetype.entityIDs.back();
                archetype.entityIDs.pop_back();

                // ...components
                archetype.componentData.at(componentID).SwapAndPop(row);

                // ...and reset the record
                archetypeID = -1;
                row = -1;
                return;
            }

            // If removing leaves the entity still with components, it needs to go in a new archetype

            // Create or find a new archetype
            // Get the components of the current archetype without the one to remove
            std::vector<size_t> newComponentIDs = archetype.componentIDs;
            for (size_t i = 0; i < newComponentIDs.size(); ++i)
            {
                if (newComponentIDs[i] == componentID)
                {
                    newComponentIDs.erase(newComponentIDs.begin() + static_cast<int64_t>(i));
                    break;
                }
            }

            // Find an archetype with the component IDs
            const size_t newArchetypeID = ECS::HashIDs(newComponentIDs);
            Archetype& newArchetype = m_archetypes[newArchetypeID];

            newArchetype.MoveComponentsFrom(archetype, row);

            archetypeID = newArchetypeID;
            row = newArchetype.entityIDs.size();

            newArchetype.entityIDs.push_back(entity);

            ClearCachedArchetypeLookups();
        }

        template <typename T>
        T GetComponentData(const UID entity)
        {
            auto& [archetypeID, row] = m_entities.at(entity);
            const size_t componentID = typeid(T).hash_code();
            auto& componentData = m_archetypes.at(archetypeID).componentData;

            return *componentData.at(componentID).Get<T>(row);
        }

        // The entity must have the component or this function will fail
        template <typename T>
        void ReplaceComponent(const UID entity, T&& component)
        {
            auto& [archetypeID, row] = m_entities.at(entity);
            const size_t componentID = typeid(T).hash_code();
            auto& componentData = m_archetypes.at(archetypeID).componentData;

            T* pExistingData = componentData.at(componentID).Get<T>(row);
            pExistingData->~Component();
            *pExistingData = component;
        }

        // The entity must have the components queried, otherwise this will throw
        template <typename... Ts, typename Fn>
            requires std::invocable<Fn, Ts&...>
        void QueryEntityComponents(const UID entity, Fn queryFunc)
        {
            auto& [archetypeID, row] = m_entities.at(entity);
            Archetype& archetype = m_archetypes.at(archetypeID);

            queryFunc((*archetype.componentData.at(typeid(Ts).hash_code()).Get<Ts>(row))...);
        }

        template <typename... Ts, typename Fn>
            requires std::invocable<Fn, Ts&...>
        void QueryComponents(Fn queryFunc)
        {
            for (const std::vector<size_t> archetypes = FindArchetypes<Ts...>(); size_t id : archetypes)
            {
                Archetype& archetype = m_archetypes.at(id);

                for (size_t i = 0; i < archetype.entityIDs.size(); i++)
                    queryFunc((*archetype.componentData.at(typeid(Ts).hash_code()).Get<Ts>(i))...);
            }
        }

        template <typename... Ts, typename Fn>
            requires std::invocable<Fn, Archetype&, size_t /* row */, Ts&...>
        void QueryArchetypes(Fn queryFunc)
        {
            for (const std::vector<size_t> archetypes = FindArchetypes<Ts...>(); size_t id : archetypes)
            {
                Archetype& archetype = m_archetypes.at(id);

                for (size_t i = 0; i < archetype.entityIDs.size(); i++)
                    queryFunc(archetype, i, (*archetype.componentData.at(typeid(Ts).hash_code()).Get<Ts>(i))...);
            }
        }

        void SetAmbientLight(float level);
        void Clear();

        void ProcessEntityChanges();
    private:
        struct Storage
        {
            alignas(16) float ambientLight = 0.0f;
        };

        struct LightData
        {
            Vector4f position;
            Vector4f color;
            alignas(16) uint32_t type;
        };

        template<typename T, typename... Args>
        void AddComponentToEmptyEntity(ECS::EntityRecord& record, const UID entity, Args&&... args)
        {
            const size_t componentID = typeid(T).hash_code();

            // Create or find an archetype with just this component.
            // Since this entity doesn't have any components yet,
            // the archetype will only have the component being added
            Archetype& archetype = m_archetypes[ECS::ArchetypeID<T>()];

            // If the archetype doesn't have T (which also would mean it doesn't have any at all)
            if (!archetype.componentData.contains(componentID))
            {
                // Make sure it exists
                archetype.componentData.emplace(componentID, sizeof(T));
                archetype.componentIDs.push_back(componentID);
            }

            // Here, the archetype exists and there is component storage for it.
            // It may or may not have components in it, so we act like it always does.

            record.row = archetype.entityIDs.size();
            archetype.entityIDs.push_back(entity);
            archetype.componentData.at(componentID).Push<T>(args...);

            // Update the record
            record.archetypeID = ECS::ArchetypeID<T>();
        }

        template <typename... Ts>
        std::vector<size_t> FindArchetypes()
        {
            auto archetypeID = ECS::ArchetypeID<Ts...>();
            auto requiredIDs = ECS::GetSortedIDs<Ts...>();

            if (m_findArchetypeResults.contains(archetypeID))
                return m_findArchetypeResults.at(archetypeID);

            std::vector<size_t> result;
            for (auto& [ id, arch ] : m_archetypes)
                if (std::includes(arch.componentIDs.begin(), arch.componentIDs.end(),
                    requiredIDs.begin(), requiredIDs.end()) && !arch.entityIDs.empty())
                    result.emplace_back(id);

            auto cachedResult = m_findArchetypeResults.emplace(archetypeID, std::move(result));

            return cachedResult.first->second;
        }

        void ProcessCreations();
        void ProcessDeletions();
        void ClearCachedArchetypeLookups();

        std::unordered_map<UID, ECS::EntityRecord> m_entities;
        std::unordered_map<size_t, std::vector<size_t>> m_findArchetypeResults;
        std::unordered_map<size_t, Archetype> m_archetypes;

        std::mutex m_creationMutex;
        std::vector<EntityCreator> m_queuedCreations;

        std::mutex m_deletionMutex;
        std::vector<UID> m_deletions;

        Storage m_storage;
        std::vector<LightData> m_lightData;
    };
}
