#include "BlurNode.h"

#include "../imnodes.h"

namespace Surge
{

BlurNode::BlurNode() : Node( NodeType::BLUR )
{
    name = "Blur";
    if ( !blurCompute )
    {
        blurCompute = new BlurCompute();
    }
    value = std::make_shared<Image>( 2048, 2048, ImageFormat::RGBA );
    constexpr auto bufSize = static_cast<size_t>( 2048 * 2048 * 4 );
    auto *data = new char[bufSize];
    memset( data, 255,  bufSize);
    value->SetData( data );
    delete[] data;
}

std::shared_ptr<Image> BlurNode::Evaluate(std::stack<std::shared_ptr<Image>> &value_stack)
{
    const std::shared_ptr<Image> input = value_stack.top();
    value_stack.pop();
    blurCompute->Run( input.get(), value.get(), { m_center, DegreesToRadians( m_angle ), m_sigma, m_samples, m_useAlpha, m_blurMode } );
    return value;
}

bool BlurNode::RenderProperties()
{
    Node::RenderProperties();
    bool changed = false;
    bool useAlpha = m_useAlpha > 0.5f;
    const char* items[] = { "Gaussian", "Motion", "Radial" };
    int item = static_cast<int>( m_blurMode );
    if ( ImGui::Combo("Mode", &item, items, IM_ARRAYSIZE(items)) )
    {
        changed = true;
    }
    m_blurMode = static_cast<BlurCompute::BlurMode>( item );
            
    changed |= ImGui::DragFloat( "Strength", &m_sigma, 0.1f, 0.2f, 100.0f );
    if (m_blurMode == BlurCompute::BlurMode::GAUSSIAN)
    {
        changed |= ImGui::DragFloat( "Samples", &m_samples, 1.f, 1.f, 100.0f );
    }
    else
    {
        changed |= ImGui::DragFloat( "Angle", &m_angle, 0.1f, 0, 180 );
    }
    if (m_blurMode == BlurCompute::BlurMode::RADIAL)
    {
        changed |= ImGui::DragFloat2( "Center", &m_center.x, 1.f, 0, static_cast<float>( value->GetWidth() ) );
    }
            
    changed |= ImGui::Checkbox( "Use Alpha", &useAlpha);
    m_useAlpha = useAlpha ? 1.f : 0.f;
    
    return changed;
}

// ------ UI ------ //

void UiBlurNode::RenderNode( Node* node ) const
{
    constexpr float node_width = 100.0f;
    const ModifyHeaderStyleJanitor header;
    ImNodes::BeginNode(id);

    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(node->name.c_str());
    ImNodes::EndNodeTitleBar();

    {
        ImNodes::BeginInputAttribute(ui.one.input);
        ImGui::TextUnformatted("input");
        ImNodes::EndInputAttribute();
    }
    
    ImGui::Spacing();

    {
        ImNodes::BeginOutputAttribute(id);
        const float label_width = ImGui::CalcTextSize("output").x;
        ImGui::Indent(node_width - label_width);
        ImGui::TextUnformatted("output");
        ImNodes::EndInputAttribute();
    }
    ImGui::Image( node->value->GetDescriptorSet(), ImVec2( node_width,node_width ), ImVec2(0, 0), ImVec2(1,1), ImVec4(1,1,1,1), ImVec4(0.6f,0.6f,0.6f,1) );
        
    ImNodes::EndNode();
}
}