#pragma once

#include <vector>

#include "../Image.h"
#include "vulkan/vulkan.h"

namespace Surge
{
    
class ComputeBase
{
protected:
    ~ComputeBase();
    
    VkDescriptorSetLayout CreateDescriptorSetLayout( VkDevice device, const int imageCount );
    VkDescriptorPool CreateDescriptorPool( VkDevice device, const int imageCount );
    virtual VkDescriptorSet CreateDescriptorSet( VkDevice device, VkDescriptorPool pool, VkDescriptorSetLayout layout, const std::vector<Image *> &images );
    VkPipelineLayout CreatePipelineLayout( VkDevice device, VkDescriptorSetLayout dscLayout, const std::vector<VkPushConstantRange> &pushConstantRanges );
    VkPipeline CreateComputePipeline( VkDevice device, VkShaderModule shader, VkPipelineLayout layout, VkPipelineCache cache );
    
    VkShaderModule m_shader;            ///< compute shader
    VkDescriptorSetLayout m_dscLayout;  ///< c++ definition of the shader binding interface
    VkDescriptorPool m_dscPool; ///< descriptors pool
    VkCommandPool m_cmdPool;            ///< used to allocate command buffers
    VkPipelineCache m_pipeCache;        ///< pipeline cache
    VkPipelineLayout m_pipeLayout;      ///< defines shader interface as a set of layout bindings and push constants

    VkPipeline m_pipe;                   ///< pipeline to submit compute commands
    VkCommandBuffer m_cmdBuffer; ///< commands recorded here, once command buffer is submitted to a queue those commands get executed
};
    
}