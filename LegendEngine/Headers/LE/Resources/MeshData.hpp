#pragma once

#include <LE/Common/Types.hpp>
#include <LE/Resources/Resource.hpp>

#include <span>

namespace le
{
    class GraphicsResources;
    class MeshData : public Resource
    {
        struct Passkey {};
    public:
        enum class UpdateFrequency
        {
            UPDATES_ONCE,
            UPDATES_OCCASIONALLY,
            UPDATES_OFTEN,
        };

        struct Vertex3
        {
            float position[3];
            float texcoord[2];
        };

        MeshData(GraphicsResources& resources, const MeshID& handle, Passkey);
        ~MeshData() override;

        void Update(std::span<Vertex3> vertices, std::span<uint32_t> indices);
        void Resize(size_t vertexCount, size_t indexCount) const;

        [[nodiscard]] MeshID GetHandle() const;

        static Ref<MeshData> Create(std::span<Vertex3> vertices, std::span<uint32_t> indices,
            UpdateFrequency frequency);
        static Ref<MeshData> Create(size_t initialVertexCount, size_t initialIndexCount,
            UpdateFrequency frequency);
    private:
        GraphicsResources& m_resources;
        MeshID m_handle;
    };
}
