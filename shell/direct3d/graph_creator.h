#ifndef SHELL_DIRECT3D_GRAPH_CREATOR_H_
#define SHELL_DIRECT3D_GRAPH_CREATOR_H_

#include <Windows.h>

#include "shell/third_party/directx_math/Inc/DirectXMath.h"
#include "shell/direct3d/assist_configure.h"
#include "shell/direct3d/model_configure.h"


namespace ukive {
    class DrawingObjectManager;
}

namespace shell {

    namespace dx = DirectX;

    struct ModelVertexData;

    class GraphCreator
    {
    private:
        ukive::DrawingObjectManager* mDrawingObjectManager;

    public:
        GraphCreator(ukive::DrawingObjectManager* doMgr);
        ~GraphCreator();

        void calculateNormalVector(ModelVertexData *vertices, int vertexCount, int *indices, int indexCount);

        HRESULT putLine(dx::XMFLOAT3 *point1, dx::XMFLOAT3 *point2, int tag);

        HRESULT putCube(int tag, float edgeLength);
        HRESULT putWorldAxis(int tag, float length);
        HRESULT putMark(int tag, dx::XMFLOAT3 *mark, float length);
        HRESULT putBlock(int tag, dx::XMFLOAT3 *posCenter, float radius);
    };

}

#endif  // SHELL_DIRECT3D_GRAPH_CREATOR_H_