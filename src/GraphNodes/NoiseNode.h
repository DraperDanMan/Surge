#pragma once

#include "Node.h"
#include "../Compute/NoiseCompute.h"

namespace Surge
{
struct NoiseNode : Node
{
    NoiseCompute::NoiseMode m_mode = NoiseCompute::NoiseMode::RAW;
    int m_seed = 0;
    float m_scale = 1;

    NoiseNode();

    std::shared_ptr<Image> Evaluate(std::stack<std::shared_ptr<Image>> &value_stack) override;

    bool RenderProperties() override;

private:
    inline static NoiseCompute *noiseCompute = nullptr;
};

struct UiNoiseNode : UiNode
{
    void RenderNode(Node* node) const override;
};
    
}