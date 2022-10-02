#include "ExplorerWindow.h"

#include <nfd.h>

#include "Application.h"

namespace Surge
{

const std::vector<std::string> Extensions = { ".png", ".jpg", ".tga" };
    
static bool IsSupportedExtension(std::string_view extension)
{
    for ( const auto& exten : Extensions )
    {
        if ( extension == exten )
        {
            return true;
        }
    }
    return false;
}
    
ExplorerWindow::ExplorerWindow()
{
    Init();
}

ExplorerWindow::~ExplorerWindow()
{
    Shutdown();
}

void ExplorerWindow::Init()
{
    m_folderImage = std::make_shared<Image>( "folder.png" );
    AppConfig &config = Application::GetConfig();
    m_explorerRoot = config.explorerRoot;
    if ( m_explorerRoot.empty() )
    {
        RequestExplorerRoot();
        config.explorerRoot = m_explorerRoot.string();
    }
    NavigateToDir( m_explorerRoot );
    
}

void ExplorerWindow::Shutdown()
{
    AppConfig &config = Application::GetConfig();
    config.explorerRoot = m_explorerRoot.string();
    m_imageCache.clear();
    m_currentImages.clear();
}

void ExplorerWindow::RequestExplorerRoot()
{
    nfdchar_t *dirPath = nullptr;
    const nfdresult_t result = NFD_PickFolder( nullptr, &dirPath );
    if ( result == NFD_OKAY )
    {
        //Success
        m_explorerRoot = dirPath;
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

void ExplorerWindow::NavigateToDir( const std::filesystem::path &path )
{
    m_currentImages.clear();
    namespace fs = std::filesystem;
    if (fs::exists(path) && fs::is_directory(path))
    {
        m_relativePath = fs::relative( path, m_explorerRoot.parent_path() );
        for (const auto& entry : fs::directory_iterator(path))
        {
            auto filename = entry.path().filename();
            std::string pathStr = entry.path().string();
            if (fs::is_directory(entry.status()))
            {
                //dir
                if (m_imageCache.find( pathStr ) == m_imageCache.end())
                {
                    m_imageCache.emplace( pathStr, m_folderImage );
                }
                m_currentImages.push_back( pathStr );
            }
            else if (fs::is_regular_file(entry.status()))
            {
                if ( !IsSupportedExtension( entry.path().extension().generic_string() ) )
                {
                    continue;
                }
                //files
                if (m_imageCache.find( pathStr ) == m_imageCache.end())
                {
                    m_imageCache.emplace( pathStr, std::make_shared<Image>( pathStr ) );
                }
                m_currentImages.push_back( pathStr );
            }
            else
            {
                // unknown
            }
        }
    }
}

void ExplorerWindow::UiRender()
{
    ImGui::Begin( "Explorer" );
    
    ImGui::BeginGroup();
    //Draw an icon for new root folder
    if ( ImGui::ImageButton( m_folderImage->GetDescriptorSet(), ImVec2(24,24) ) )
    {
        RequestExplorerRoot();
        NavigateToDir( m_explorerRoot );
    }
    if ( ImGui::IsItemHovered() )
    {
        ImGui::BeginTooltip();
        ImGui::Text( "Select a new Project Root Folder" );
        ImGui::EndTooltip();
    }
    ImGui::PushFont( Application::GetFont( FontType::Header ) );
    std::filesystem::path currentPath;
    for (auto folder = m_relativePath.begin(); folder != m_relativePath.end(); ++folder)
    {
        currentPath /= *folder; //build up as we go for when we capture the click.
        ImGui::SameLine( 0, 5);
        if ( ImGui::Button( (*folder).generic_string().c_str() ))
        {
            const std::filesystem::path dir = m_explorerRoot.parent_path() / currentPath;
            NavigateToDir( dir );
            break;
        }
    }
    ImGui::PopFont();
    ImGui::PushStyleColor( ImGuiCol_Separator, ImColor(0.8f, 0.8f, 0.8f).Value );
    ImGui::Separator();
    ImGui::PopStyleColor();
    ImGui::EndGroup();
    
    float maxWidth = std::max( 128.f , ImGui::GetWindowContentRegionWidth() );
    ImVec2 size = ImVec2( 128, 128 );
    int wrapCount = static_cast<int>( maxWidth / size.x);
    for ( int i = 0; i < m_currentImages.size(); ++i )
    {
        const std::string& currentImage = m_currentImages[i];
        ImGui::PushID(i);
        
        if ((i % wrapCount) != 0)
        {
            ImGui::SameLine();
        }
        std::shared_ptr<Image> image = m_imageCache[currentImage];
        bool isFolder = image == m_folderImage;
        ImGui::BeginGroup();
        if (isFolder)
        {
            if (ImGui::ImageButton( image->GetDescriptorSet(), size ))
            {
                //TODO draperdanman: this is jank as all get out. Fix this
                NavigateToDir( currentImage );
            }
        }
        else
        {
            ImGui::Image( image->GetDescriptorSet(), size );
        }
        std::filesystem::path file = currentImage;
        ImGui::Text("%s", file.filename().generic_string().c_str());
        ImGui::EndGroup();
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID) && !isFolder)
        {
            // Set payload to carry the index of our item (could be anything)
            ImGui::SetDragDropPayload("DND_EXPLORER", &i, sizeof(int));

            // Display preview, image names
            ImGui::Text("%s", m_imageCache[currentImage]->GetFilename().c_str());
            ImGui::EndDragDropSource();
        }
        
        ImGui::PopID();
    }
    
    ImGui::End();
}

std::shared_ptr<Image> ExplorerWindow::GetImageForIndex( int index )
{
    if ( index >= m_currentImages.size() || index < 0 )
    {
        return nullptr;
    }
    const std::string& currentImage = m_currentImages[index];
    return m_imageCache[currentImage];
}
    
}