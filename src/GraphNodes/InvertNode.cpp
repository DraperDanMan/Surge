#include "InvertNode.h"

#include "imgui.h"
#include "../imnodes.h"

namespace Surge
{

    InvertNode::InvertNode() : Node( NodeType::INVERT )
    {
        name = "Invert";
        if ( !invertCompute )
        {
            invertCompute = new InvertCompute();
        }
        value = std::make_shared<Image>( 2048, 2048, ImageFormat::RGBA );
        constexpr auto bufSize = static_cast<size_t>( 2048 * 2048 * 4 );
        auto *data = new char[bufSize];
        memset( data, 255,  bufSize);
        value->SetData( data );
        delete[] data;
    }

    std::shared_ptr<Image> InvertNode::Evaluate(std::stack<std::shared_ptr<Image>> &value_stack)
    {
        const std::shared_ptr<Image> input = value_stack.top();
        value_stack.pop();
        invertCompute->Run( input.get(), value.get(), { m_channels } );
        return value;
    }

    bool InvertNode::RenderProperties()
    {
        ImGui::Text( name.c_str() );
        ImGui::Separator();
        bool changed = false;
        ImGui::Text( "Channels" );
        changed |= ImGui::CheckboxFlags( "Red", &m_channels, 1 << 0);
        changed |= ImGui::CheckboxFlags( "Green", &m_channels, 1 << 1);
        changed |= ImGui::CheckboxFlags( "Blue", &m_channels, 1 << 2);
        changed |= ImGui::CheckboxFlags( "Alpha", &m_channels, 1 << 3);

        return changed;
    }

    // ------ UI ------ //

    void UiInvertNode::RenderNode( Node* node ) const
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