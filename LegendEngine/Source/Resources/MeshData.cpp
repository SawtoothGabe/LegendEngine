#include <LE/Application.hpp>
#include <LE/Resources/MeshData.hpp>

namespace le
{
    MeshData::MeshData()
        :
        m_vertexCount(0),
        m_indexCount(0)
    {

    }

    size_t MeshData::GetVertexCount() const
    {
        return m_vertexCount;
    }

    size_t MeshData::GetIndexCount() const
    {
        return m_indexCount;
    }

    Ref<MeshData> MeshData::Create(std::span<Vertex3> vertices, std::span<uint32_t> indices, UpdateFrequency frequency)
    {
        return std::make_shared<MeshData>();
    }
}
