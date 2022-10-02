#pragma once

#include "../Image.h"
#include "vulkan/vulkan.h"

#include <string>

#include "ComputeBase.h"

namespace Surge
{
class NoiseCompute : ComputeBase
{
    const std::string NoiseComputeShader = "Shaders/NoiseCompute.comp";
public:

    enum class NoiseMode
    {
        RAW,
        VORONOI,
        PERLIN,
        SMOKE,
    };
    
    struct PushParams
    {
        NoiseMode noiseMode;
        uint32_t width;
        uint32_t height;
        int seed;
        float scale;
    };

    NoiseCompute();
    ~NoiseCompute();
    void Run( Image *output, PushParams params );

private:
    void Bind( Image *output, PushParams params );
    void UnBind();

    VkCommandBuffer CreateCommandBuffer( VkDevice device, VkCommandPool pool, VkPipeline pipeline, VkPipelineLayout layout, VkDescriptorSet dscSet, PushParams p );
    
    Image *m_output;
};
}