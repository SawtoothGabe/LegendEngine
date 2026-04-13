#include <LE/Graphics/Explicit/ExplicitMesh.hpp>
#include <LE/Graphics/Explicit/PerFrameBuffer.hpp>
#include <LE/Graphics/Explicit/SimpleBuffer.hpp>
#include <LE/Graphics/Explicit/SmartBuffer.hpp>

namespace le
{
    ExplicitMesh::ExplicitMesh(ExplicitRenderer& renderer, const std::span<MeshData::Vertex3> vertices,
        const std::span<uint32_t> indices, const UpdateFrequency frequency)
        :
        m_vertexCount(vertices.size()),
        m_indexCount(indices.size())
    {
        const size_t vertexSize = m_vertexCount * sizeof(MeshData::Vertex3);
        const size_t indexSize = m_indexCount * sizeof(uint32_t);

        CreateBuffer(renderer, vertexSize, indexSize, frequency);

        if (frequency == UpdateFrequency::UPDATES_ONCE)
        {
            ExplicitDriver& driver = renderer.GetDriver();

            CommandBufferID c = driver.AllocateCommandBuffer(renderer.GetTransferPool());

            const BufferID vertexStager = driver.CreateBuffer(BufferUsageFlagBits::TRANSFER_SRC,
                vertexSize, true);
            const BufferID indexStager = driver.CreateBuffer(BufferUsageFlagBits::TRANSFER_SRC,
                indexSize, true);

            memcpy(driver.GetMappedBufferData(vertexStager), vertices.data(), vertexSize);
            memcpy(driver.GetMappedBufferData(indexStager), indices.data(), indexSize);

            driver.BeginCommandBuffer(c, true);
            {
                BufferCopy vertexCopy;
                vertexCopy.size = vertexSize;
                driver.CmdCopyBuffer(c, vertexStager, m_vertexBuffer->GetDesc().buffer,
                    std::span(&vertexCopy, 1));

                BufferCopy indexCopy;
                indexCopy.size = indexSize;
                driver.CmdCopyBuffer(c, indexStager, m_indexBuffer->GetDesc().buffer,
                    std::span(&indexCopy, 1));
            }
            driver.EndCommandBuffer(c);

            FenceID fence = driver.CreateFence(true);

            SubmitInfo info;
            info.commandBuffer = c;
            info.fence = fence;

            {
                std::scoped_lock lock(renderer.GetTransferMutex());
                driver.QueueSubmit(renderer.GetTransferQueue(), info);
            }

            driver.WaitForFences(1, &fence);

            driver.DestroyBuffer(vertexStager);
            driver.DestroyBuffer(indexStager);

            driver.FreeCommandBuffers(renderer.GetTransferPool(), 1, &c);
            driver.DestroyFence(fence);
        }
        else
        {
            m_vertexBuffer->Update(vertexSize, 0, vertices.data());
            m_indexBuffer->Update(indexSize, 0, vertices.data());
        }
    }

    ExplicitMesh::ExplicitMesh(ExplicitRenderer& renderer, const size_t initialVertexCount,
        const size_t initialIndexCount, const UpdateFrequency frequency)
        :
        m_vertexCount(initialVertexCount),
        m_indexCount(initialIndexCount)
    {
        const size_t vertexSize = m_vertexCount * sizeof(MeshData::Vertex3);
        const size_t indexSize = m_indexCount * sizeof(uint32_t);

        LE_ASSERT(frequency != UpdateFrequency::UPDATES_ONCE, "ExplicitMeshData made with UpdateFrequency::UPDATES_ONCE must specify vertex data");

        CreateBuffer(renderer, vertexSize, indexSize, frequency);
    }

    void ExplicitMesh::Update(const std::span<MeshData::Vertex3> vertices, const std::span<uint32_t> indices)
    {
        m_vertexBuffer->Update(vertices.size() * sizeof(MeshData::Vertex3), 0, vertices.data());
        m_indexBuffer->Update(indices.size() * sizeof(uint32_t), 0, indices.data());

        m_vertexCount = vertices.size();
        m_indexCount = indices.size();
    }

    void ExplicitMesh::Resize(const size_t vertexCount, const size_t indexCount)
    {
        m_vertexBuffer->Resize(vertexCount * sizeof(MeshData::Vertex3));
        m_indexBuffer->Resize(indexCount * sizeof(uint32_t));

        m_vertexCount = vertexCount;
        m_indexCount = indexCount;
    }

    size_t ExplicitMesh::GetVertexCount() const
    {
        return m_vertexCount;
    }

    size_t ExplicitMesh::GetIndexCount() const
    {
        return m_indexCount;
    }

    Buffer& ExplicitMesh::GetVertexBuffer() const
    {
        return *m_vertexBuffer;
    }

    Buffer& ExplicitMesh::GetIndexBuffer() const
    {
        return *m_indexBuffer;
    }

    void ExplicitMesh::CreateBuffer(ExplicitRenderer& renderer, size_t vertexSize, size_t indexSize,
        const UpdateFrequency frequency)
    {
        switch (frequency)
        {
            case UpdateFrequency::UPDATES_ONCE:
            {
                m_vertexBuffer = std::make_unique<SimpleBuffer>(renderer,
                    BufferUsageFlagBits::VERTEX_BUFFER | BufferUsageFlagBits::TRANSFER_DST, vertexSize);
                m_indexBuffer = std::make_unique<SimpleBuffer>(renderer,
                    BufferUsageFlagBits::INDEX_BUFFER | BufferUsageFlagBits::TRANSFER_DST, vertexSize);
            }
            break;

            case UpdateFrequency::UPDATES_OCCASIONALLY:
            {
                m_vertexBuffer = std::make_unique<SmartBuffer>(renderer, BufferUsageFlagBits::VERTEX_BUFFER);
                m_indexBuffer = std::make_unique<SmartBuffer>(renderer, BufferUsageFlagBits::INDEX_BUFFER);
            }
            break;

            case UpdateFrequency::UPDATES_OFTEN:
            {
                m_vertexBuffer = std::make_unique<PerFrameBuffer>(renderer, BufferUsageFlagBits::VERTEX_BUFFER,
                    vertexSize);
                m_indexBuffer = std::make_unique<PerFrameBuffer>(renderer, BufferUsageFlagBits::INDEX_BUFFER,
                    indexSize);
            }
            break;

            default:;
        }
    }
}
