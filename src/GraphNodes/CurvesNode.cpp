#include "CurvesNode.h"

#include "imgui.h"
#include "../imnodes.h"
#include "../ImWidgets/ImBezier.h"

namespace Surge
{

    CurvesNode::CurvesNode() : Node( NodeType::CURVES )
    {
        name = "Curves";
        if ( !curvesCompute )
        {
            curvesCompute = new CurvesCompute();
        }
        value = std::make_shared<Image>( 2048, 2048, ImageFormat::RGBA );
        constexpr auto bufSize = static_cast<size_t>( 2048 * 2048 * 4 );
        auto *data = new char[bufSize];
        memset( data, 255,  bufSize);
        value->SetData( data );
        delete[] data;

        m_curvesLUTImage = std::make_shared<Image>( 255, 1, ImageFormat::RGBA );
        UpdateLUT();
    }

    std::shared_ptr<Image> CurvesNode::Evaluate(std::stack<std::shared_ptr<Image>> &value_stack)
    {
        const std::shared_ptr<Image> input = value_stack.top();
        value_stack.pop();
        UpdateLUT();
        curvesCompute->Run( input.get(), m_curvesLUTImage.get(), value.get(), { static_cast<int>( m_curvesLUTImage->GetWidth() ) } );
        return value;
    }

    bool CurvesNode::RenderProperties()
    {
        ImGui::Text( name.c_str() );
        ImGui::Separator();
        bool changed = false;
        changed |= ImGui::Bezier( "Red", m_red );
        changed |= ImGui::Bezier( "Green", m_green );
        changed |= ImGui::Bezier( "Blue", m_blue );
        return changed;
    }

    void CurvesNode::UpdateLUT()
    {
        constexpr auto bufSize = static_cast<size_t>( 255 * 1 * 4 );
        auto *data = new char[bufSize];
        char channel[4];
        
        for( int i = 0; i < bufSize; i+=4)
        {
            const float val = static_cast<float>( i/4 ) / 255.f;

            const float redCurveVal = ImGui::BezierValue( val, m_red );
            const int redValue = static_cast<int>( redCurveVal*255 );

            const float greenCurveVal = ImGui::BezierValue( val, m_green );
            const int greenValue = static_cast<int>( greenCurveVal*255 );

            const float blueCurveVal = ImGui::BezierValue( val, m_blue );
            const int blueValue = static_cast<int>( blueCurveVal*255 );

            const float alphaCurveVal = ImGui::BezierValue( val, m_alpha );
            const int alphaValue = static_cast<int>( alphaCurveVal*255 );
            
            channel[0] = static_cast<char>( redValue );
            channel[1] = static_cast<char>( greenValue );
            channel[2] = static_cast<char>( blueValue );
            channel[3] = static_cast<char>( alphaValue );
            memcpy( &data[i], channel, sizeof(char)*4 );
        }
        m_curvesLUTImage->SetData( data );
        delete[] data;
    }

    // ------ UI ------ //

    void UiCurvesNode::RenderNode( Node* node ) const
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