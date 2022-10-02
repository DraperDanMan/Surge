#pragma once

#include "Node.h"
#include "../Compute/HSLCompute.h"

namespace Surge
{
struct HSLNode : Node
{
    float m_hue = 0;
    float m_saturation = 0;
    float m_lightness = 0;

    HSLNode();

    std::shared_ptr<Image> Evaluate(std::stack<std::shared_ptr<Image>> &value_stack) override;

    bool RenderProperties() override;

private:
    inline static HSLCompute *hslCompute = nullptr;
};

struct UiHSLNode : UiNode
{
    void RenderNode(Node* node) const override;
};
    
}