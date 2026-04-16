#pragma once

#include <LE/Graphics/Explicit/Buffer.hpp>
#include <LE/Graphics/Explicit/ExplicitResources.hpp>
#include <LE/Resources/MeshData.hpp>

namespace le
{
    class ExplicitMesh final
    {
    public:
        ExplicitMesh(ExplicitResources& resources, std::span<MeshData::Vertex3> vertices, std::span<uint32_t> indices,
            MeshData::UpdateFrequency frequency);
        ExplicitMesh(ExplicitResources& resources, size_t initialVertexCount, size_t initialIndexCount,
            MeshData::UpdateFrequency frequency);

        void Update(std::span<MeshData::Vertex3> vertices, std::span<uint32_t> indices);
        void Resize(size_t vertexCount, size_t indexCount);

        [[nodiscard]] size_t GetVertexCount() const;
        [[nodiscard]] size_t GetIndexCount() const;

        [[nodiscard]] Buffer& GetVertexBuffer() const;
        [[nodiscard]] Buffer& GetIndexBuffer() const;
    private:
        void CreateBuffer(ExplicitResources& resources, size_t vertexSize, size_t indexSize, MeshData::UpdateFrequency frequency);

        Scope<Buffer> m_vertexBuffer;
        Scope<Buffer> m_indexBuffer;

        size_t m_vertexCount;
        size_t m_indexCount;
    };
}
