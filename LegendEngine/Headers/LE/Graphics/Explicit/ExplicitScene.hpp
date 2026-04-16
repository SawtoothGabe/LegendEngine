#pragma once
#include <LE/Graphics/Types.hpp>

namespace le
{
    class ExplicitScene final
    {
    public:
        explicit ExplicitScene(const ExplicitRenderer& renderer);

        void SetAmbientLight(float level);
        void SetLightCount(size_t count);
        void UpdateLightData(size_t index, const SceneLightData& data);

        void UpdateUniforms();

        [[nodiscard]] DescriptorSetID GetSet(size_t index) const;
    private:
        ExplicitDriver& m_driver;

        SceneStorage m_storage;

        std::vector<DescriptorSetID> m_sets;
        PerFrameBuffer m_storageBuffer; // not a storage buffer lol, just a uniform buffer for the storage
        PerFrameBuffer m_lightsBuffer; // this is a storage buffer
    };
}
