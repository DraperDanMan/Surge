#include "LevelsNode.h"

#include "imgui.h"
#include "../imnodes.h"

namespace Surge
{

    LevelsNode::LevelsNode() : Node( NodeType::LEVELS )
    {
        name = "Levels";
        if ( !levelsCompute )
        {
            levelsCompute = new LevelsCompute();
        }
        value = std::make_shared<Image>( 2048, 2048, ImageFormat::RGBA );
        constexpr auto bufSize = static_cast<size_t>( 2048 * 2048 * 4 );
        auto *data = new char[bufSize];
        memset( data, 255,  bufSize);
        value->SetData( data );
        delete[] data;
    }

    std::shared_ptr<Image> LevelsNode::Evaluate(std::stack<std::shared_ptr<Image>> &value_stack)
    {
        const std::shared_ptr<Image> input = value_stack.top();
        value_stack.pop();
        levelsCompute->Run( input.get(), value.get(), { m_inputRange, m_outputRange, m_gamma, m_luminanceOnly } );
        return value;
    }

    bool LevelsNode::RenderProperties()
    {
        ImGui::Text( name.c_str() );
        ImGui::Separator();
        bool changed = false;
        bool luminanceOnly = m_luminanceOnly > 0.5f;
        changed |= ImGui::DragFloat2( "Input", &m_inputRange.x, 0.01f, 0, 1 );
        changed |= ImGui::DragFloat2( "Ouput", &m_outputRange.x, 0.01f, 0, 1 );
        changed |= ImGui::DragFloat( "Gamma", &m_gamma, 0.01f, 0.2f, 5.0f );
        changed |= ImGui::Checkbox( "Luminance Only", &luminanceOnly);

        m_luminanceOnly = luminanceOnly ? 1.f : 0.f;
        return changed;
    }

    // ------ UI ------ //

    void UiLevelsNode::RenderNode( Node* node ) const
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