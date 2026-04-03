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
        const size_t vertexSize = m_vertexCount * sizeof(Vertex3);
        const size_t indexSize = m_indexCount * sizeof(uint32_t);

        CreateBuffer(vertexSize, indexSize, frequency, context);

        if (frequency == UpdateFrequency::UPDATES_ONCE)
        {
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
        else
        {
            m_vertexBuffer->Update(vertexSize, 0, vertices.data());
            m_indexBuffer->Update(indexSize, 0, vertices.data());
        }
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

    void MeshData::Update(const std::span<Vertex3> vertices, const std::span<uint32_t> indices)
    {
        m_vertexBuffer->Update(vertices.size() * sizeof(Vertex3), 0, vertices.data());
        m_indexBuffer->Update(indices.size() * sizeof(uint32_t), 0, indices.data());

        m_vertexCount = vertices.size();
        m_indexCount = indices.size();
    }

    void MeshData::Resize(const size_t vertexCount, const size_t indexCount)
    {
        m_vertexBuffer->Resize(vertexCount * sizeof(Vertex3));
        m_indexBuffer->Resize(indexCount * sizeof(uint32_t));

        m_vertexCount = vertexCount;
        m_indexCount = indexCount;
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

    void MeshData::CreateBuffer(size_t vertexSize, size_t indexSize, UpdateFrequency frequency, GraphicsContext& context)
    {
        switch (frequency)
        {
            case UpdateFrequency::UPDATES_ONCE:
            {
                m_vertexBuffer = context.CreateSimpleBuffer(Buffer::UsageFlags::VERTEX_BUFFER | Buffer::UsageFlags::TRANSFER_DST,
                    vertexSize, false);
                m_indexBuffer = context.CreateSimpleBuffer(Buffer::UsageFlags::INDEX_BUFFER | Buffer::UsageFlags::TRANSFER_DST,
                    indexSize, false);
            }
            break;

            case UpdateFrequency::UPDATES_OCCASIONALLY:
            {
                m_vertexBuffer = context.CreateSmartBuffer(Buffer::UsageFlags::VERTEX_BUFFER);
                m_indexBuffer = context.CreateSmartBuffer(Buffer::UsageFlags::INDEX_BUFFER);
            }
            break;

            case UpdateFrequency::UPDATES_OFTEN:
            {
                m_vertexBuffer = context.CreatePerFrameBuffer(Buffer::UsageFlags::VERTEX_BUFFER,
                    vertexSize);
                m_indexBuffer = context.CreatePerFrameBuffer(Buffer::UsageFlags::INDEX_BUFFER,
                    indexSize);
            }
            break;

            default:;
        }
    }
}
