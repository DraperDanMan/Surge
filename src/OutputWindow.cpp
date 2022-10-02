#include "OutputWindow.h"

#include "Application.h"

namespace Surge
{
OutputWindow::OutputWindow( const NodeCanvas *nodeCanvas )
{
    m_canvas = nodeCanvas;
    Init();
}

OutputWindow::~OutputWindow()
{
    Shutdown();
}

void OutputWindow::Init()
{
    m_outputImage = std::make_shared<Image>( 2048, 2048, ImageFormat::RGBA );
    Refresh();
}

void OutputWindow::Shutdown()
{
    m_outputImage = nullptr;
    m_graph = nullptr;
    m_canvas = nullptr;
}

void OutputWindow::Refresh()
{
    const int roodId = m_canvas->GetRootNodeId();
    m_graph = m_canvas->GetGraph();
    if ( roodId < 0) return;
    m_outputImage = m_graph->node(roodId)->value;
}

void OutputWindow::UiRender()
{
    Refresh();
    
    ImGui::Begin("Output");
    ImGui::Text( "Show: " );
    ImGui::SameLine();
    if ( ImGui::Button( m_isFollowingSelection ? "output" : "selection" ) )
    {
        m_isFollowingSelection = !m_isFollowingSelection;
    }

    const int selectedNodeCount = ImNodes::NumSelectedNodes();
    std::vector<int> selectedNodes;
    selectedNodes.resize(static_cast<size_t>(selectedNodeCount));
    ImNodes::GetSelectedNodes(selectedNodes.data());

    //choose either the selected or the output node
    const std::shared_ptr<Image> output = (selectedNodeCount > 0 && m_isFollowingSelection) ? m_graph->node( selectedNodes[0] )->value : m_outputImage;

    float maxWidth = ImGui::GetWindowContentRegionWidth();
    
    const auto width = static_cast<float>( output->GetWidth() );
    maxWidth = maxWidth < width ? maxWidth : width;
    const auto height = static_cast<float>( output->GetHeight() );

    const ImVec2 size = ImVec2( maxWidth, maxWidth );
    
    ImGui::Image( output->GetDescriptorSet(), size );
    ImGui::End();
}
    
}