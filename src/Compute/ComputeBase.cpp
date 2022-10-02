#include "ComputeBase.h"

#include "../Application.h"
#include "../VulkanUtils.h"

namespace Surge
{
constexpr uint32_t WORKGROUP_SIZE = 16;
    
ComputeBase::~ComputeBase()
{
    VkDevice device = Application::GetDevice();
    
    vkDestroyPipeline( device, m_pipe, nullptr );
    vkDestroyPipelineLayout( device, m_pipeLayout, nullptr );
    vkDestroyPipelineCache( device, m_pipeCache, nullptr );
    vkDestroyDescriptorPool( device, m_dscPool, nullptr );
    vkDestroyDescriptorSetLayout( device, m_dscLayout, nullptr );
    vkDestroyShaderModule( device, m_shader, nullptr );
}

VkDescriptorSetLayout ComputeBase::CreateDescriptorSetLayout( VkDevice device, const int imageCount )
{
    VkDescriptorSetLayout set;

    std::vector<VkDescriptorSetLayoutBinding> bindings;
    for ( uint32_t i = 0; i < imageCount; ++i )
    {
        bindings.push_back( {i, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT} );
    }

    VkDescriptorSetLayoutCreateInfo createInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    createInfo.bindingCount = bindings.size();
    createInfo.pBindings = bindings.data();

    vkCreateDescriptorSetLayout( device, &createInfo, nullptr, &set );
    return set;
}


VkDescriptorPool ComputeBase::CreateDescriptorPool( VkDevice device, const int imageCount )
{
    VkDescriptorPool pool;
    VkDescriptorPoolSize size;
    size.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    size.descriptorCount = imageCount;
    VkDescriptorPoolCreateInfo poolCI = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
    poolCI.maxSets = 1;
    poolCI.poolSizeCount = 1;
    poolCI.pPoolSizes = &size;

    vkCreateDescriptorPool( device, &poolCI, nullptr, &pool );
    return pool;
}

VkDescriptorSet ComputeBase::CreateDescriptorSet( VkDevice device, VkDescriptorPool pool, VkDescriptorSetLayout layout, const std::vector<Image *> &images )
{
    VkDescriptorSetAllocateInfo descSetAI = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    descSetAI.descriptorPool = pool;
    descSetAI.descriptorSetCount = 1;
    descSetAI.pSetLayouts = &layout;

    VkDescriptorSet set;
    auto result = vkAllocateDescriptorSets( device, &descSetAI, &set );

    fprintf(stderr, vulkan::utils::ErrorAsString(result).c_str());
    
    std::vector<VkDescriptorImageInfo> descInfos;
    for (int i = 0; i < images.size(); ++i)
    {
        VkDescriptorImageInfo info = {};
        info.imageView = images[i]->GetVkImageView();
        info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        descInfos.push_back(info);
    }
    
    std::vector<VkWriteDescriptorSet> descWrite;

    for (int i = 0; i < descInfos.size(); ++i)
    {
        VkWriteDescriptorSet descW = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        descW.dstSet = set;
        descW.dstBinding = i;
        descW.descriptorCount = 1;
        descW.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        descW.pImageInfo = &descInfos[i];
        descWrite.push_back( descW );
    }

    vkUpdateDescriptorSets( device, descWrite.size(), descWrite.data(), 0, nullptr );

    return set;
}


VkPipelineLayout ComputeBase::CreatePipelineLayout( VkDevice device, VkDescriptorSetLayout dscLayout, const std::vector<VkPushConstantRange> &pushConstantRanges )
{
    VkPipelineLayoutCreateInfo createInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    createInfo.pushConstantRangeCount = pushConstantRanges.size();
    createInfo.pPushConstantRanges = pushConstantRanges.data();
    createInfo.pSetLayouts = &dscLayout;
    createInfo.setLayoutCount = 1;

    VkPipelineLayout layout;
    vkCreatePipelineLayout( device, &createInfo, nullptr, &layout );

    return layout;
}


VkPipeline ComputeBase::CreateComputePipeline( VkDevice device, VkShaderModule shader, VkPipelineLayout layout, VkPipelineCache cache )
{
    // specialize constants of the shader
    std::vector<VkSpecializationMapEntry> specEntries;
    specEntries.push_back( { 0, 0 , sizeof(int)} );
    specEntries.push_back( { 1, 1*sizeof(int) , sizeof(int)} );

    std::vector<int> specValues;
    specValues.push_back( WORKGROUP_SIZE );
    specValues.push_back( WORKGROUP_SIZE );

    VkSpecializationInfo specInfo = {
        static_cast<uint32_t>( specEntries.size() ),
        specEntries.data(),
        specValues.size()*sizeof(int),
        specValues.data()
    };

    VkPipelineShaderStageCreateInfo stageCI = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    stageCI.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    stageCI.module = shader;
    stageCI.pName = "main"; //entry point
    stageCI.pSpecializationInfo = &specInfo;

    VkComputePipelineCreateInfo pipelineCI = { VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
    pipelineCI.stage = stageCI;
    pipelineCI.layout = layout;

    VkPipeline pipe;
    vkCreateComputePipelines( device, cache, 1, &pipelineCI, nullptr, &pipe);

    return pipe;
}

    
}