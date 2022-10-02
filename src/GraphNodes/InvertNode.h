#pragma once

#include "Node.h"
#include "../Compute/InvertCompute.h"

namespace Surge
{
struct InvertNode : Node
{
    int m_channels = 7;

    InvertNode();

    std::shared_ptr<Image> Evaluate(std::stack<std::shared_ptr<Image>> &value_stack) override;

    bool RenderProperties() override;

private:
    inline static InvertCompute *invertCompute = nullptr;
};

struct UiInvertNode : UiNode
{
    void RenderNode(Node* node) const override;
};
    
}