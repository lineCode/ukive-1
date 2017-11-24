#include "qtree_node.h"


namespace shell {

    QTreeNode::QTreeNode()
    {
        level = 0;
        indexX = -1;
        indexY = -1;
        rough = 0.f;

        child[0] = nullptr;
        child[1] = nullptr;
        child[2] = nullptr;
        child[3] = nullptr;

        next = nullptr;
    }

    QTreeNode::~QTreeNode()
    {
    }

}