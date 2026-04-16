#include <LE/Application.hpp>
#include <LE/Graphics/Explicit/ExplicitMaterial.hpp>
#include <LE/Graphics/Explicit/ExplicitMesh.hpp>
#include <LE/Graphics/Explicit/ExplicitRenderTarget.hpp>
#include <LE/Graphics/Explicit/ExplicitResources.hpp>
#include <LE/Graphics/Explicit/ExplicitScene.hpp>
#include <LE/Graphics/Explicit/ExplicitTexture2D.hpp>
#include <LE/Graphics/Explicit/ExplicitTexture2DArray.hpp>

namespace le
{
    ExplicitResources::ExplicitResources(ExplicitDriver& driver)
        :
        m_driver(driver)
    {
        m_deletionQueues.resize(Application::FRAMES_IN_FLIGHT);

        m_depthFormat = m_driver.FindDepthFormat();

        CreateQueues();
        CreateDescriptorSetLayouts();
        CreatePipelineLayout();
    }

    ExplicitResources::~ExplicitResources()
    {
        m_driver.WaitIdle();

        for (auto& queue : m_deletionQueues)
            for (auto& deletionFunc : queue)
                deletionFunc();

        m_driver.DestroyCommandPool(m_graphicsPool);
        if (m_driver.HasTransferQueue())
            m_driver.DestroyCommandPool(m_transferPool);

        m_driver.DestroyPipelineLayout(m_pipelineLayout);

        m_driver.DestroyLayoutPoolManager(m_cameraPool);
        m_driver.DestroyLayoutPoolManager(m_scenePool);
        m_driver.DestroyLayoutPoolManager(m_materialPool);
        for (const DescriptorSetLayoutID& layout : m_descriptorSetLayouts)
            m_driver.DestroyDescriptorSetLayout(layout);
    }

    void ExplicitResources::ProcessDeletionQueue()
    {
        m_currentFrame = Application::Get().GetCurrentFrame();

        for (auto& func : m_deletionQueues[m_currentFrame])
            func();

        m_deletionQueues[m_currentFrame].clear();
    }

    void ExplicitResources::EnqueueDeletionFunc(const std::function<void()>& func)
    {
        m_deletionQueues[m_currentFrame].push_back(func);
    }

    MaterialID ExplicitResources::CreateMaterial()
    {
        return MaterialID(new ExplicitMaterial(*this));
    }

    MeshID ExplicitResources::CreateMesh(const std::span<MeshData::Vertex3> vertices, const std::span<uint32_t> indices,
                                        const MeshData::UpdateFrequency frequency)
    {
        return MeshID(new ExplicitMesh(*this, vertices, indices, frequency));
    }

    MeshID ExplicitResources::CreateMesh(const size_t initialVertexCount, const size_t initialIndexCount,
                                        const MeshData::UpdateFrequency frequency)
    {
        return MeshID(new ExplicitMesh(*this, initialVertexCount, initialIndexCount, frequency));
    }

    SceneID ExplicitResources::CreateScene()
    {
        return SceneID(new ExplicitScene(*this));
    }

    ShaderID ExplicitResources::CreateShader(const sh::ShaderInfo& shaderInfo)
    {
        std::vector<VertexBinding> bindings;
        std::vector<VertexAttribute> attributes;

        // Vertex3
        {
            bindings.push_back({
                .binding = 0,
                .stride = sizeof(MeshData::Vertex3),
                .inputRate = InputRate::VERTEX
            });

            attributes.push_back({
                .location = 0,
                .binding = 0,
                .offset = offsetof(MeshData::Vertex3, position),
                .format = Format::R32G32B32_SFLOAT,
            });

            attributes.push_back({
                .location = 1,
                .binding = 0,
                .offset = offsetof(MeshData::Vertex3, texcoord),
                .format = Format::R32G32_SFLOAT,
            });
        }

        PipelineInfo info;
        info.colorAttachmentFormats = std::span(&COLOR_FORMAT, 1);
        info.depthFormat = m_depthFormat;
        info.layout = m_pipelineLayout;
        info.shaderInfo = shaderInfo;
        info.vertexBindings = bindings;
        info.vertexAttributes = attributes;

        return ShaderID(m_driver.CreatePipeline(info).id);
    }

    Texture2DID ExplicitResources::CreateTexture2D(const TextureData& loader)
    {
        return Texture2DID(new ExplicitTexture2D(*this, loader));
    }

