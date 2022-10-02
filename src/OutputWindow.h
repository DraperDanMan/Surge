#pragma once

#include "Image.h"
#include "NodeCanvas.h"

namespace Surge
{
    class OutputWindow
    {
    public:
        OutputWindow( const NodeCanvas *nodeCanvas );
        ~OutputWindow();

        void UiRender();

    private:
        void Init();
        void Shutdown();
        void Refresh();
        
        bool m_isFollowingSelection = true;
        bool m_isStretchingOutput = false;
        const NodeCanvas *m_canvas = nullptr;
        const Graph<Node *> *m_graph = nullptr;
        std::shared_ptr<Image> m_outputImage;
    };
    
}