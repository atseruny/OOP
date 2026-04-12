#pragma once

#include "Node.hpp"

#include <vector>

class BlockNode : public Node
{
public:
    std::vector<std::unique_ptr<Node>> statements;

    BlockNode() : Node(NodeType::Block) {}
};