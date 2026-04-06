#pragma once

#include <LE/Common/Types.hpp>
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

        MeshData();

        size_t GetVertexCount() const;
        size_t GetIndexCount() const;

        static Ref<MeshData> Create(std::span<Vertex3> vertices, std::span<uint32_t> indices,
            UpdateFrequency frequency);
    private:
        size_t m_vertexCount;
        size_t m_indexCount;
    };
}
