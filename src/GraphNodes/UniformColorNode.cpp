#include "UniformColorNode.h"

#include "imgui.h"
#include "../imnodes.h"

namespace Surge
{
UniformColorNode::UniformColorNode() : Node( NodeType::UNIFORM_COLOR )
{
    name = "Uniform Color";
    value = std::make_shared<Image>( 2048, 2048, ImageFormat::RGBA );
    m_color.asPart.red = 1;
    m_color.asPart.green = 1;
    m_color.asPart.blue = 1;
    m_color.asPart.alpha = 1;
    UpdateColor();
}

std::shared_ptr<Image> UniformColorNode::Evaluate(std::stack<std::shared_ptr<Image>> &value_stack)
{
    UpdateColor();
    return value;
}

bool UniformColorNode::RenderProperties()
{
    ImGui::Text( name.c_str() );
    ImGui::Separator();
    bool changed = false;
    changed |= ImGui::ColorPicker4("Color", m_color.asArray.data);
    return changed;
}

void UniformColorNode::UpdateColor()
{
    constexpr auto bufSize = static_cast<size_t>( 2048 * 2048 * 4 );
    auto *data = new char[bufSize];
    char channel[4];
    channel[0] = static_cast<char>( m_color.asPart.red*255 );
    channel[1] = static_cast<char>( m_color.asPart.green*255 );
    channel[2] = static_cast<char>( m_color.asPart.blue*255 );
    channel[3] = static_cast<char>( m_color.asPart.alpha*255 );
    for( int i = 0; i < bufSize; i+=4)
    {
        memcpy( &data[i], channel, sizeof(char)*4 );
    }
    value->SetData( data );
    delete[] data;
}

// ------ UI ------ //

void UiUniformColorNode::RenderNode( Node* node ) const
{
    constexpr float node_width = 100.0f;
    const InputHeaderStyleJanitor header;
    ImNodes::BeginNode(id);

    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(node->name.c_str());
    ImNodes::EndNodeTitleBar();

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