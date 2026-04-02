#include "API/Pipeline.hpp"

#include <VkDefs.hpp>

#include "API/DescriptorSetLayout.hpp"
#include <LE/Resources/MeshData.hpp>

namespace le::vk
{
    Pipeline::Pipeline(Tether::Rendering::Vulkan::GraphicsContext& context,
        const Info& info)
        :
        m_Context(context)
    {
        // This updates every frame so it literally doesn't matter
        VkExtent2D extent = { 1280, 720 };

        VkPushConstantRange pushConstant{};
        pushConstant.size = sizeof(ObjectTransform);
        pushConstant.offset = 0;
        pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    	std::vector<VkDescriptorSetLayout> layouts;
    	layouts.reserve(info.setLayouts.size());
    	for (le::DescriptorSetLayout* set : info.setLayouts)
    		layouts.push_back(static_cast<DescriptorSetLayout*>(set)->GetDescriptorSetLayout());

        VkPipelineLayoutCreateInfo pipelineLayoutDesc{};
		pipelineLayoutDesc.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutDesc.setLayoutCount = layouts.size();
		pipelineLayoutDesc.pSetLayouts = layouts.data();
        pipelineLayoutDesc.pushConstantRangeCount = 1;
        pipelineLayoutDesc.pPushConstantRanges = &pushConstant;

		LE_CHECK_VK(vkCreatePipelineLayout(m_Context.GetDevice(), &pipelineLayoutDesc,
			nullptr, &m_PipelineLayout));

		std::vector<VkVertexInputBindingDescription> bindingDescs;
		std::vector<VkVertexInputAttributeDescription> attribDescs;

		// Vertex3c
		{
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(MeshData::Vertex3);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			VkVertexInputAttributeDescription position;
			position.location = 0;
			position.binding = 0;
			position.format = VK_FORMAT_R32G32B32_SFLOAT;
			position.offset = offsetof(MeshData::Vertex3, position);
			VkVertexInputAttributeDescription color;
			color.location = 1;
			color.binding = 0;
			color.format = VK_FORMAT_R32G32B32_SFLOAT;
			color.offset = offsetof(MeshData::Vertex3, texcoord);

			bindingDescs.push_back(bindingDescription);
			attribDescs.push_back(position);
			attribDescs.push_back(color);
		}

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount =
			static_cast<uint32_t>(bindingDescs.size());
		vertexInputInfo.pVertexBindingDescriptions = bindingDescs.data();
		vertexInputInfo.vertexAttributeDescriptionCount =
			static_cast<uint32_t>(attribDescs.size());
		vertexInputInfo.pVertexAttributeDescriptions = attribDescs.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(extent.width);
		viewport.height = static_cast<float>(extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = extent.width;
		scissor.extent.height = extent.height;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_NONE;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampleState{};
		multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.sampleShadingEnable = VK_FALSE;
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleState.minSampleShading = 0.0f;
		multisampleState.pSampleMask = nullptr;
		multisampleState.alphaToCoverageEnable = VK_FALSE;
		multisampleState.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f;
		depthStencil.maxDepthBounds = 1.0f;
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {};
		depthStencil.back = {};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = info.dynamicStateCount;
		dynamicState.pDynamicStates = info.pDynamicStates;

		// Oh, yes, cool thing about Vulkan, you can actually have multiple shader
		// stages for one shader module. That means that you can have a VSMain and
		// a PSMain in one shader module (aka a glsl file in this case).

    	VkFormat colorFormat = VK_FORMAT_B8G8R8A8_SRGB;

    	VkPipelineRenderingCreateInfoKHR renderingCreateInfo{};
    	renderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    	renderingCreateInfo.colorAttachmentCount = 1;
    	renderingCreateInfo.pColorAttachmentFormats = &colorFormat;
    	renderingCreateInfo.depthAttachmentFormat = info.depthFormat;

		VkGraphicsPipelineCreateInfo pipelineDesc{};
		pipelineDesc.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    	pipelineDesc.pNext = &renderingCreateInfo;
		pipelineDesc.stageCount = info.stageCount;
		pipelineDesc.pStages = info.pStages;
		pipelineDesc.pVertexInputState = &vertexInputInfo;
		pipelineDesc.pInputAssemblyState = &inputAssembly;
		pipelineDesc.pViewportState = &viewportState;
		pipelineDesc.pRasterizationState = &rasterizer;
		pipelineDesc.pMultisampleState = &multisampleState;
		pipelineDesc.pColorBlendState = &colorBlending;
		pipelineDesc.layout = m_PipelineLayout;
		pipelineDesc.renderPass = VK_NULL_HANDLE;
		pipelineDesc.subpass = 0;
		pipelineDesc.basePipelineHandle = VK_NULL_HANDLE;
		pipelineDesc.pDepthStencilState = &depthStencil;

		if (info.pDynamicStates && info.dynamicStateCount > 0)
			pipelineDesc.pDynamicState = &dynamicState;

        LE_CHECK_VK(vkCreateGraphicsPipelines(m_Context.GetDevice(), VK_NULL_HANDLE,
        	1, &pipelineDesc, nullptr, &m_Pipeline));
    }

    Pipeline::~Pipeline()
    {
        vkDestroyPipeline(m_Context.GetDevice(), m_Pipeline, nullptr);
        vkDestroyPipelineLayout(m_Context.GetDevice(), m_PipelineLayout, nullptr);
    }

    VkPipeline Pipeline::Get() const
    {
        return m_Pipeline;
    }

    VkPipelineLayout Pipeline::GetPipelineLayout() const
    {
        return m_PipelineLayout;
    }
}
