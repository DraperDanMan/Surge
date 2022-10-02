#pragma once

#include "Node.h"
#include "../Compute/LevelsCompute.h"

namespace Surge
{
    struct OutputNode : Node
    {
        OutputNode();

        std::shared_ptr<Image> Evaluate(std::stack<std::shared_ptr<Image>> &value_stack) override;
    };

    struct UiOutputNode : UiNode
    {
        void RenderNode(Node* node) const override;
    };
    
}