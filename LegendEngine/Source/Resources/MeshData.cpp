#include <LE/Application.hpp>
#include <LE/Resources/MeshData.hpp>

namespace le
{
    MeshData::MeshData(const std::span<Vertex3> vertices, const std::span<uint32_t> indices, UpdateFrequency frequency)
        :
        m_vertexCount(vertices.size()),
        m_indexCount(indices.size())
    {
    }

    MeshData::MeshData(const size_t initialVertexCount, const size_t initialIndexCount, const UpdateFrequency frequency)
        :
        m_vertexCount(initialVertexCount),
        m_indexCount(initialIndexCount)
    {
        GraphicsContext& context = Application::Get().GetGraphicsContext();
        const size_t vertexSize = m_vertexCount * sizeof(Vertex3);
        const size_t indexSize = m_indexCount * sizeof(uint32_t);

        LE_ASSERT(frequency != UpdateFrequency::UPDATES_ONCE, "MeshData made with UpdateFrequency::UPDATES_ONCE must specify vertex data");

        CreateBuffer(vertexSize, indexSize, frequency, context);
    }

    size_t MeshData::GetVertexCount() const
    {
        return m_vertexCount;
    }

    size_t MeshData::GetIndexCount() const
    {
        return m_indexCount;
    }
}
