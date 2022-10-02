#pragma once

#include "Node.h"

#include <vector>

namespace Surge
{

struct DynamicImageNode : Node
{
    std::string m_folderPath;
    uint32_t m_currentImage = 0;
    std::vector<std::string> m_filePaths;
    
    DynamicImageNode( const std::string &path );

    const std::vector<std::string> Extensions = { ".png", ".jpg", ".tga" };

    bool IsSupportedExtension(std::string extension)
    {
        for ( const auto exten : Extensions )
        {
            if ( extension == exten )
            {
                return true;
            }
        }
        return false;
    }

    void RequestRoot();
    
    void BuildPathList( const std::string &root );
    
    bool NextImage();
    
    std::shared_ptr<Image> Evaluate( std::stack<std::shared_ptr<Image>> &value_stack ) override;

    bool RenderProperties() override;
};

struct UiDynamicImageNode : UiNode
{
    void RenderNode(Node* node) const override;
};
    
}