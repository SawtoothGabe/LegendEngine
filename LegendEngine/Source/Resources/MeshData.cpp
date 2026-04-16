#include <LE/Application.hpp>
#include <LE/Resources/MeshData.hpp>

namespace le
{
    MeshData::MeshData(GraphicsResources& resources, const MeshID& handle, Passkey)
        :
        m_resources(resources),
        m_handle(handle)
    {}

    MeshData::~MeshData()
    {
        m_resources.DestroyMesh(m_handle);
    }

    void MeshData::Update(std::span<Vertex3> vertices, std::span<uint32_t> indices)
    {
        m_resources.UpdateMesh(m_handle, vertices, indices);
    }

    void MeshData::Resize(const size_t vertexCount, const size_t indexCount) const
    {
        m_resources.ResizeMesh(m_handle, vertexCount, indexCount);
    }

    MeshID MeshData::GetHandle() const
    {
        return m_handle;
    }

    Ref<MeshData> MeshData::Create(std::span<Vertex3> vertices, std::span<uint32_t> indices, UpdateFrequency frequency)
    {
        GraphicsResources& resources = Application::Get().GetGraphicsContext().GetResources();
        return std::make_shared<MeshData>(resources, resources.CreateMesh(vertices, indices, frequency), Passkey{});
    }

    Ref<MeshData> MeshData::Create(const size_t initialVertexCount, const size_t initialIndexCount,
        UpdateFrequency frequency)
    {
        GraphicsResources& resources = Application::Get().GetGraphicsContext().GetResources();
        return std::make_shared<MeshData>(resources, resources.CreateMesh(initialVertexCount,
            initialIndexCount, frequency), Passkey{});
    }
}
