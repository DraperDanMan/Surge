#pragma once

#include <filesystem>
#include <memory>
#include <utility>

#include "Graph.h"
#include "Image.h"
#include "imgui.h"
#include "imnodes.h"

#include "GraphNodes/Node.h"

namespace Surge
{

class NodeCanvas
{
public:
    NodeCanvas();
    ~NodeCanvas();

    bool RenderPropertiesWindow();
    void UiRender();
    void Export() const;
    void SaveGraph( std::string filepath );
    void LoadGraph( std::string filepath );
    void ClearProject();

    bool HasNodes() const { return m_nodes.empty(); }
    const Graph<Node *> *GetGraph() const { return &m_graph; }
    int GetRootNodeId() const { return m_rootNodeId; }
private:
    void Init();
    void Shutdown();

    std::shared_ptr<Image> Evaluate(const Graph<Node *> &graph, const int startNode) const;
    void DrawCreateNodeMenu( const ImVec2 createPos );
    Graph<Node *>          m_graph;
    std::vector<UiNode *>  m_nodes;
    int                    m_rootNodeId;
    ImNodesMiniMapLocation m_minimapLocation;

    std::shared_ptr<Image> m_outputImage;
};

}