    Texture2DArrayID ExplicitResources::CreateTexture2DArray(const size_t width, const size_t height,
                                                            const uint8_t channels, const std::span<TextureData*>& textureData)
    {
        return Texture2DArrayID(new ExplicitTexture2DArray(*this, width, height, channels, textureData));
    }

    RenderTargetID ExplicitResources::CreateRenderTarget(Window& window)
    {
        constexpr auto color = Format::B8G8R8A8_SRGB;
        return RenderTargetID(new ExplicitRenderTarget(*this, color, m_depthFormat, window));
    }

    void ExplicitResources::DestroyMaterial(MaterialID id)
    {
        EnqueueDeletionFunc([id] { delete reinterpret_cast<ExplicitMaterial*>(id.id); });
    }

    void ExplicitResources::DestroyMesh(MeshID id)
    {
        EnqueueDeletionFunc([id] { delete reinterpret_cast<ExplicitMesh*>(id.id); });
    }

    void ExplicitResources::DestroyScene(SceneID id)
    {
        EnqueueDeletionFunc([id] { delete reinterpret_cast<ExplicitScene*>(id.id); });
    }

    void ExplicitResources::DestroyShader(ShaderID id)
    {
        EnqueueDeletionFunc([this, id] { m_driver.DestroyPipeline(PipelineID(id.id)); });
    }
    void ExplicitResources::DestroyTexture2D(Texture2DID id)
    {
        EnqueueDeletionFunc([id] { delete reinterpret_cast<ExplicitTexture2D*>(id.id); });
    }
    void ExplicitResources::DestroyTexture2DArray(Texture2DArrayID id)
    {
        EnqueueDeletionFunc([id] { delete reinterpret_cast<ExplicitTexture2DArray*>(id.id); });
    }
    void ExplicitResources::DestroyRenderTarget(RenderTargetID id)
    {
        EnqueueDeletionFunc([id] { delete reinterpret_cast<ExplicitRenderTarget*>(id.id); });
    }

    void ExplicitResources::UpdateMesh(const MeshID id, const std::span<MeshData::Vertex3> vertices, const std::span<uint32_t> indices)
    {
        reinterpret_cast<ExplicitMesh*>(id.id)->Update(vertices, indices);
    }

    void ExplicitResources::ResizeMesh(const MeshID id, const size_t vertexCount, const size_t indexCount)
    {
        reinterpret_cast<ExplicitMesh*>(id.id)->Resize(vertexCount, indexCount);
    }

    void ExplicitResources::SetMaterialTexture(const MaterialID id, const Ref<Texture> texture)
    {
        reinterpret_cast<ExplicitMaterial*>(id.id)->SetTexture(texture);
    }

    void ExplicitResources::SetMaterialColor(const MaterialID id, const Color color)
    {
        reinterpret_cast<ExplicitMaterial*>(id.id)->SetColor(color);
    }

    void ExplicitResources::SetMaterialShader(const MaterialID id, const ShaderID shader)
    {
        reinterpret_cast<ExplicitMaterial*>(id.id)->SetShader(shader);
    }

    void ExplicitResources::SetSceneAmbientLight(const SceneID id, const float level)
    {
        reinterpret_cast<ExplicitScene*>(id.id)->SetAmbientLight(level);
    }

    ImageID ExplicitResources::GetTexture2DImage(const Texture2DID texture)
    {
        return reinterpret_cast<ExplicitTexture2D*>(texture.id)->GetImage();
    }

    ImageViewID ExplicitResources::GetTexture2DImageView(const Texture2DID texture)
    {
        return reinterpret_cast<ExplicitTexture2D*>(texture.id)->GetImageView();
    }

    ImageID ExplicitResources::GetTexture2DArrayImage(const Texture2DArrayID texture)
    {
        return reinterpret_cast<ExplicitTexture2DArray*>(texture.id)->GetImage();
    }

    ImageViewID ExplicitResources::GetTexture2DArrayImageView(const Texture2DArrayID texture)
    {
        return reinterpret_cast<ExplicitTexture2DArray*>(texture.id)->GetImageView();
    }

    PoolManagerID ExplicitResources::GetCameraPoolManager() const
    {
        return m_cameraPool;
    }

    PoolManagerID ExplicitResources::GetScenePoolManager() const
    {
        return m_scenePool;
    }

