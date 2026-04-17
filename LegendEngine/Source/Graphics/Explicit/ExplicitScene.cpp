#include <LE/Application.hpp>
#include <LE/Graphics/Explicit/ExplicitResources.hpp>
#include <LE/Graphics/Explicit/ExplicitScene.hpp>

namespace le
{
    ExplicitScene::ExplicitScene(ExplicitResources& resources)
        :
        m_driver(resources.GetDriver()),
        m_storageBuffer(resources, BufferUsageFlagBits::UNIFORM_BUFFER, sizeof(SceneStorage), true),
        m_lightsBuffer(resources, BufferUsageFlagBits::STORAGE_BUFFER, sizeof(SceneLightData), true),
        m_poolManager(resources.GetScenePoolManager())
    {
        m_framesUntilSetsValid = Application::FRAMES_IN_FLIGHT;
        m_sets = m_driver.AllocateDescriptorSets(
            m_poolManager,
            m_descriptorPool,
            Application::FRAMES_IN_FLIGHT
        );
    }

    ExplicitScene::~ExplicitScene()
    {
        m_driver.FreeDescriptorSets(m_poolManager, m_descriptorPool,
            m_sets.size(), m_sets.data());
    }

    void ExplicitScene::SetAmbientLight(const float level)
    {
        m_storage.ambientLight = level;
    }

    void ExplicitScene::StartFrame(const size_t frame, const size_t lightCount)
    {
        if (m_lightCount != lightCount)
        {
            m_lightsBuffer.Resize(sizeof(SceneLightData) * lightCount);
            m_framesUntilSetsValid = Application::FRAMES_IN_FLIGHT;
            m_lightCount = lightCount;
        }
    }

    void ExplicitScene::UpdateLightData(const size_t frame, const size_t index, const SceneLightData& data)
    {
        m_lightsBuffer.Update(sizeof(SceneLightData), sizeof(SceneLightData) * index, &data, frame);
    }

    void ExplicitScene::UpdateUniforms(const size_t frame)
    {
        m_storageBuffer.Update(sizeof(SceneStorage), 0, &m_storage, frame);

        if (m_framesUntilSetsValid)
            UpdateSets(frame);
    }

    DescriptorSetID ExplicitScene::GetSet(const size_t index) const
    {
        return m_sets[index];
    }

    void ExplicitScene::UpdateSets(const size_t frame)
    {
        DescriptorBufferInfo storageInfo {
            .buffer = m_storageBuffer.GetDesc(frame).buffer,
            .range = sizeof(SceneStorage),
        };

        DescriptorBufferInfo lightsInfo {
            .buffer = m_lightsBuffer.GetDesc(frame).buffer,
            .range = sizeof(SceneLightData) * std::max<size_t>(1, m_lightCount),
        };

        WriteDescriptorSet writes[2]{};
        writes[0].dstSet = m_sets[frame];
        writes[0].pBufferInfo = &storageInfo;

        writes[1].dstSet = m_sets[frame];
        writes[1].dstBinding = 1,
        writes[1].descriptorType = DescriptorType::STORAGE_BUFFER,
        writes[1].pBufferInfo = &lightsInfo;

        m_driver.UpdateDescriptorSets(writes);

        m_framesUntilSetsValid--;
    }
}
