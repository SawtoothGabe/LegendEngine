#pragma once
#include <LE/Graphics/Types.hpp>
#include <LE/Graphics/Explicit/ExplicitResources.hpp>
#include <LE/Graphics/Explicit/PerFrameBuffer.hpp>

namespace le
{
    class ExplicitScene final
    {
    public:
        explicit ExplicitScene(ExplicitResources& resources);
        ~ExplicitScene();

        void SetAmbientLight(float level);
        void StartFrame(size_t frame, size_t lightCount);
        void UpdateLightData(size_t frame, size_t index, const SceneLightData& data);

        void UpdateUniforms(size_t frame);

        [[nodiscard]] DescriptorSetID GetSet(size_t index) const;
    private:
        void UpdateSets(size_t frame);

        size_t m_lightCount = 0;
        size_t m_framesUntilSetsValid = 0;

        ExplicitDriver& m_driver;

        SceneStorage m_storage;

        PoolManagerID m_poolManager;
        DescriptorPoolID m_descriptorPool;

        std::vector<DescriptorSetID> m_sets;
        PerFrameBuffer m_storageBuffer; // not a storage buffer lol, just a uniform buffer for the storage
        PerFrameBuffer m_lightsBuffer; // this is a storage buffer
    };
}
