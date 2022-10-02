#include "OutputNode.h"

#include "imgui.h"
#include "../imnodes.h"

namespace Surge
{

    OutputNode::OutputNode() : Node( NodeType::OUTPUT )
    {
        name = "Output";
        value = std::make_shared<Image>( 2048, 2048, ImageFormat::RGBA );
        constexpr auto bufSize = static_cast<size_t>( 2048 * 2048 * 4 );
        auto *data = new char[bufSize];
        memset( data, 255,  bufSize);
        value->SetData( data );
        delete[] data;
    }

    std::shared_ptr<Image> OutputNode::Evaluate(std::stack<std::shared_ptr<Image>> &value_stack)
    {
        return value;
    }

    // ------ UI ------ //

    void UiOutputNode::RenderNode( Node* node ) const
    {
        constexpr float node_width = 100.0f;
        const OutputHeaderStyleJanitor header;
        ImNodes::BeginNode(id);

        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(node->name.c_str());
        ImNodes::EndNodeTitleBar();

        ImGui::Dummy(ImVec2(node_width, 0.f));
        {
            ImNodes::BeginInputAttribute(ui.one.input);
            ImGui::TextUnformatted("input");
            ImNodes::EndInputAttribute();
        }
                
        ImGui::Image( node->value->GetDescriptorSet(), ImVec2( node_width,node_width ), ImVec2(0, 0), ImVec2(1,1), ImVec4(1,1,1,1), ImVec4(0.6f,0.6f,0.6f,1) );
                
        ImNodes::EndNode();
    }
}