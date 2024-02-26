#include "pipeline.hpp"

namespace vkexperiment::vk {

CPipeline::CPipeline(CShader &&Vertex, CShader &&Fragment, const CDevice &Device, const CSwapChain &SwapChain, const CRenderPass &RenderPass) : m_Device(Device), m_VertexShader(Vertex), m_FragmentShader(Fragment)
{
	VkPipelineShaderStageCreateInfo VertexShaderStageCreateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_VERTEX_BIT,
		.module = Vertex.Module(),
		.pName = "main",
	};

	VkPipelineShaderStageCreateInfo FragmentShaderStageCreateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = Fragment.Module(),
		.pName = "main",
	};	
	std::array Stages = {std::move(VertexShaderStageCreateInfo), std::move(FragmentShaderStageCreateInfo)};
	
	std::array DynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
	VkPipelineDynamicStateCreateInfo DynamicStateCreateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = DynamicStates.size(),
		.pDynamicStates = DynamicStates.data(),
	};

	VkPipelineVertexInputStateCreateInfo VertexInputCreateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 0,
		.pVertexBindingDescriptions = nullptr,
		.vertexAttributeDescriptionCount = 0,
		.pVertexAttributeDescriptions = nullptr,
	};

	VkPipelineInputAssemblyStateCreateInfo InputAssemblyCreateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE,
	};

	VkViewport Viewport{
		.x = 0.0f,
		.y = 0.0f,
		.width = static_cast<float>(SwapChain.Extent().width),
		.height = static_cast<float>(SwapChain.Extent().height),
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};

	VkRect2D Scissor{
		.offset = {0, 0},
		.extent = SwapChain.Extent(),
	};

	VkPipelineViewportStateCreateInfo ViewportCreateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.pViewports = &Viewport,
			.scissorCount = 1,
			.pScissors = &Scissor,
	};

	VkPipelineRasterizationStateCreateInfo RasterizationCreateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.depthBiasConstantFactor = 0.0f,
		.depthBiasClamp = 0.0f,
		.depthBiasSlopeFactor = 0.0f,
		.lineWidth = 1.0f,
	};

	VkPipelineMultisampleStateCreateInfo MultisampleCreateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE,
		.minSampleShading = 1.0f,
		.pSampleMask = nullptr,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE,
	};

	VkPipelineColorBlendAttachmentState ColorBlendAttachment{
		.blendEnable = VK_FALSE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
		.colorBlendOp = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp = VK_BLEND_OP_ADD,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
	};

	VkPipelineColorBlendStateCreateInfo ColorBlendCreateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY,
		.attachmentCount = 1,
		.pAttachments = &ColorBlendAttachment,
		.blendConstants = {0.0f, 0.0f, 0.0f, 0.0f},
	};

	VkPipelineLayoutCreateInfo LayoutCreateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 0,
		.pSetLayouts = nullptr,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = nullptr,
	};

	if(vkCreatePipelineLayout(Device.Device(), &LayoutCreateInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		throw std::runtime_error{"Couldn't create pipeline layout"};
	
	VkGraphicsPipelineCreateInfo CreateInfo{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = Stages.size(),
		.pStages = Stages.data(),
		.pVertexInputState = &VertexInputCreateInfo,
		.pInputAssemblyState = &InputAssemblyCreateInfo,
		.pViewportState = &ViewportCreateInfo,
		.pRasterizationState = &RasterizationCreateInfo,
		.pMultisampleState = &MultisampleCreateInfo,
		.pDepthStencilState = nullptr,
		.pColorBlendState = &ColorBlendCreateInfo,
		.pDynamicState = &DynamicStateCreateInfo,
		.layout = m_PipelineLayout,
		.renderPass = RenderPass.RenderPass(),
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1,
	};

	if(vkCreateGraphicsPipelines(Device.Device(), VK_NULL_HANDLE, 1, &CreateInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
		throw std::runtime_error{"Couldn't create graphics pipeline"};
}

CPipeline::~CPipeline()
{
	vkDestroyPipeline(m_Device.Device(), m_Pipeline, nullptr);
	vkDestroyPipelineLayout(m_Device.Device(), m_PipelineLayout, nullptr);
}

}
