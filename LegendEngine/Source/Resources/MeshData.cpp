#include <LE/Application.hpp>
#include <LE/Resources/MeshData.hpp>

namespace le
{
    Buffer::UsageFlags operator|(Buffer::UsageFlags lhs, Buffer::UsageFlags rhs)
    {
        return static_cast<Buffer::UsageFlags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
    }

    MeshData::MeshData(const std::span<Vertex3> vertices, const std::span<uint32_t> indices, UpdateFrequency frequency)
        :
        m_vertexCount(vertices.size()),
        m_indexCount(indices.size())
    {
        GraphicsContext& context = Application::Get().GetGraphicsContext();

        size_t vertexSize = m_vertexCount * sizeof(Vertex3);
        size_t indexSize = m_indexCount * sizeof(uint32_t);

        m_vertexBuffer = context.CreateSimpleBuffer(Buffer::UsageFlags::VERTEX_BUFFER | Buffer::UsageFlags::TRANSFER_DST,
            vertexSize, false);
        m_indexBuffer = context.CreateSimpleBuffer(Buffer::UsageFlags::INDEX_BUFFER | Buffer::UsageFlags::TRANSFER_DST,
            indexSize, false);

        Scope<Buffer> vertexStager = context.CreateSimpleBuffer(Buffer::UsageFlags::TRANSFER_SRC,
            vertexSize, true);
        Scope<Buffer> indexStager = context.CreateSimpleBuffer(Buffer::UsageFlags::TRANSFER_SRC,
            indexSize, true);

        vertexStager->Update(vertexSize, 0, vertices.data());
        indexStager->Update(indexSize, 0, indices.data());

        Scope<CommandBuffer> c = context.CreateCommandBuffer(true);
        c->Begin(true);
        {
            CommandBuffer::BufferCopy vertexCopy;
            vertexCopy.size = vertexSize;
            c->CmdCopyBuffer(*vertexStager, *m_vertexBuffer, 1, &vertexCopy);

            CommandBuffer::BufferCopy indexCopy;
            indexCopy.size = indexSize;
            c->CmdCopyBuffer(*indexStager, *m_indexBuffer, 1, &indexCopy);
        }
        c->End();
        c->Submit(true);
    }

    MeshData::MeshData(const size_t initialVertexCount, const size_t initialIndexCount, UpdateFrequency frequency)
        :
        m_vertexCount(initialVertexCount),
        m_indexCount(initialIndexCount)
    {
        GraphicsContext& context = Application::Get().GetGraphicsContext();

        m_vertexBuffer = context.CreateSimpleBuffer(Buffer::UsageFlags::VERTEX_BUFFER,
            initialVertexCount * sizeof(Vertex3), false);
        m_indexBuffer = context.CreateSimpleBuffer(Buffer::UsageFlags::INDEX_BUFFER,
            initialIndexCount * sizeof(uint32_t), false);
    }

    void MeshData::Update(std::span<Vertex3> vertices, std::span<uint32_t> indices)
    {

    }

    void MeshData::Resize(size_t vertexCount, size_t indexCount)
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

    Buffer& MeshData::GetVertexBuffer() const
    {
        return *m_vertexBuffer;
    }

    Buffer& MeshData::GetIndexBuffer() const
    {
        return *m_indexBuffer;
    }
}
