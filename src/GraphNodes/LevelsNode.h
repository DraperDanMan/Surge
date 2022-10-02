#pragma once

#include "Node.h"
#include "../Compute/LevelsCompute.h"

namespace Surge
{
struct LevelsNode : Node
{
    ImVec2 m_inputRange = ImVec2(0,1);
    ImVec2 m_outputRange = ImVec2(0,1);
    float m_gamma = 1; // 0.2f -> 5.0f roughly
    float m_luminanceOnly = 0; //should be 0 No or 1 Yes

    LevelsNode();

    std::shared_ptr<Image> Evaluate(std::stack<std::shared_ptr<Image>> &value_stack) override;

    bool RenderProperties() override;

private:
    inline static LevelsCompute *levelsCompute = nullptr;
};

struct UiLevelsNode : UiNode
{
    void RenderNode(Node* node) const override;
};
    
}