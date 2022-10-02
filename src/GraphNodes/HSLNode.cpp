#include "HSLNode.h"

#include "imgui.h"
#include "../imnodes.h"

namespace Surge
{

    HSLNode::HSLNode() : Node( NodeType::HSL )
    {
        name = "HSL";
        if ( !hslCompute )
        {
            hslCompute = new HSLCompute();
        }
        value = std::make_shared<Image>( 2048, 2048, ImageFormat::RGBA );
        constexpr auto bufSize = static_cast<size_t>( 2048 * 2048 * 4 );
        auto *data = new char[bufSize];
        memset( data, 255,  bufSize);
        value->SetData( data );
        delete[] data;
    }

    std::shared_ptr<Image> HSLNode::Evaluate(std::stack<std::shared_ptr<Image>> &value_stack)
    {
        const std::shared_ptr<Image> input = value_stack.top();
        value_stack.pop();
        hslCompute->Run( input.get(), value.get(), { m_hue, m_saturation, m_lightness } );
        return value;
    }

    bool HSLNode::RenderProperties()
    {
        ImGui::Text( name.c_str() );
        ImGui::Separator();
        bool changed = false;
        changed |= ImGui::DragFloat( "Hue", &m_hue, 0.01f, -1, 1 );
        changed |= ImGui::DragFloat( "Saturation", &m_saturation, 0.01f, -1, 1 );
        changed |= ImGui::DragFloat( "Lightness", &m_lightness, 0.01f, -1, 1 );
        return changed;
    }

    // ------ UI ------ //

    void UiHSLNode::RenderNode( Node* node ) const
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