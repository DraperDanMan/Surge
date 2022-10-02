#include "NodeCanvas.h"
#include "nfd.h"
#include <fstream>
#include <memory>

#include "Application.h"
#include "imnodes_internal.h"

#include "GraphNodes/GraphNodes.h"

namespace Surge
{

NodeCanvas::NodeCanvas()
    : m_rootNodeId( -1 ), m_minimapLocation( ImNodesMiniMapLocation_BottomRight )
{
    Init();
}


NodeCanvas::~NodeCanvas()
{
    Shutdown();
}


void NodeCanvas::Init()
{
    ImNodes::CreateContext();
    ImNodesIO& io = ImNodes::GetIO();
    io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;

    m_outputImage = std::make_shared<Image>( 2048, 2048, ImageFormat::RGBA);
    constexpr auto bufSize = static_cast<size_t>( 2048 * 2048 * 4 );
    auto *data = new char[bufSize];
    memset( data, 255,  bufSize);
    m_outputImage->SetData( data );
    delete[] data;
}


void NodeCanvas::Shutdown()
{
    ImNodes::DestroyContext();
}


std::shared_ptr<Image> NodeCanvas::Evaluate( const Graph<Node *>& graph, const int startNode ) const
{
    std::stack<int> postorder;
    dfs_traverse( graph, startNode, [&postorder]( const int nodeId ) -> void { postorder.push( nodeId ); } );

    std::stack<std::shared_ptr<Image>> value_stack;
    while (!postorder.empty())
    {
        const int id = postorder.top();
        postorder.pop();
        Node *node = graph.node(id);

        switch (node->type)
        {
        case NodeType::VALUE:
        {
            // If the edge does not have an edge connecting to another node, then just use the value
            // at this node. It means the node's input pin has not been connected to anything and
            // the value comes from the node's UI.
            if (graph.num_edges_from_node(id) == 0ull)
            {
                value_stack.push(node->value);
            }
        }
        break;
        case NodeType::BLEND:
        case NodeType::HSL:
        case NodeType::LEVELS:
        case NodeType::CURVES:
        case NodeType::TRANSFORM:
        case NodeType::BLUR:
        case NodeType::INVERT:
        case NodeType::UNIFORM_COLOR:
        case NodeType::NOISE:
        case NodeType::DYNAMIC_IMAGE:
        case NodeType::IMAGE:
        {
            auto val = node->Evaluate( value_stack );
            value_stack.push(val);
        }
        break;
        default:
            break;
        }
    }

    // The final output node isn't evaluated in the loop -- instead we just pop
    // the three values which should be in the stack.
    assert(value_stack.size() == 1ull);
    
    return value_stack.top();
}

void NodeCanvas::DrawCreateNodeMenu( const ImVec2 createPos )
{
    if (ImGui::MenuItem("Blend"))
    {
        Node *value = new Node(NodeType::VALUE);
        Node *op = new BlendNode();

        const auto ui_node = new UiBlendNode();
        ui_node->type = NodeType::BLEND;
        ui_node->ui.two.lhs = m_graph.insert_node(value);
        ui_node->ui.two.rhs = m_graph.insert_node(value);
        ui_node->id = m_graph.insert_node(op);

        m_graph.insert_edge(ui_node->id, ui_node->ui.two.lhs);
        m_graph.insert_edge(ui_node->id, ui_node->ui.two.rhs);

        m_nodes.push_back(ui_node);
        ImNodes::SetNodeScreenSpacePos(ui_node->id, createPos);
    }

    if (ImGui::MenuItem("HSL"))
    {
        Node *value = new Node(NodeType::VALUE);
        Node *op = new HSLNode();

        UiHSLNode *ui_node = new UiHSLNode();
        ui_node->type = NodeType::HSL;
        ui_node->ui.one.input = m_graph.insert_node(value);
        ui_node->id = m_graph.insert_node(op);

        m_graph.insert_edge(ui_node->id, ui_node->ui.one.input);

        m_nodes.push_back(ui_node);
        ImNodes::SetNodeScreenSpacePos(ui_node->id, createPos);
    }

    if (ImGui::MenuItem("Levels"))
    {
        Node *value = new Node(NodeType::VALUE);
        Node *op = new LevelsNode();

        UiLevelsNode *ui_node = new UiLevelsNode();
        ui_node->type = NodeType::LEVELS;
        ui_node->ui.one.input = m_graph.insert_node(value);
        ui_node->id = m_graph.insert_node(op);

        m_graph.insert_edge(ui_node->id, ui_node->ui.one.input);

        m_nodes.push_back(ui_node);
        ImNodes::SetNodeScreenSpacePos(ui_node->id, createPos);
    }

    if (ImGui::MenuItem("Curves"))
    {
        Node *value = new Node(NodeType::VALUE);
        Node *op = new CurvesNode();

        UiCurvesNode *ui_node = new UiCurvesNode();
        ui_node->type = NodeType::CURVES;
        ui_node->ui.one.input = m_graph.insert_node(value);
        ui_node->id = m_graph.insert_node(op);

        m_graph.insert_edge(ui_node->id, ui_node->ui.one.input);

        m_nodes.push_back(ui_node);
        ImNodes::SetNodeScreenSpacePos(ui_node->id, createPos);
    }

    if (ImGui::MenuItem("Blur"))
    {
        Node *value = new Node(NodeType::VALUE);
        Node *op = new BlurNode();

        UiBlurNode *ui_node = new UiBlurNode();
        ui_node->type = NodeType::BLUR;
        ui_node->ui.one.input = m_graph.insert_node(value);
        ui_node->id = m_graph.insert_node(op);

        m_graph.insert_edge(ui_node->id, ui_node->ui.one.input);

        m_nodes.push_back(ui_node);
        ImNodes::SetNodeScreenSpacePos(ui_node->id, createPos);
    }

    if (ImGui::MenuItem("Invert"))
    {
        Node *value = new Node(NodeType::VALUE);
        Node *op = new InvertNode();

        UiInvertNode *ui_node = new UiInvertNode();
        ui_node->type = ui_node->type;//NodeType::INVERT;
        ui_node->ui.one.input = m_graph.insert_node(value);
        ui_node->id = m_graph.insert_node(op);

        m_graph.insert_edge(ui_node->id, ui_node->ui.one.input);

        m_nodes.push_back(ui_node);
        ImNodes::SetNodeScreenSpacePos(ui_node->id, createPos);
    }

    if (ImGui::MenuItem("Transform"))
    {
        Node *value = new Node(NodeType::VALUE);
        Node *op = new TransformNode();

        UiTransformNode *ui_node = new UiTransformNode();
        ui_node->type = ui_node->type;//NodeType::TRANSFORM;
        ui_node->ui.one.input = m_graph.insert_node(value);
        ui_node->id = m_graph.insert_node(op);

        m_graph.insert_edge(ui_node->id, ui_node->ui.one.input);

        m_nodes.push_back(ui_node);
        ImNodes::SetNodeScreenSpacePos(ui_node->id, createPos);
    }
    
    if (ImGui::MenuItem("Uniform Color"))
    {
        UiUniformColorNode *ui_node = new UiUniformColorNode();
        ui_node->type = ui_node->type;//NodeType::UNIFORM_COLOR;
        ui_node->id = m_graph.insert_node(new UniformColorNode());

        m_nodes.push_back(ui_node);
        ImNodes::SetNodeScreenSpacePos(ui_node->id, createPos);
    }

    if (ImGui::MenuItem("Noise"))
    {
        UiNoiseNode *ui_node = new UiNoiseNode();
        ui_node->type = ui_node->type;//NodeType::NOISE;
        ui_node->id = m_graph.insert_node(new NoiseNode());

        m_nodes.push_back(ui_node);
        ImNodes::SetNodeScreenSpacePos(ui_node->id, createPos);
    }

    if ( ImGui::MenuItem( "Dynamic Image" ) )
    {
        UiDynamicImageNode *ui_node = new UiDynamicImageNode();
        ui_node->type = ui_node->type;//NodeType::DYNAMIC_IMAGE;
        ui_node->id = m_graph.insert_node( new DynamicImageNode( std::string() ) );

        m_nodes.push_back(ui_node);
        ImNodes::SetNodeScreenSpacePos(ui_node->id, createPos);
    }
    
    if (ImGui::MenuItem("Output") && m_rootNodeId == -1)
    {
        Node *value = new Node(NodeType::VALUE);
        Node *out = new OutputNode();

        UiOutputNode *ui_node = new UiOutputNode();
        ui_node->type = ui_node->type;//NodeType::OUTPUT;
        ui_node->ui.one.input = m_graph.insert_node(value);
        ui_node->id = m_graph.insert_node(out);

        m_graph.insert_edge(ui_node->id, ui_node->ui.one.input);

        m_nodes.push_back(ui_node);
        ImNodes::SetNodeScreenSpacePos(ui_node->id, createPos);
        m_rootNodeId = ui_node->id;
    }
}


void NodeCanvas::UiRender()
{
    constexpr auto flags = ImGuiWindowFlags_MenuBar;

    // The node editor window
    ImGui::Begin("Node Editor", nullptr, flags);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Nodes"))
        {
            //calculate current canvas center;
            const ImVec2 canvasCenter = ImNodes::GetCurrentContext()->CanvasRectScreenSpace.GetCenter();
            DrawCreateNodeMenu( canvasCenter );
            
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
    
    ImGui::Columns(1);

    const bool save_image = ImGui::IsKeyDown( ImGuiKey_LeftCtrl ) &&
                            ImGui::IsKeyReleased( ImGuiKey_S );

    if ( save_image )
    {
        Export();
    }
    
    bool invalidateGraph = false;
    
    ImNodes::BeginNodeEditor();

    // Handle new nodes
    // These are driven by the user, so we place this code before rendering the nodes
    {
        const bool open_popup = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
                                ImNodes::IsEditorHovered() &&
                                ( ImGui::IsKeyReleased(ImGuiKey_A) || ImGui::IsMouseClicked( ImGuiMouseButton_Right ) );

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
        if (!ImGui::IsAnyItemHovered() && open_popup)
        {
            ImGui::OpenPopup("Add Node");
        }

        if (ImGui::BeginPopup("Add Node"))
        {
            const ImVec2 clickPos = ImGui::GetMousePosOnOpeningCurrentPopup();
            DrawCreateNodeMenu( clickPos );
            
            ImGui::EndPopup();
        }
        ImGui::PopStyleVar();
    }

    for (const UiNode *node : m_nodes)
    {
        node->RenderNode( m_graph.node( node->id ) );
    }

    for (const auto& edge : m_graph.edges())
    {
        // If edge doesn't start at value, then it's an internal edge, i.e.
        // an edge which links a node's operation to its input. We don't
        // want to render node internals with visible links.
        if (m_graph.node(edge.from)->type != NodeType::VALUE)
        {
            continue;
        }

        ImNodes::Link(edge.id, edge.from, edge.to);
    }

    ImNodes::MiniMap(0.2f, m_minimapLocation);
    ImNodes::EndNodeEditor();
    
    // Handle new links
    // These are driven by Imnodes, so we place the code after EndNodeEditor().
    int start_attr, end_attr;
    if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
    {
        const NodeType start_type = m_graph.node(start_attr)->type;
        const NodeType end_type = m_graph.node(end_attr)->type;

        if (start_type != end_type)
        {
            // Ensure the edge is always directed from the value to
            // whatever produces the value
            if (start_type != NodeType::VALUE)
            {
                std::swap(start_attr, end_attr);
            }
            // Input nodes can only have one connection coming in, but outputs can have many.
            // enforce this here by unbinding the previous connection, before the new one is bound
            for( auto edge : m_graph.edges())
            {
                if ( edge.from == start_attr )
                {
                    m_graph.erase_edge( edge.id );
                    break;    
                }
            }
            m_graph.insert_edge(start_attr, end_attr);
            invalidateGraph = true;
        }
    }
    

    // Handle deleted links
    int link_id;
    if (ImNodes::IsLinkDestroyed(&link_id))
    {
        m_graph.erase_edge(link_id);
        invalidateGraph = true;
    }

    
    const int numLinksSelected = ImNodes::NumSelectedLinks();
    const bool deleteReleased = ImGui::IsKeyReleased(ImGuiKey_X) || ImGui::IsKeyReleased(ImGuiKey_Delete);
    if (numLinksSelected > 0 && deleteReleased)
    {
        static std::vector<int> selected_links;
        selected_links.resize(static_cast<size_t>(numLinksSelected));
        ImNodes::GetSelectedLinks(selected_links.data());
        for (const int edge_id : selected_links)
        {
            m_graph.erase_edge(edge_id);
        }
        invalidateGraph = true;
    }
    

    
    const int numNodesSelected = ImNodes::NumSelectedNodes();
    if (numNodesSelected > 0 && deleteReleased)
    {
        static std::vector<int> selected_nodes;
        selected_nodes.resize(static_cast<size_t>(numNodesSelected));
        ImNodes::GetSelectedNodes(selected_nodes.data());
        for (const int node_id : selected_nodes)
        {
            m_graph.erase_node(node_id);
            const auto iter = std::find_if(
                m_nodes.begin(), m_nodes.end(), [node_id](const UiNode *node) -> bool {
                    return node->id == node_id;
                });
            // Erase any additional internal nodes
            const UiNode *erasedNode = (*iter);
            switch (erasedNode->type)
            {
            case NodeType::BLEND:
                m_graph.erase_node(erasedNode->ui.two.lhs);
                m_graph.erase_node(erasedNode->ui.two.rhs);
                break;
            case NodeType::OUTPUT:
                m_graph.erase_node(erasedNode->ui.one.input);
                m_rootNodeId = -1;
                break;
            default:
                m_graph.erase_node(erasedNode->ui.one.input);
                break;
            }
            m_nodes.erase(iter);
        }
        invalidateGraph = true;
    }
    

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_EXPLORER"))
        {
            const ImVec2 click_pos = ImGui::GetMousePos();
            IM_ASSERT(payload->DataSize == sizeof(int));
            int payloadN = *static_cast<const int*>( payload->Data );

            std::shared_ptr<Image> image = Application::GetExplorer()->GetImageForIndex( payloadN );
            UiImageNode *ui_node = new UiImageNode();
            ui_node->type = NodeType::IMAGE;
            ui_node->id = m_graph.insert_node( new ImageNode( image ) );

            m_nodes.push_back(ui_node);
            ImNodes::SetNodeScreenSpacePos(ui_node->id, click_pos);
        }
        ImGui::EndDragDropTarget();
    }
    
    ImGui::End();

    invalidateGraph |= RenderPropertiesWindow();
    
    // Calculate if invalid
    if (invalidateGraph && m_rootNodeId != -1)
    {
        m_outputImage = Evaluate(m_graph, m_rootNodeId);
        Node *node = m_graph.node(m_rootNodeId);
        node->value = m_outputImage;
        m_graph.update_node( m_rootNodeId, node );
    }
}

    
bool NodeCanvas::RenderPropertiesWindow()
{
    bool invalidate = false;
    ImGui::Begin( "Properties" );

    int selectedNodeCount = ImNodes::NumSelectedNodes();
    std::vector<int> selectedNodes;
    selectedNodes.resize(static_cast<size_t>(selectedNodeCount));
    ImNodes::GetSelectedNodes(selectedNodes.data());

    if ( selectedNodeCount > 0 )
    {
        Node *activeNode = m_graph.node( selectedNodes[0] );
        invalidate = activeNode->RenderProperties();
    }
    else
    {
        ImGui::Text( "Select a node to view its properties." );
    }
    ImGui::End();
    return invalidate;
}

    
void NodeCanvas::Export() const
{
    nfdchar_t *savePath = nullptr;
    nfdresult_t result = NFD_SaveDialog( "png", nullptr, &savePath );
    if ( result == NFD_OKAY )
    {
        puts("Success!");

        std::vector<UiNode *> dynamicNodes;
        std::filesystem::path outFolder = savePath;
        for ( const auto& node : m_nodes )
        {
            if ( node->type == NodeType::DYNAMIC_IMAGE )
            {
                dynamicNodes.push_back( node );
                if (outFolder.empty())
                {
                    const auto dynImage = static_cast<DynamicImageNode*>( m_graph.node( node->id ) );
                    outFolder = dynImage->m_folderPath;
                }
            }
        }
        if ( !dynamicNodes.empty() )
        {
            bool allDone = false;
            int index = 0;
            while ( !allDone )
            {
                for ( const auto& dynNode : dynamicNodes )
                {
                    const auto dynImage = dynamic_cast<DynamicImageNode*>( m_graph.node( dynNode->id ) );
                    if ( dynImage->NextImage() )
                    {
                        allDone = true;
                        break;
                    }
                }
                const std::shared_ptr<Image> temp = Evaluate( m_graph, m_rootNodeId );
                std::string outFile = outFolder.remove_filename().generic_string();
                outFile.append( std::to_string( index ) );
                temp->SaveToFile( outFile );
                index++;
            }
        }
        else
        {
            m_outputImage->SaveToFile( savePath );
        }
        
        free(savePath);
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

    
void NodeCanvas::SaveGraph( std::string filepath )
{
    std::ofstream outfile( filepath, std::ofstream::binary );

    outfile << m_nodes.size() << " ";
    for( const auto& node : m_nodes)
    {
        outfile << static_cast<int>( node->type ) << " ";
        outfile << node->id  << " ";

        const ImVec2 nodePos = ImNodes::GetNodeGridSpacePos( node->id );
        outfile << nodePos.x << " " << nodePos.y << " ";
        
        switch (node->type)
        {
        case NodeType::BLEND:
            {
                outfile << node->ui.two.lhs << " ";
                outfile << node->ui.two.rhs << " ";
                auto blendNode = dynamic_cast<BlendNode *>( m_graph.node( node->id ));
                outfile << static_cast<int>( blendNode->m_mode ) << " ";
            }
            break;
        case NodeType::HSL:
            {
                outfile << node->ui.one.input << " ";
                auto hslNode = dynamic_cast<HSLNode *>( m_graph.node( node->id ));
                outfile << hslNode->m_hue << " " << hslNode->m_lightness << " " << hslNode->m_saturation << " ";
            }
            break;
        case NodeType::LEVELS:
            {
                outfile << node->ui.one.input << " ";
                auto levelsNode = dynamic_cast<LevelsNode *>( m_graph.node( node->id ));
                outfile << levelsNode->m_gamma << " " << levelsNode->m_luminanceOnly << " ";
                outfile << levelsNode->m_inputRange.x << " " << levelsNode->m_inputRange.y << " ";
                outfile << levelsNode->m_outputRange.x << " " << levelsNode->m_outputRange.y << " ";
            }
            break;
        case NodeType::BLUR: 
            {
                outfile << node->ui.one.input << " ";
                auto blurNode = dynamic_cast<BlurNode *>( m_graph.node( node->id ));
                outfile << static_cast<int>( blurNode->m_blurMode );
                outfile << blurNode->m_angle << " " << blurNode->m_samples << " " << blurNode->m_sigma << " " << blurNode->m_useAlpha << " ";
                outfile << blurNode->m_center.x << " " << blurNode->m_center.y << " ";
            }
            break;
        case NodeType::INVERT:
            {
                outfile << node->ui.one.input << " ";
                auto invertNode = dynamic_cast<InvertNode *>( m_graph.node( node->id ));
                outfile << invertNode->m_channels << " ";
            }
            break;
        case NodeType::CURVES:
            {
                outfile << node->ui.one.input << " ";
                auto op = dynamic_cast<CurvesNode *>( m_graph.node( node->id ));
                outfile << op->m_red[0] << " " << op->m_red[1] << " " << op->m_red[2] << " " << op->m_red[3] << " " << op->m_red[4] << " ";
                outfile << op->m_green[0] << " " << op->m_green[1] << " " << op->m_green[2] << " " << op->m_green[3] << " " << op->m_green[4] << " ";
                outfile << op->m_blue[0] << " " << op->m_blue[1] << " " << op->m_blue[2] << " " << op->m_blue[3] << " " << op->m_blue[4] << " ";
            }
            break;
        case NodeType::OUTPUT:
            {
                outfile << node->ui.one.input << " ";
            }
            break;
        case NodeType::UNIFORM_COLOR: 
            {
                auto uniColNode = dynamic_cast<UniformColorNode *>( m_graph.node( node->id ));
                outfile << uniColNode->m_color.asPart.red << " " << uniColNode->m_color.asPart.green << " " << uniColNode->m_color.asPart.blue << " " << uniColNode->m_color.asPart.alpha << " ";
            }
            break;
        case NodeType::NOISE: 
            {
                auto noiseNode = dynamic_cast<NoiseNode *>( m_graph.node( node->id ));
                outfile << static_cast<int>( noiseNode->m_mode ) << " ";
                outfile << noiseNode->m_seed << " ";
                outfile << noiseNode->m_scale << " ";

                //other values
            }
            break;
        case NodeType::IMAGE:
            {
                auto imageNode = dynamic_cast<ImageNode *>( m_graph.node( node->id ));
                const std::string fname = imageNode->value->GetFilename();
                const int strLen = static_cast<int>( fname.size() );
                outfile << strLen << " " << fname << " ";
            }
            break;
        case NodeType::DYNAMIC_IMAGE:
            {
                auto dynImage = dynamic_cast<DynamicImageNode *>( m_graph.node( node->id ));
                const std::string fpath = dynImage->m_folderPath;
                const int strLen = static_cast<int>( fpath.size() );
                outfile << strLen << " " << fpath << " ";
            }
            break;
        }
    }

    const auto edges = m_graph.edges();
    outfile << edges.size() << " ";
    for ( auto edge : edges)
    {
        outfile << edge.from << " " << edge.to << " ";
    }
    outfile.close();
}

void NodeCanvas::LoadGraph( std::string filepath )
{
    std::ifstream infile( filepath, std::ifstream::binary );

    std::vector<std::pair<int, int>> fixUpTable;
    
    int totalNodes;
    infile >> totalNodes;
    for( int i = 0; i < totalNodes; ++i)
    {
        int nodeTypeRaw;
        infile >> nodeTypeRaw;
        auto nodeType = static_cast<NodeType>( nodeTypeRaw );
        
        int nodeId;
        infile >> nodeId;

        ImVec2 nodePos;
        infile >> nodePos.x >> nodePos.y;
        
        switch (nodeType)
        {
        case NodeType::BLEND:
            {
                Node *value = new Node(NodeType::VALUE);
                BlendNode *op = new BlendNode();

                UiBlendNode *ui_node = new UiBlendNode();
                ui_node->type = NodeType::BLEND;

                int oldLhs, oldRhs;
                infile >> oldLhs >> oldRhs;
                ui_node->ui.two.lhs = m_graph.insert_node(value);
                ui_node->ui.two.rhs = m_graph.insert_node(value);
                auto lhs = std::make_pair( oldLhs , ui_node->ui.two.lhs );
                auto rhs = std::make_pair( oldRhs , ui_node->ui.two.rhs );
                fixUpTable.push_back( lhs );
                fixUpTable.push_back( rhs );

                //Don't need to store old id, but need the pair for linking later.
                ui_node->id = m_graph.insert_node(op);
                auto nodePair = std::make_pair( nodeId, ui_node->id );
                fixUpTable.push_back( nodePair );

                m_nodes.push_back(ui_node);
                ImNodes::SetNodeGridSpacePos(ui_node->id, nodePos);

                int blendModeRaw;
                infile >> blendModeRaw;
                op->m_mode = static_cast<BlendCompute::BlendMode>( blendModeRaw );
            }
            break;
        case NodeType::HSL:
            {
                Node *value = new Node(NodeType::VALUE);
                HSLNode *op = new HSLNode();

                UiHSLNode *ui_node = new UiHSLNode();
                ui_node->type = NodeType::HSL;

                int oldInput;
                infile >> oldInput;
                ui_node->ui.one.input = m_graph.insert_node(value);
                auto inpair = std::make_pair( oldInput , ui_node->ui.one.input );
                fixUpTable.push_back( inpair );

                //Don't need to store old id, but need the pair for linking later.
                ui_node->id = m_graph.insert_node(op);
                auto nodePair = std::make_pair( nodeId, ui_node->id );
                fixUpTable.push_back( nodePair );

                m_nodes.push_back(ui_node);
                ImNodes::SetNodeGridSpacePos(ui_node->id, nodePos);
                
                infile >> op->m_hue >> op->m_lightness >> op->m_saturation;
            }
            break;
        case NodeType::LEVELS:
            {
                Node *value = new Node(NodeType::VALUE);
                LevelsNode *op = new LevelsNode();

                UiLevelsNode *ui_node = new UiLevelsNode();
                ui_node->type = NodeType::LEVELS;

                int oldInput;
                infile >> oldInput;
                ui_node->ui.one.input = m_graph.insert_node(value);
                auto inpair = std::make_pair( oldInput , ui_node->ui.one.input );
                fixUpTable.push_back( inpair );

                //Don't need to store old id, but need the pair for linking later.
                ui_node->id = m_graph.insert_node(op);
                auto nodePair = std::make_pair( nodeId, ui_node->id );
                fixUpTable.push_back( nodePair );

                m_nodes.push_back(ui_node);
                ImNodes::SetNodeGridSpacePos(ui_node->id, nodePos);
                
                infile >> op->m_gamma >> op->m_luminanceOnly;
                infile >> op->m_inputRange.x >> op->m_inputRange.y;
                infile >> op->m_outputRange.x >> op->m_outputRange.y;
            }
            break;
        case NodeType::BLUR: 
            {
                Node *value = new Node(NodeType::VALUE);
                BlurNode *op = new BlurNode();

                UiBlurNode *ui_node = new UiBlurNode();
                ui_node->type = NodeType::BLUR;

                int oldInput;
                infile >> oldInput;
                ui_node->ui.one.input = m_graph.insert_node(value);
                auto inpair = std::make_pair( oldInput , ui_node->ui.one.input );
                fixUpTable.push_back( inpair );

                //Don't need to store old id, but need the pair for linking later.
                ui_node->id = m_graph.insert_node(op);
                auto nodePair = std::make_pair( nodeId, ui_node->id );
                fixUpTable.push_back( nodePair );

                m_nodes.push_back(ui_node);
                ImNodes::SetNodeGridSpacePos(ui_node->id, nodePos);
                
                int blurModeRaw;
                infile >> blurModeRaw;
                op->m_blurMode = static_cast<BlurCompute::BlurMode>( blurModeRaw );
                infile >> op->m_angle >> op->m_samples >> op->m_sigma >> op->m_useAlpha;
                infile >> op->m_center.x >> op->m_center.y;
            }
            break;
        case NodeType::INVERT:
            {
                Node *value = new Node(NodeType::VALUE);
                InvertNode *op = new InvertNode();

                UiInvertNode *ui_node = new UiInvertNode();
                ui_node->type = NodeType::INVERT;

                int oldInput;
                infile >> oldInput;
                ui_node->ui.one.input = m_graph.insert_node(value);
                auto inpair = std::make_pair( oldInput , ui_node->ui.one.input );
                fixUpTable.push_back( inpair );

                //Don't need to store old id, but need the pair for linking later.
                ui_node->id = m_graph.insert_node(op);
                auto nodePair = std::make_pair( nodeId, ui_node->id );
                fixUpTable.push_back( nodePair );

                m_nodes.push_back(ui_node);
                ImNodes::SetNodeGridSpacePos(ui_node->id, nodePos);
                
                infile >> op->m_channels;
            }
            break;
        case NodeType::CURVES:
            {
                Node *value = new Node(NodeType::VALUE);
                CurvesNode *op = new CurvesNode();

                UiCurvesNode *ui_node = new UiCurvesNode();
                ui_node->type = NodeType::CURVES;

                int oldInput;
                infile >> oldInput;
                ui_node->ui.one.input = m_graph.insert_node(value);
                auto inpair = std::make_pair( oldInput , ui_node->ui.one.input );
                fixUpTable.push_back( inpair );

                //Don't need to store old id, but need the pair for linking later.
                ui_node->id = m_graph.insert_node(op);
                auto nodePair = std::make_pair( nodeId, ui_node->id );
                fixUpTable.push_back( nodePair );

                m_nodes.push_back(ui_node);
                ImNodes::SetNodeGridSpacePos(ui_node->id, nodePos);
                
                infile >> op->m_red[0] >> op->m_red[1] >> op->m_red[2] >> op->m_red[3] >> op->m_red[4];
                infile >> op->m_green[0] >> op->m_green[1] >> op->m_green[2] >> op->m_green[3] >> op->m_green[4];
                infile >> op->m_blue[0] >> op->m_blue[1] >> op->m_blue[2] >> op->m_blue[3] >> op->m_blue[4];
            }
            break;
        case NodeType::OUTPUT:
            {
                Node *value = new Node(NodeType::VALUE);
                Node *out = new Node(NodeType::OUTPUT);

                UiOutputNode *ui_node = new UiOutputNode();
                ui_node->type = NodeType::OUTPUT;

                int oldInput;
                infile >> oldInput;
                ui_node->ui.one.input = m_graph.insert_node(value);
                auto inpair = std::make_pair( oldInput , ui_node->ui.one.input );
                fixUpTable.push_back( inpair );

                //Don't need to store old id, but need the pair for linking later.
                ui_node->id = m_graph.insert_node(out);
                auto nodePair = std::make_pair( nodeId, ui_node->id );
                fixUpTable.push_back( nodePair );

                m_nodes.push_back(ui_node);
                ImNodes::SetNodeGridSpacePos(ui_node->id, nodePos);
                //set the root node
                m_rootNodeId = ui_node->id;
            }
            break;
        case NodeType::UNIFORM_COLOR: 
            {
                UniformColorNode *op = new UniformColorNode();

                UiUniformColorNode *ui_node = new UiUniformColorNode();
                ui_node->type = NodeType::UNIFORM_COLOR;

                //Don't need to store old id, but need the pair for linking later.
                ui_node->id = m_graph.insert_node(op);
                auto nodePair = std::make_pair( nodeId, ui_node->id );
                fixUpTable.push_back( nodePair );

                m_nodes.push_back(ui_node);
                ImNodes::SetNodeGridSpacePos(ui_node->id, nodePos);

                infile >> op->m_color.asPart.red >> op->m_color.asPart.green >> op->m_color.asPart.blue >> op->m_color.asPart.alpha;
            }
            break;
        case NodeType::NOISE: 
            {
                NoiseNode *op = new NoiseNode();

                UiNoiseNode *ui_node = new UiNoiseNode();
                ui_node->type = NodeType::NOISE;

                //Don't need to store old id, but need the pair for linking later.
                ui_node->id = m_graph.insert_node(op);
                auto nodePair = std::make_pair( nodeId, ui_node->id );
                fixUpTable.push_back( nodePair );

                m_nodes.push_back(ui_node);
                ImNodes::SetNodeGridSpacePos(ui_node->id, nodePos);

                int noiseModeRaw;
                infile >> noiseModeRaw;
                op->m_mode = static_cast<NoiseCompute::NoiseMode>( noiseModeRaw );

                infile >> op->m_seed;
                infile >> op->m_scale;
                //TODO draperdanman: other values
            }
            break;
        case NodeType::IMAGE:
            {
                int strLen;
                infile >> strLen;
                //char buffer[1024]; //longer than the max filepath in windows
                //infile.read( buffer, strLen );
                std::string imgFile;
                infile >> imgFile;
                
                std::shared_ptr<Image> img = std::make_shared<Image>( imgFile );
                ImageNode *op = new ImageNode( img );

                UiImageNode *ui_node = new UiImageNode();
                ui_node->type = NodeType::IMAGE;

                //Don't need to store old id, but need the pair for linking later.
                ui_node->id = m_graph.insert_node(op);
                auto nodePair = std::make_pair( nodeId, ui_node->id );
                fixUpTable.push_back( nodePair );

                m_nodes.push_back(ui_node);
                ImNodes::SetNodeGridSpacePos(ui_node->id, nodePos);
            }
            break;
        case NodeType::DYNAMIC_IMAGE:
            {
                int strLen;
                infile >> strLen;

                std::string folderPath;
                infile >> folderPath;
                
                DynamicImageNode *op = new DynamicImageNode( folderPath );

                UiDynamicImageNode *ui_node = new UiDynamicImageNode();
                ui_node->type = NodeType::DYNAMIC_IMAGE;

                //Don't need to store old id, but need the pair for linking later.
                ui_node->id = m_graph.insert_node(op);
                auto nodePair = std::make_pair( nodeId, ui_node->id );
                fixUpTable.push_back( nodePair );

                m_nodes.push_back(ui_node);
                ImNodes::SetNodeGridSpacePos(ui_node->id, nodePos);
            }
            break;
        }
    }

    int edgeTotal;
    infile >> edgeTotal;

    auto findFixup = [&fixUpTable](int id) -> int
    {
        for( const auto [fst, snd] : fixUpTable )
        {
            if (fst == id)
            {
                return snd;
            }
        }
        return -1;
    };
    
    for (int i = 0; i < edgeTotal; ++i)
    {
        int from, to;
        infile >> from >> to;
        m_graph.insert_edge( findFixup(from), findFixup(to) );
    }

    if (m_rootNodeId != -1)
    {
        m_outputImage = Evaluate(m_graph, m_rootNodeId);
        Node *node = m_graph.node(m_rootNodeId);
        node->value = m_outputImage;
        m_graph.update_node( m_rootNodeId, node );
    }
    
    infile.close();
}


void NodeCanvas::ClearProject()
{
    ImNodes::ClearLinkSelection();
    ImNodes::ClearNodeSelection();
    for ( const auto node : m_graph.nodes())
    {
        delete node;
    }
    m_graph = Graph<Node *>();
    m_nodes.clear();
    m_rootNodeId = -1;
}

}