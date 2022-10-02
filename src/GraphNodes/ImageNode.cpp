#include "ImageNode.h"

#include "../imnodes.h"

namespace Surge
{

ImageNode::ImageNode(const std::shared_ptr<Image> &image) : Node( NodeType::IMAGE )
{
    name = "Image";
    value = image;
}

std::shared_ptr<Image> ImageNode::Evaluate(std::stack<std::shared_ptr<Image>> &value_stack)
{
    return value;
}

    // ------ UI ------ //

void UiImageNode::RenderNode( Node* node ) const
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