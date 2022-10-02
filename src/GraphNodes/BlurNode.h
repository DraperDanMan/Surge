#pragma once

#include "Node.h"
#include "../Compute/BlurCompute.h"

namespace Surge
{
    
struct BlurNode : Node
{
    BlurCompute::BlurMode m_blurMode = BlurCompute::BlurMode::MOTION;
    ImVec2 m_center = ImVec2(0,0);
    float m_angle = 0;
    float m_sigma = 0.3f;
    float m_samples = 10.0f;
    float m_useAlpha = 0;

    BlurNode();
    
    std::shared_ptr<Image> Evaluate(std::stack<std::shared_ptr<Image>> &value_stack) override;

    bool RenderProperties() override;

private:
    inline static BlurCompute *blurCompute = nullptr;
    const float PI = 3.141592653589793f;

    [[nodiscard]] float DegreesToRadians( float degrees ) const { return degrees * ( PI / 180 ); }
};

struct UiBlurNode : UiNode
{
    void RenderNode(Node* node) const override;
};
    
}