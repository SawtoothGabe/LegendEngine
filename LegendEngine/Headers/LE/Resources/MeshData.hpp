#pragma once

#include <LE/Common/Types.hpp>
#include <LE/Resources/Resource.hpp>

#include <span>

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
            float texcoord[2];
        };

        MeshData();

        [[nodiscard]] virtual size_t GetVertexCount() const;
        [[nodiscard]] virtual size_t GetIndexCount() const;

        MeshID GetHandle() const;

        static Ref<MeshData> Create(std::span<Vertex3> vertices, std::span<uint32_t> indices,
            UpdateFrequency frequency);
    private:
        size_t m_vertexCount;
        size_t m_indexCount;
    };
}
