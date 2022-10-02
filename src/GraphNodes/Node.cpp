#include "Node.h"

#include "imgui.h"
#include "../imnodes.h"

namespace Surge
{

Node::Node(const NodeType t): type(t)
{
    value = std::make_shared<Image>( 2048, 2048, ImageFormat::RGBA );
}

Node::Node(const NodeType t, const std::shared_ptr<Image> &val) : type(t)
{
    value = val;
}
    
std::shared_ptr<Image> Node::Evaluate(std::stack<std::shared_ptr<Image>> &value_stack)
{
    return nullptr;
}
    
bool Node::RenderProperties()
{
    ImGui::Text( name.c_str() );
    ImGui::Separator();
    ImGui::Text( "No Properties" );
    return false;
}

// Utils for pushing and popping node styles
InputHeaderStyleJanitor::InputHeaderStyleJanitor()
{
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(109, 11, 191, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(129, 46, 194, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(148, 84, 204, 255));
}

    
InputHeaderStyleJanitor::~InputHeaderStyleJanitor()
{
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
}

    
ModifyHeaderStyleJanitor::ModifyHeaderStyleJanitor()
{
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(65, 141, 11, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(92, 144, 46, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(98, 184, 86, 255));
}

    
ModifyHeaderStyleJanitor::~ModifyHeaderStyleJanitor()
{
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
}

    
OutputHeaderStyleJanitor::OutputHeaderStyleJanitor()
{
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(11, 109, 191, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(45, 126, 194, 255));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(81, 148, 204, 255));
}

    
OutputHeaderStyleJanitor::~OutputHeaderStyleJanitor()
{
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
}
}