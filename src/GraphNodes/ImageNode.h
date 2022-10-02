#pragma once

#include "Node.h"

namespace Surge
{
struct ImageNode : Node
{
    ImageNode(const std::shared_ptr<Image> &image);
    

    std::shared_ptr<Image> Evaluate(std::stack<std::shared_ptr<Image>> &value_stack) override;
};

struct UiImageNode : UiNode
{
    void RenderNode(Node* node) const override;
};
}