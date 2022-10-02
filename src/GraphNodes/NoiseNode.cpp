#include "NoiseNode.h"

#include "imgui.h"
#include "../imnodes.h"

namespace Surge
{

NoiseNode::NoiseNode() : Node( NodeType::NOISE )
{
    name = "Noise";
    if ( !noiseCompute )
    {
        noiseCompute = new NoiseCompute();
    }
    value = std::make_shared<Image>( 2048, 2048, ImageFormat::RGBA );
    constexpr auto bufSize = static_cast<size_t>( 2048 * 2048 * 4 );
    auto *data = new char[bufSize];
    memset( data, 255,  bufSize);
    value->SetData( data );
    delete[] data;
}

std::shared_ptr<Image> NoiseNode::Evaluate(std::stack<std::shared_ptr<Image>> &value_stack)
{
    noiseCompute->Run( value.get(), { m_mode, value->GetWidth(), value->GetHeight(), m_seed, m_scale } );
    return value;
}

bool NoiseNode::RenderProperties()
{
    ImGui::Text( name.c_str() );
    ImGui::Separator();
    bool changed = false;
    const char* items[] = { "Raw", "Voronoi", "Perlin", "Smokey" };
    int item = static_cast<int>( m_mode );
    if ( ImGui::Combo("Mode", &item, items, IM_ARRAYSIZE(items)) )
    {
        changed = true;
    }
    m_mode = static_cast<NoiseCompute::NoiseMode>( item );

    changed |= ImGui::DragInt( "Seed", &m_seed, 1, 0, UINTMAX_MAX );
    changed |= ImGui::DragFloat( "Scale", &m_scale, 0.1f, -256.f, 256.f );

    return changed;
}

// ------ UI ------ //

void UiNoiseNode::RenderNode( Node* node ) const
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