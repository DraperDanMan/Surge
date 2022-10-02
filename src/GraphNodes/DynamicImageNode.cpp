#include "DynamicImageNode.h"

#include "imgui.h"
#include "../imnodes.h"
#include <nfd.h>
#include <filesystem>


namespace Surge
{

DynamicImageNode::DynamicImageNode( const std::string &path ) : Node( NodeType::DYNAMIC_IMAGE )
{
    name = "Dynamic Image";
    if ( path.empty() )
    {
        RequestRoot();
    }
    else
    {
        BuildPathList( path );
    }
}

void DynamicImageNode::RequestRoot()
{
    nfdchar_t *dirPath = nullptr;
    const nfdresult_t result = NFD_PickFolder( nullptr, &dirPath );
    if ( result == NFD_OKAY )
    {
        //Success
        BuildPathList( dirPath );
        free(dirPath);
    }
    else if ( result == NFD_CANCEL )
    {
        fprintf(stderr, "User canceled save." );
    }
    else 
    {
        fprintf(stderr, "Error: %s\n", NFD_GetError() );
    }
}

void DynamicImageNode::BuildPathList( const std::string &root )
{
    m_folderPath = root;

    namespace fs = std::filesystem;
    for (const auto& entry : fs::directory_iterator(m_folderPath))
    {
        auto filename = entry.path().filename();
        std::string pathStr = entry.path().string();
        if (fs::is_directory(entry.status()))
        {
            //dir
        }
        else if (fs::is_regular_file(entry.status()))
        {
            if ( !IsSupportedExtension( entry.path().extension().generic_string() ) )
            {
                continue;
            }
            m_filePaths.push_back( pathStr );
        }
        else
        {
            // unknown
        }
    }
    if ( m_filePaths.size() <= 0)
    {
        value = std::make_shared<Image>( 2048, 2048, ImageFormat::RGBA );
        return;
    }
    value = std::make_shared<Image>( m_filePaths[m_currentImage] );
}

bool DynamicImageNode::NextImage()
{
    bool hitEnd = false;
    m_currentImage++;
    if ( m_currentImage >= m_filePaths.size() )
    {
        m_currentImage = 0;
        hitEnd = true;
    }
    value = std::make_shared<Image>( m_filePaths[m_currentImage] );
        
    return hitEnd;
}

std::shared_ptr<Image> DynamicImageNode::Evaluate( std::stack<std::shared_ptr<Image>> &value_stack )
{
    return value;
}

bool DynamicImageNode::RenderProperties()
{
    ImGui::Text( name.c_str() );
    ImGui::Separator();
    bool changed = false;
    if ( ImGui::Button( "Open" ))
    {
        RequestRoot();
    }
    ImGui::SameLine();
    ImGui::TextDisabled( m_folderPath.c_str() );
    if ( ImGui::Button( "Next Image" ) )
    {
        changed |= true;
        NextImage();
    }
    return changed;
}

// ------ UI ------ //

void UiDynamicImageNode::RenderNode( Node* node ) const
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
    
    const DynamicImageNode *dynImage = static_cast<DynamicImageNode*>( node );
    ImGui::TextDisabled( dynImage->m_folderPath.c_str() );
    ImGui::Image( dynImage->value->GetDescriptorSet(), ImVec2( node_width,node_width ), ImVec2(0, 0), ImVec2(1,1), ImVec4(1,1,1,1), ImVec4(0.6f,0.6f,0.6f,1) );
        
    ImNodes::EndNode();
}
}