    PoolManagerID ExplicitResources::GetMaterialPoolManager() const
    {
        return m_materialPool;
    }

    CommandPoolID ExplicitResources::GetGraphicsPool() const
    {
        return m_graphicsPool;
    }

    CommandPoolID ExplicitResources::GetTransferPool() const
    {
        return *m_pTransferPool;
    }

    QueueID ExplicitResources::GetGraphicsQueue() const
    {
        return m_graphicsQueue;
    }

    QueueID ExplicitResources::GetTransferQueue() const
    {
        return *m_pTransferQueue;
    }

    std::mutex& ExplicitResources::GetGraphicsMutex()
    {
        return m_graphicsMutex;
    }

    std::mutex& ExplicitResources::GetTransferMutex() const
    {
        return *m_pTransferMutex;
    }

    ExplicitDriver& ExplicitResources::GetDriver() const
    {
        return m_driver;
    }

    PipelineLayoutID ExplicitResources::GetPipelineLayout() const
    {
        return m_pipelineLayout;
    }

    void ExplicitResources::CreateQueues()
    {
        m_graphicsPool = m_driver.CreateCommandPool(QueueFamily::GRAPHICS);
        m_graphicsQueue = m_driver.GetQueue(QueueFamily::GRAPHICS);
        m_pTransferPool = &m_graphicsPool;
        m_pTransferQueue = &m_graphicsQueue;
        m_pTransferMutex = &m_graphicsMutex;

        if (m_driver.HasTransferQueue())
        {
            m_transferPool = m_driver.CreateCommandPool(QueueFamily::TRANSFER);
            m_transferQueue = m_driver.GetQueue(QueueFamily::TRANSFER);
            m_pTransferPool = &m_transferPool;
            m_pTransferQueue = &m_transferQueue;
            m_pTransferMutex = &m_transferMutex;
        }
    }

    void ExplicitResources::CreateDescriptorSetLayouts()
    {
        // Camera
        {
            DescriptorSetLayoutBinding bindings[] =
            {
                DescriptorSetLayoutBinding {
                    .binding = 0,
                    .descriptorCount = 1,
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .stageFlags = ShaderStageFlagBits::VERTEX
                }
            };

            DescriptorSetLayoutID layout = m_driver.CreateDescriptorSetLayout(bindings);
            m_descriptorSetLayouts.emplace_back(layout);
            m_cameraPool = m_driver.CreateLayoutPoolManager(layout);
        }

        // Scene
        {
            DescriptorSetLayoutBinding bindings[] =
            {
                DescriptorSetLayoutBinding {
                    .binding = 0,
                    .descriptorCount = 1,
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .stageFlags = ShaderStageFlagBits::FRAGMENT
                },
                DescriptorSetLayoutBinding {
                    .binding = 1,
                    .descriptorCount = 1,
                    .descriptorType = DescriptorType::STORAGE_BUFFER,
                    .stageFlags = ShaderStageFlagBits::FRAGMENT
                }
            };

            DescriptorSetLayoutID layout = m_driver.CreateDescriptorSetLayout(bindings);
            m_descriptorSetLayouts.emplace_back(layout);
            m_scenePool = m_driver.CreateLayoutPoolManager(layout);
        }

        // Material
        {
            DescriptorSetLayoutBinding bindings[] =
            {
                DescriptorSetLayoutBinding {
                    .binding = 0,
                    .descriptorCount = 1,
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .stageFlags = ShaderStageFlagBits::FRAGMENT
                },
                DescriptorSetLayoutBinding {
                    .binding = 1,
                    .descriptorCount = 1,
                    .descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
                    .stageFlags = ShaderStageFlagBits::FRAGMENT
                }
            };

            DescriptorSetLayoutID layout = m_driver.CreateDescriptorSetLayout(bindings);
            m_descriptorSetLayouts.emplace_back(layout);
            m_materialPool = m_driver.CreateLayoutPoolManager(layout);
        }
    }

    void ExplicitResources::CreatePipelineLayout()
    {
        PushConstantRange ranges[] = {
            {
                .size = sizeof(Transform),
                .offset = 0,
                .stage = ShaderStageFlagBits::VERTEX
            }
        };

        m_pipelineLayout = m_driver.CreatePipelineLayout(ranges, m_descriptorSetLayouts);
    }
}
