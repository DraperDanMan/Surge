#include "NoiseCompute.h"

#include "../Application.h"
#include "../VulkanUtils.h"

namespace Surge
{
constexpr uint32_t WORKGROUP_SIZE = 16;

NoiseCompute::NoiseCompute()
{
    VkDevice device = Application::GetDevice();

    vulkan::ShaderLoader loader;
    m_shader = loader.LoadShader( device, NoiseComputeShader.c_str() );

    m_dscLayout = CreateDescriptorSetLayout( device, 1 );
    m_dscPool = CreateDescriptorPool( device, 1 );
    
    VkPipelineCacheCreateInfo pipeCacheCI = { VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO };
    vkCreatePipelineCache( device, &pipeCacheCI, nullptr, &m_pipeCache );

    std::vector<VkPushConstantRange> pcRanges;
    VkPushConstantRange pcRange = {};
    pcRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pcRange.offset = 0;
    pcRange.size = sizeof(PushParams);
    pcRanges.push_back( pcRange );
    
    m_pipeLayout = CreatePipelineLayout( device, m_dscLayout, pcRanges );

    m_pipe = CreateComputePipeline( device, m_shader, m_pipeLayout, m_pipeCache );
    m_cmdBuffer = {};
}


NoiseCompute::~NoiseCompute()
{
    //do nothing for now
}


void NoiseCompute::Run( Image *output, PushParams params )
{
    Bind(output, params);
    Application::FlushComputeCommandBuffer( m_cmdBuffer );
    UnBind();
}


void NoiseCompute::Bind( Image *output, PushParams params )
{
    VkDevice device = Application::GetDevice();

    std::vector<Image *> images;
    m_output = images.emplace_back( output );;
    
    VkDescriptorSet set = CreateDescriptorSet( device, m_dscPool, m_dscLayout, images );
    
    m_cmdBuffer = CreateCommandBuffer( device, m_cmdPool, m_pipe, m_pipeLayout, set, params );
}


void NoiseCompute::UnBind()
{
    VkDevice device = Application::GetDevice();
    
    vkDestroyDescriptorPool( device, m_dscPool, nullptr );
    m_dscPool = CreateDescriptorPool( device, 1);
    m_cmdBuffer = {};
}


VkCommandBuffer NoiseCompute::CreateCommandBuffer( VkDevice device, VkCommandPool pool, VkPipeline pipeline, VkPipelineLayout layout, VkDescriptorSet dscSet, PushParams p )
{
    VkCommandBuffer cmdBuffer = Application::GetComputeCommandBuffer();

    {
        //Make the barriers for the resources
        VkImageMemoryBarrier barrier[1] = {};

        barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier[0].subresourceRange.levelCount = barrier[0].subresourceRange.layerCount = 1;
        barrier[0].subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

        barrier[0].oldLayout       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier[0].newLayout       = VK_IMAGE_LAYOUT_GENERAL;
        barrier[0].srcAccessMask   = 0;
        barrier[0].dstAccessMask   = VK_ACCESS_MEMORY_WRITE_BIT;
        barrier[0].image           = m_output->GetVkImage();
        

        vkCmdPipelineBarrier( cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                0, 0, nullptr, 0, nullptr,
                                1, barrier);
    }
    
    vkCmdBindPipeline( cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline );
    
    vkCmdBindDescriptorSets( cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, layout, 0, 1, &dscSet, 0, nullptr);

    vkCmdPushConstants( cmdBuffer, layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(p), &p );

    const uint32_t wgWidthSize = (m_output->GetWidth() + WORKGROUP_SIZE + 1u) / WORKGROUP_SIZE;
    const uint32_t wgHeightSize = (m_output->GetHeight() + WORKGROUP_SIZE + 1u) / WORKGROUP_SIZE;
    
    vkCmdDispatch( cmdBuffer, wgWidthSize, wgHeightSize, 1 );

    {
        //Make the barriers for the resources
        VkImageMemoryBarrier barrier[1] = {};

        barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier[0].subresourceRange.levelCount = barrier[0].subresourceRange.layerCount = 1;
        barrier[0].subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

        barrier[0].oldLayout       = VK_IMAGE_LAYOUT_GENERAL;
        barrier[0].newLayout       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier[0].srcAccessMask   = 0;
        barrier[0].dstAccessMask   = VK_ACCESS_SHADER_READ_BIT;
        barrier[0].image           = m_output->GetVkImage();

        vkCmdPipelineBarrier( cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                0, 0, nullptr, 0, nullptr,
                                1, barrier);
    }
    
    vkEndCommandBuffer( cmdBuffer );
    
    return cmdBuffer;
}

}