#pragma once

#include <string>

#include "ComputeBase.h"
#include "../Image.h"
#include "vulkan/vulkan.h"

namespace Surge
{
class BlendCompute : ComputeBase
{
    const std::string BlendComputeShader = "Shaders/BlendCompute.comp";
public:
    
    enum class BlendMode
    {
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        SCREEN,
    };
    
    struct PushParams
    {
        BlendMode blendMode;  // Blend mode to use
        int unused; // Currently Unused.
    };

    BlendCompute();
    ~BlendCompute();
    void Run( Image *left, Image *right, Image *output, PushParams params );

private:
    void Bind( Image *left, Image *right, Image *output, PushParams params );
    void UnBind();
    
    VkCommandBuffer CreateCommandBuffer( VkDevice device, VkCommandPool pool, VkPipeline pipeline, VkPipelineLayout layout, VkDescriptorSet dscSet, PushParams p );

    Image *m_left;
    Image *m_right;
    Image *m_output;
};

}