#pragma once

#include "Image.h"

#include <filesystem>
#include <map>

namespace Surge
{
class ExplorerWindow
{
public:
    ExplorerWindow();
    ~ExplorerWindow();

    void UiRender();

    std::shared_ptr<Image> GetImageForIndex( int index );
private:
    void Init();
    void Shutdown();

    void RequestExplorerRoot();

    void NavigateToDir( const std::filesystem::path &path );
    
    std::filesystem::path m_explorerRoot;
    std::filesystem::path m_relativePath;

    std::vector<std::string> m_currentImages;
    //TODO draperdanman: fix this to only load the currently needed images, not cache everything
    std::map<std::string,std::shared_ptr<Image>> m_imageCache;
    std::shared_ptr<Image> m_folderImage;
};
    
}