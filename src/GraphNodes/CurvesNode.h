#pragma once

#include "Node.h"
#include "../Compute/CurvesCompute.h"

namespace Surge
{
struct CurvesNode : Node
{
    std::shared_ptr<Image> m_curvesLUTImage = nullptr;

    float m_red[5] = { 0.25f, 0.25f, 0.75f, 0.75f };
    float m_green[5] = { 0.25f, 0.25f, 0.75f, 0.75f };
    float m_blue[5] = { 0.25f, 0.25f, 0.75f, 0.75f };
    float m_alpha[5] = { 0.25f, 0.25f, 0.75f, 0.75f };
    
    CurvesNode();
    
    std::shared_ptr<Image> Evaluate(std::stack<std::shared_ptr<Image>> &value_stack) override;

    bool RenderProperties() override;

    void UpdateLUT();

private:
    inline static CurvesCompute *curvesCompute = nullptr;
};

struct UiCurvesNode : UiNode
{
    void RenderNode(Node* node) const override;
};
    
}