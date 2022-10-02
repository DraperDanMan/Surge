#include "TransformNode.h"

#include "imgui.h"
#include "../imnodes.h"

namespace Surge
{

    TransformNode::TransformNode() : Node( NodeType::TRANSFORM )
    {
        name = "Transform";
        if ( !transformCompute )
        {
            transformCompute = new TransformCompute();
        }
        value = std::make_shared<Image>( 2048, 2048, ImageFormat::RGBA );
        constexpr auto bufSize = static_cast<size_t>( 2048 * 2048 * 4 );
        auto *data = new char[bufSize];
        memset( data, 255,  bufSize);
        value->SetData( data );
        delete[] data;
    }

    std::shared_ptr<Image> TransformNode::Evaluate(std::stack<std::shared_ptr<Image>> &value_stack)
    {
        const std::shared_ptr<Image> input = value_stack.top();
        value_stack.pop();
        ImVec2 scale = ImVec2(m_flipH ? -1.f : 1.f, m_flipV ? -1.f : 1.f);
        ImVec2 size = ImVec2( static_cast<float>(input->GetWidth()),static_cast<float>(input->GetHeight()));
        transformCompute->Run( input.get(), value.get(), { scale, size, m_rotation } );
        return value;
    }

    bool TransformNode::RenderProperties()
    {
        ImGui::Text( name.c_str() );
        ImGui::Separator();
        bool changed = false;
        ImGui::Text( "Flip" );
        changed |= ImGui::Checkbox( "Horizontal", &m_flipH);
        changed |= ImGui::Checkbox( "Vertical", &m_flipV);
        ImGui::Spacing();
        changed |= ImGui::DragFloat( "Rotation", &m_rotation, 0.2f, 0.f, 360.f);

        return changed;
    }

    // ------ UI ------ //

    void UiTransformNode::RenderNode( Node* node ) const
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