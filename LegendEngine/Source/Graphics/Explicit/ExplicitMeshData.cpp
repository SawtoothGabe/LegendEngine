#include <LE/Graphics/Explicit/ExplicitMeshData.hpp>

namespace le
{
    ExplicitMeshData::ExplicitMeshData(ExplicitRenderer& renderer, const std::span<Vertex3> vertices,
        const std::span<uint32_t> indices, const UpdateFrequency frequency)
        :
        m_vertexCount(vertices.size()),
        m_indexCount(indices.size())
    {
        const size_t vertexSize = m_vertexCount * sizeof(Vertex3);
        const size_t indexSize = m_indexCount * sizeof(uint32_t);

        CreateBuffer(renderer, vertexSize, indexSize, frequency);

        if (frequency == UpdateFrequency::UPDATES_ONCE)
        {
            ExplicitDriver& driver = renderer.GetDriver();

            const std::vector<CommandBufferID> buffers =
                driver.AllocateCommandBuffers(renderer.GetTransferPool(), 1);
            if (buffers.empty())
                return;

            const CommandBufferID c = buffers.front();

            const BufferID vertexStager = driver.CreateBuffer(BufferUsageFlags::TRANSFER_SRC,
                vertexSize, true);
            const BufferID indexStager = driver.CreateBuffer(BufferUsageFlags::TRANSFER_SRC,
                indexSize, true);

            memcpy(driver.GetMappedBufferData(vertexStager), vertices.data(), vertexSize);
            memcpy(driver.GetMappedBufferData(indexStager), indices.data(), indexSize);

            driver.BeginCommandBuffer(c, true);
            {
                BufferCopy vertexCopy;
                vertexCopy.size = vertexSize;
                driver.CmdCopyBuffer(c, vertexStager, m_vertexBuffer->GetBuffer(),
                    std::span(&vertexCopy, 1));

                BufferCopy indexCopy;
                indexCopy.size = indexSize;
                driver.CmdCopyBuffer(c, indexStager, m_indexBuffer->GetBuffer(),
                    std::span(&indexCopy, 1));
            }
            driver.EndCommandBuffer(c);

            {
                std::scoped_lock lock(renderer.GetTransferMutex());
                driver.QueueSubmit(renderer.GetTransferQueue(), { .commandBuffer = c });
            }
            
            driver.DestroyBuffer(vertexStager);
            driver.DestroyBuffer(indexStager);
        }
        else
        {
            m_vertexBuffer->Update(vertexSize, 0, vertices.data());
            m_indexBuffer->Update(indexSize, 0, vertices.data());
        }
    }

    ExplicitMeshData::ExplicitMeshData(ExplicitRenderer& renderer, const size_t initialVertexCount,
        const size_t initialIndexCount, const UpdateFrequency frequency)
        :
        m_vertexCount(initialVertexCount),
        m_indexCount(initialIndexCount)
    {
        const size_t vertexSize = m_vertexCount * sizeof(Vertex3);
        const size_t indexSize = m_indexCount * sizeof(uint32_t);

        LE_ASSERT(frequency != UpdateFrequency::UPDATES_ONCE, "ExplicitMeshData made with UpdateFrequency::UPDATES_ONCE must specify vertex data");

        CreateBuffer(renderer, vertexSize, indexSize, frequency);
    }

    void ExplicitMeshData::Update(const std::span<Vertex3> vertices, const std::span<uint32_t> indices)
    {
        m_vertexBuffer->Update(vertices.size() * sizeof(Vertex3), 0, vertices.data());
        m_indexBuffer->Update(indices.size() * sizeof(uint32_t), 0, indices.data());

        m_vertexCount = vertices.size();
        m_indexCount = indices.size();
    }

    void ExplicitMeshData::Resize(const size_t vertexCount, const size_t indexCount)
    {
        m_vertexBuffer->Resize(vertexCount * sizeof(Vertex3));
        m_indexBuffer->Resize(indexCount * sizeof(uint32_t));

        m_vertexCount = vertexCount;
        m_indexCount = indexCount;
    }

    size_t ExplicitMeshData::GetVertexCount() const
    {
        return m_vertexCount;
    }

    size_t ExplicitMeshData::GetIndexCount() const
    {
        return m_indexCount;
    }

    Buffer& ExplicitMeshData::GetVertexBuffer() const
    {
        return *m_vertexBuffer;
    }

    Buffer& ExplicitMeshData::GetIndexBuffer() const
    {
        return *m_indexBuffer;
    }

    void ExplicitMeshData::CreateBuffer(ExplicitRenderer& renderer, size_t vertexSize, size_t indexSize,
        const UpdateFrequency frequency)
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