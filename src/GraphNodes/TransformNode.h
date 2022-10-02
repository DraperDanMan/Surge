#pragma once

#include "Node.h"
#include "../Compute/TransformCompute.h"

namespace Surge
{
struct TransformNode : Node
{
    bool m_flipH = false;
    bool m_flipV = false;
    float m_rotation = 0;

    TransformNode();

    std::shared_ptr<Image> Evaluate(std::stack<std::shared_ptr<Image>> &value_stack) override;

    bool RenderProperties() override;

private:
    inline static TransformCompute *transformCompute = nullptr;
};

struct UiTransformNode : UiNode
{
    void RenderNode(Node* node) const override;
};
    
}