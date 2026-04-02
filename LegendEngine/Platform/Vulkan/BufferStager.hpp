#pragma once

#include "GraphicsContext.hpp"

namespace le::vk
{
    class BufferStager final
    {
    public:
        explicit BufferStager(GraphicsContext& context);
        ~BufferStager();
        LE_NO_COPY(BufferStager);

        void CreateStagingBuffer(VkBuffer target, size_t targetSize);
        void Upload(const void* data, size_t targetSize) const;
        void DeleteStagingBuffer();

        bool IsFenceSignaled() const;
        bool HasStagingBuffer() const;

        void Wait() const;
    private:
        void CreateCommandBuffer();
        void RecordCommandBuffer(VkBuffer target, size_t targetSize) const;
        void CreateFence();

        Tether::Rendering::Vulkan::GraphicsContext& m_GraphicsContext;
        std::mutex& m_TransferMutex;
        VkDevice m_Device = nullptr;
        VkQueue m_Queue = nullptr;
        VkCommandPool m_CommandPool = nullptr;
        VkCommandBuffer m_CommandBuffer = nullptr;

        VkBuffer m_StagingBuffer = nullptr;
        VmaAllocationInfo m_StagingInfo;
        VmaAllocation m_StagingAllocation = nullptr;
        VkFence m_Fence = nullptr;
    };
}