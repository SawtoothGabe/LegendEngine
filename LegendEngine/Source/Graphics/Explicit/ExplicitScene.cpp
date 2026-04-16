#include <LE/Application.hpp>
#include <LE/Graphics/Explicit/ExplicitResources.hpp>
#include <LE/Graphics/Explicit/ExplicitScene.hpp>

namespace le
{
    ExplicitScene::ExplicitScene(ExplicitResources& resources)
        :
        m_driver(resources.GetDriver()),
        m_storageBuffer(resources, BufferUsageFlagBits::UNIFORM_BUFFER, sizeof(SceneStorage)),
        m_lightsBuffer(resources, BufferUsageFlagBits::STORAGE_BUFFER, sizeof(SceneLightData) * 1024)
    {
        m_sets = m_driver.AllocateDescriptorSets(
            resources.GetScenePoolManager(),
            m_descriptorPool,
            Application::FRAMES_IN_FLIGHT
        );

        for (size_t i = 0; i < m_sets.size(); ++i)
        {
            DescriptorBufferInfo storageInfo {
                .buffer = m_storageBuffer.GetDesc(i).buffer,
                .range = sizeof(SceneStorage),
            };

            DescriptorBufferInfo lightsInfo {
                .buffer = m_lightsBuffer.GetDesc(i).buffer,
                .range = sizeof(SceneLightData) * 1024,
            };

            WriteDescriptorSet writes[2]{};
            writes[0].dstSet = m_sets[i];
            writes[0].pBufferInfo = &storageInfo;

            writes[1].dstSet = m_sets[i];
            writes[1].dstBinding = 1,
            writes[1].descriptorType = DescriptorType::STORAGE_BUFFER,
            writes[1].pBufferInfo = &lightsInfo;

            m_driver.UpdateDescriptorSets(writes);
        }
    }

    void ExplicitScene::SetAmbientLight(const float level)
    {
        m_storage.ambientLight = level;
    }

    void ExplicitScene::SetLightCount(const size_t count)
    {
        m_storage.lightCount = static_cast<uint32_t>(count);
    }

    void ExplicitScene::UpdateLightData(const size_t index, const SceneLightData& data)
    {
        const size_t frame = Application::Get().GetCurrentFrame();
        const BufferID buffer = m_lightsBuffer.GetDesc(frame).buffer;

        void* pData = m_driver.GetMappedBufferData(buffer);
        memcpy(static_cast<SceneLightData*>(pData) + index, &data, sizeof(SceneLightData));
    }

    void ExplicitScene::UpdateUniforms()
    {
        const size_t frame = Application::Get().GetCurrentFrame();
        const BufferID buffer = m_storageBuffer.GetDesc(frame).buffer;

        memcpy(m_driver.GetMappedBufferData(buffer), &m_storage, sizeof(m_storage));
    }

    DescriptorSetID ExplicitScene::GetSet(const size_t index) const
    {
        return m_sets[index];
    }
}
