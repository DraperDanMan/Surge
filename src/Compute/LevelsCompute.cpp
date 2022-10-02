#include "LevelsCompute.h"

#include "../Application.h"
#include "../VulkanUtils.h"

namespace Surge
{
constexpr uint32_t WORKGROUP_SIZE = 16;

LevelsCompute::LevelsCompute()
{
    VkDevice device = Application::GetDevice();

    vulkan::ShaderLoader loader;
    m_shader = loader.LoadShader( device, LevelsComputeShader.c_str() );

    m_dscLayout = CreateDescriptorSetLayout( device, 2 );
    m_dscPool = CreateDescriptorPool( device, 2 );
    
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


LevelsCompute::~LevelsCompute()
{
    // do nothing
}


void LevelsCompute::Run( Image *input, Image *output, PushParams params )
{
    Bind(input, output, params);
    Application::FlushComputeCommandBuffer( m_cmdBuffer );
    UnBind();
}


void LevelsCompute::Bind( Image *input, Image *output, PushParams params )
{
    VkDevice device = Application::GetDevice();

    std::vector<Image *> images;
    m_input = images.emplace_back( input );
    m_output = images.emplace_back( output );

    VkDescriptorSet set = CreateDescriptorSet( device, m_dscPool, m_dscLayout, images );
    
    m_cmdBuffer = CreateCommandBuffer( device, m_cmdPool, m_pipe, m_pipeLayout, set, params );
}


void LevelsCompute::UnBind()
{
    VkDevice device = Application::GetDevice();
    
    vkDestroyDescriptorPool( device, m_dscPool, nullptr );
    m_dscPool = CreateDescriptorPool( device, 2 );
    m_cmdBuffer = {};
}


VkCommandBuffer LevelsCompute::CreateCommandBuffer( VkDevice device, VkCommandPool pool, VkPipeline pipeline, VkPipelineLayout layout, VkDescriptorSet dscSet, PushParams p )
{
    VkCommandBuffer cmdBuffer = Application::GetComputeCommandBuffer();

    {
        //Make the barriers for the resources
        VkImageMemoryBarrier barrier[2] = {};

        barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier[0].subresourceRange.levelCount = barrier[0].subresourceRange.layerCount = 1;
        barrier[0].subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

        barrier[0].oldLayout       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier[0].newLayout       = VK_IMAGE_LAYOUT_GENERAL;
        barrier[0].srcAccessMask   = 0;
        barrier[0].dstAccessMask   = VK_ACCESS_SHADER_READ_BIT;
        barrier[0].image           = m_input->GetVkImage();

        barrier[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier[1].subresourceRange.levelCount = barrier[2].subresourceRange.layerCount = 1;
        barrier[1].subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

        barrier[1].oldLayout       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier[1].newLayout       = VK_IMAGE_LAYOUT_GENERAL;
        barrier[1].srcAccessMask   = 0;
        barrier[1].dstAccessMask   = VK_ACCESS_MEMORY_WRITE_BIT;
        barrier[1].image           = m_output->GetVkImage();

        vkCmdPipelineBarrier( cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                0, 0, nullptr, 0, nullptr,
                                2, barrier);
    }
    
    vkCmdBindPipeline( cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline );
    
    vkCmdBindDescriptorSets( cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, layout, 0, 1, &dscSet, 0, nullptr);

    vkCmdPushConstants( cmdBuffer, layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(p), &p );

    const uint32_t wgWidthSize = (m_output->GetWidth() + WORKGROUP_SIZE + 1u) / WORKGROUP_SIZE;
    const uint32_t wgHeightSize = (m_output->GetHeight() + WORKGROUP_SIZE + 1u) / WORKGROUP_SIZE;
    
    vkCmdDispatch( cmdBuffer, wgWidthSize, wgHeightSize, 1 );

    {
        //Make the barriers for the resources
        VkImageMemoryBarrier barrier[2] = {};

        barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier[0].subresourceRange.levelCount = barrier[0].subresourceRange.layerCount = 1;
        barrier[0].subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

        barrier[0].oldLayout       = VK_IMAGE_LAYOUT_GENERAL;
        barrier[0].newLayout       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier[0].srcAccessMask   = 0;
        barrier[0].dstAccessMask   = VK_ACCESS_SHADER_READ_BIT;
        barrier[0].image           = m_input->GetVkImage();

        barrier[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier[1].subresourceRange.levelCount = barrier[1].subresourceRange.layerCount = 1;
        barrier[1].subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

        barrier[1].oldLayout       = VK_IMAGE_LAYOUT_GENERAL;
        barrier[1].newLayout       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier[1].srcAccessMask   = 0;
        barrier[1].dstAccessMask   = VK_ACCESS_SHADER_READ_BIT;
        barrier[1].image           = m_output->GetVkImage();

        vkCmdPipelineBarrier( cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                0, 0, nullptr, 0, nullptr,
                                2, barrier);
    }
    
    vkEndCommandBuffer( cmdBuffer );
    
    return cmdBuffer;
}

}