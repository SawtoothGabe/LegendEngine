#pragma once

#include <LE/Common/Types.hpp>
#include <LE/Graphics/API/Buffer.hpp>
#include <LE/Resources/Resource.hpp>

namespace le
{
    class MeshData : public Resource
    {
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
            float texcoord[3];
        };

        MeshData(std::span<Vertex3> vertices, std::span<uint32_t> indices,
            UpdateFrequency frequency);
        MeshData(size_t initialVertexCount, size_t initialIndexCount,
            UpdateFrequency frequency);

        void Update(std::span<Vertex3> vertices, std::span<uint32_t> indices);
        void Resize(size_t vertexCount, size_t indexCount);

        size_t GetVertexCount() const;
        size_t GetIndexCount() const;

        Buffer& GetVertexBuffer() const;
        Buffer& GetIndexBuffer() const;

        ID<MeshData> id = ID<MeshData>(m_uid);
    private:
        Scope<Buffer> m_vertexBuffer;
        Scope<Buffer> m_indexBuffer;

        size_t m_vertexCount;
        size_t m_indexCount;
    };
}
