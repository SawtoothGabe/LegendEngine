#include <ranges>
#include <LE/Application.hpp>
#include <LE/Graphics/GraphicsContext.hpp>
#include <LE/World/Entity.hpp>
#include <LE/World/Scene.hpp>
#include <LE/Components/Light.hpp>

namespace le
{
#ifndef LE_HEADLESS
    Scene::Scene()
        :
        Scene(Application::Get().GetGraphicsContext(), Application::Get().GetGraphicsResources())
    {}

    Scene::Scene(GraphicsContext& context, const GraphicsResources& resources)
        :
        m_context(context)
    {
        m_buffer = context.CreatePerFrameBuffer(Buffer::UsageFlags::STORAGE_BUFFER,
            sizeof(Storage));
        m_uniforms = context.CreateDynamicUniforms(
            DynamicUniforms::UpdateFrequency::PER_FRAME, resources.GetSceneLayout());

        m_lightData.reserve(16);
    }

    Scene::~Scene()
    {
       m_context.WaitIdle();
    }
#endif

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

    void Scene::UpdateUniforms()
    {
        m_lightData.clear();
        QueryComponents<Transform, Light>([this](const Transform& transform, Light& light)
        {
            m_lightData.push_back({
                Vector4f(transform.GetPosition(), 0.0f),
                light.color,
                static_cast<uint32_t>(light.type),
            });
        });

        size_t bufferSize = sizeof(Storage) + sizeof(LightData) * m_lightData.size();

        if (m_buffer->GetSize() < bufferSize)
        {
            m_buffer->Resize(bufferSize);
            m_uniforms->Invalidate();
        }

        m_buffer->Update(sizeof(Storage), 0, &m_storage);
        m_buffer->Update(sizeof(LightData) * m_lightData.size(), sizeof(Storage), m_lightData.data());

        m_uniforms->UpdateStorageBuffer(*m_buffer, 0);
    }

    DynamicUniforms& Scene::GetUniforms() const
    {
        return *m_uniforms;
    }
}
