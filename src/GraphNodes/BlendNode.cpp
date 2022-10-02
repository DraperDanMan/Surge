#include "BlendNode.h"

#include "imgui.h"
#include "../imnodes.h"

namespace Surge
{

    BlendNode::BlendNode() : Node( NodeType::BLEND )
    {
        name = "Blend";
        if ( !blendCompute )
        {
            blendCompute = new BlendCompute();
        }
        value = std::make_shared<Image>( 2048, 2048, ImageFormat::RGBA );
        constexpr auto bufSize = static_cast<size_t>( 2048 * 2048 * 4 );
        auto *data = new char[bufSize];
        memset( data, 255,  bufSize);
        value->SetData( data );
        delete[] data;
    }

    std::shared_ptr<Image> BlendNode::Evaluate(std::stack<std::shared_ptr<Image>> &value_stack)
    {
        const std::shared_ptr<Image> rhs = value_stack.top();
        value_stack.pop();
        const std::shared_ptr<Image> lhs = value_stack.top();
        value_stack.pop();
        blendCompute->Run( lhs.get(), rhs.get(), value.get(), { m_mode, 0 } );
        return value;
    }

    bool BlendNode::RenderProperties()
    {
        ImGui::Text( name.c_str() );
        ImGui::Separator();
        bool changed = false;
        const char* items[] = { "Add", "Subtract", "Multiply", "Divide", "Screen" };
        int item = static_cast<int>( m_mode );
        if ( ImGui::Combo("Mode", &item, items, IM_ARRAYSIZE(items)) )
        {
            changed = true;
        }
        m_mode = static_cast<BlendCompute::BlendMode>( item );
        return changed;
    }

    // ------ UI ------ //

    void UiBlendNode::RenderNode( Node* node ) const
    {
        constexpr float node_width = 100.f;
        const ModifyHeaderStyleJanitor header;
        ImNodes::BeginNode(id);

        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(node->name.c_str());
        ImNodes::EndNodeTitleBar();

        {
            ImNodes::BeginInputAttribute(ui.two.lhs);
            ImGui::TextUnformatted("Top");
            ImNodes::EndInputAttribute();

            ImNodes::BeginInputAttribute(ui.two.rhs);
            ImGui::TextUnformatted("Base");
            ImNodes::EndInputAttribute();
        }

        ImGui::Spacing();

        {
            ImNodes::BeginOutputAttribute(id);
            const float label_width = ImGui::CalcTextSize("result").x;
            ImGui::Indent(node_width - label_width);
            ImGui::TextUnformatted("result");
            ImNodes::EndOutputAttribute();
        }
                
        ImGui::Image( node->value->GetDescriptorSet(), ImVec2( node_width,node_width ), ImVec2(0, 0), ImVec2(1,1), ImVec4(1,1,1,1), ImVec4(0.6f,0.6f,0.6f,1) );
                
        ImNodes::EndNode();
    }

}