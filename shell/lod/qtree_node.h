#ifndef SHELL_LOD_QTREE_NODE_H_
#define SHELL_LOD_QTREE_NODE_H_

#include "shell/third_party/directx_math/Inc/DirectXMath.h"


namespace shell {

    namespace dx = DirectX;

    class QTreeNode
    {
    public:
        int level;
        int indexX;
        int indexY;
        float rough;
        dx::XMFLOAT3 mincoord;
        dx::XMFLOAT3 maxcoord;
        QTreeNode *child[4];

        QTreeNode *next;

    public:
        QTreeNode();
        ~QTreeNode();
    };

}

#endif  // SHELL_LOD_QTREE_NODE_H_