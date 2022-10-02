#pragma once

#include "../Image.h"
#include "vulkan/vulkan.h"

#include <string>

#include "ComputeBase.h"

namespace Surge
{
class HSLCompute : ComputeBase
{
    const std::string HSLComputeShader = "Shaders/HSLCompute.comp";
public:
    
    struct PushParams
    {
        float hue;
        float saturation;
        float lightness;
    };

    HSLCompute();
    ~HSLCompute();
    void Run( Image *input, Image *output, PushParams params );

private:
    void Bind( Image *input, Image *output, PushParams params );
    void UnBind();

    VkCommandBuffer CreateCommandBuffer( VkDevice device, VkCommandPool pool, VkPipeline pipeline, VkPipelineLayout layout, VkDescriptorSet dscSet, PushParams p );
    
    Image *m_input;
    Image *m_output;
};
}