#include "BufferStager.hpp"

#include "VkDefs.hpp"

namespace le::vk
{
	BufferStager::BufferStager(GraphicsContext& context)
		:
		m_GraphicsContext(context.GetTetherGraphicsContext()),
		m_TransferMutex(context.GetTransferQueueMutex()),
		m_Device(m_GraphicsContext.GetDevice()),
		m_Queue(context.GetTransferQueue()),
		m_CommandPool(context.GetTransferPool())
	{
		CreateCommandBuffer();
	}

	BufferStager::~BufferStager()
	{
		DeleteStagingBuffer();

		vkFreeCommandBuffers(m_Device, m_CommandPool, 1,
			&m_CommandBuffer);
		vkDestroyFence(m_Device, m_Fence, nullptr);
	}

	void BufferStager::CreateStagingBuffer(VkBuffer target, const size_t targetSize)
	{
		if (!m_Fence)
			CreateFence();

		Wait();
		vmaDestroyBuffer(m_GraphicsContext.GetAllocator(), m_StagingBuffer, m_StagingAllocation);

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = targetSize;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		LE_CHECK_VK(vmaCreateBuffer(m_GraphicsContext.GetAllocator(), &bufferInfo, &allocInfo, &m_StagingBuffer, &m_StagingAllocation, &m_StagingInfo));

		RecordCommandBuffer(target, targetSize);
	}

	void BufferStager::Upload(const void* data, const size_t targetSize) const
	{
		memcpy(m_StagingInfo.pMappedData, data, targetSize);

		VkCommandBufferSubmitInfo commandBufferInfo{};
		commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
		commandBufferInfo.commandBuffer = m_CommandBuffer;

		VkSubmitInfo2 submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &commandBufferInfo;

		vkResetFences(m_Device, 1, &m_Fence);
		std::scoped_lock lock(m_TransferMutex);
		vkQueueSubmit2(m_Queue, 1, &submitInfo, m_Fence);
	}

	void BufferStager::DeleteStagingBuffer()
	{
		vmaDestroyBuffer(m_GraphicsContext.GetAllocator(), m_StagingBuffer, m_StagingAllocation);
		vkDestroyFence(m_Device, m_Fence, nullptr);
		m_StagingBuffer = nullptr;
		m_StagingAllocation = nullptr;
		m_Fence = nullptr;
	}

	bool BufferStager::IsFenceSignaled() const
	{
		return vkGetFenceStatus(m_Device, m_Fence) == VK_SUCCESS;
	}

	bool BufferStager::HasStagingBuffer() const
	{
		return m_StagingBuffer != nullptr;
	}

	void BufferStager::CreateCommandBuffer()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_CommandPool;
		allocInfo.commandBufferCount = 1;

		LE_CHECK_VK(vkAllocateCommandBuffers(m_Device, &allocInfo, &m_CommandBuffer));
	}

	void BufferStager::Wait() const
	{
		vkWaitForFences(m_Device, 1, &m_Fence, true, UINT64_MAX);
	}

	void BufferStager::RecordCommandBuffer(const VkBuffer target, const size_t targetSize) const
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkResetCommandBuffer(m_CommandBuffer, 0);

		LE_CHECK_VK(vkBeginCommandBuffer(m_CommandBuffer, &beginInfo));
		{
			VkBufferCopy copyRegion{};
			copyRegion.size = targetSize;

			vkCmdCopyBuffer(m_CommandBuffer, m_StagingBuffer, target,
				1, &copyRegion);
		}
		LE_CHECK_VK(vkEndCommandBuffer(m_CommandBuffer));
	}

	void BufferStager::CreateFence()
	{
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		LE_CHECK_VK(vkCreateFence(m_Device, &fenceInfo, nullptr, &m_Fence));
	}
}