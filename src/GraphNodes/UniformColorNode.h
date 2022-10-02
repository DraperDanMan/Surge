#pragma once

#include "Node.h"

namespace Surge
{
struct UniformColorNode : Node
{
    union
    {
        struct
        {
            float red;
            float green;
            float blue;
            float alpha;
        } asPart;

        struct
        {
            float data[4];
        } asArray;
        
    } m_color;
    

    UniformColorNode();

    std::shared_ptr<Image> Evaluate(std::stack<std::shared_ptr<Image>> &value_stack) override;

    bool RenderProperties() override;

    void UpdateColor();
};

struct UiUniformColorNode : UiNode
{
    void RenderNode(Node* node) const override;
};
    
}