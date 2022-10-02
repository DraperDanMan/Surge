#pragma once

#include "Node.h"
#include "../Compute/BlendCompute.h"
namespace Surge
{
struct BlendNode : Node
{
    BlendCompute::BlendMode m_mode = BlendCompute::BlendMode::ADD;

    BlendNode();

    std::shared_ptr<Image> Evaluate(std::stack<std::shared_ptr<Image>> &value_stack) override;

    bool RenderProperties() override;

private:
    inline static BlendCompute *blendCompute = nullptr;
};

struct UiBlendNode : UiNode
{
    void RenderNode(Node* node) const override;
};
    
}