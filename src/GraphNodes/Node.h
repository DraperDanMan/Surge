#pragma once

#include <memory>
#include <stack>
#include <string>

#include "../Image.h"

namespace Surge
{
    
enum class NodeType
{
    BLEND,
    HSL,
    OUTPUT,
    LEVELS,
    CURVES,
    BLUR,
    INVERT,
    TRANSFORM,
    VALUE,
    UNIFORM_COLOR,
    NOISE,
    IMAGE,
    DYNAMIC_IMAGE,
};

struct Node
{
    std::string name = "Unknown";
    NodeType type;
    std::shared_ptr<Image> value;

    explicit Node(const NodeType t);
    Node(const NodeType t, const std::shared_ptr<Image> &val);
    virtual ~Node() = default;

    virtual std::shared_ptr<Image> Evaluate(std::stack<std::shared_ptr<Image>> &value_stack);
    virtual bool RenderProperties();
};

struct UiNode
{
    NodeType type;
    
    // The identifying id of the ui node within the graph.
    int id;

    union
    {
        struct
        {
            int lhs, rhs;
        } two;

        struct
        {
            int input;
        } one;
        
    } ui;

    virtual void RenderNode( Node* node ) const = 0;
    
protected:
    virtual ~UiNode() = default;
};

struct InputHeaderStyleJanitor
{
    InputHeaderStyleJanitor();
    ~InputHeaderStyleJanitor();
};

struct ModifyHeaderStyleJanitor
{
    ModifyHeaderStyleJanitor();
    ~ModifyHeaderStyleJanitor();
};

struct OutputHeaderStyleJanitor
{
    OutputHeaderStyleJanitor();
    ~OutputHeaderStyleJanitor();
};
    
}