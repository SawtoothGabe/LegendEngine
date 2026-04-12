#pragma once

#include <LE/Graphics/Explicit/Buffer.hpp>
#include <LE/Graphics/Explicit/ExplicitRenderer.hpp>
#include <LE/Resources/MeshData.hpp>

namespace le
{
    class ExplicitMeshData : public MeshData
    {
    public:
        enum class UpdateFrequency
        {
            UPDATES_ONCE,
            UPDATES_OCCASIONALLY,
            UPDATES_OFTEN,
        };

        ExplicitMeshData(ExplicitRenderer& renderer, std::span<Vertex3> vertices, std::span<uint32_t> indices,
            UpdateFrequency frequency);
        ExplicitMeshData(ExplicitRenderer& renderer, size_t initialVertexCount, size_t initialIndexCount,
            UpdateFrequency frequency);

        void Update(std::span<Vertex3> vertices, std::span<uint32_t> indices);
        void Resize(size_t vertexCount, size_t indexCount);

        size_t GetVertexCount() const;
        size_t GetIndexCount() const;

        Buffer& GetVertexBuffer() const;
        Buffer& GetIndexBuffer() const;
    private:
        void CreateBuffer(ExplicitRenderer& renderer, size_t vertexSize, size_t indexSize, UpdateFrequency frequency);

        Scope<Buffer> m_vertexBuffer;
        Scope<Buffer> m_indexBuffer;

        size_t m_vertexCount;
        size_t m_indexCount;
    };
}
