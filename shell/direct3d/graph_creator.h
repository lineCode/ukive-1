#ifndef SHELL_DIRECT3D_GRAPH_CREATOR_H_
#define SHELL_DIRECT3D_GRAPH_CREATOR_H_

#include <Windows.h>

#include "shell/direct3d/assist_configure.h"
#include "shell/direct3d/model_configure.h"
#include "shell/third_party/directx_math/Inc/DirectXMath.h"


namespace ukive {
    class DrawingObjectManager;
}

namespace shell {

    namespace dx = DirectX;

    struct ModelVertexData;

    class GraphCreator {
    public:
        GraphCreator(ukive::DrawingObjectManager* mgr);
        ~GraphCreator();

        void calculateNormalVector(ModelVertexData* vertices, int vertexCount, int* indices, int indexCount);

        void putLine(dx::XMFLOAT3* point1, dx::XMFLOAT3* point2, int tag);
        void putCube(int tag, float edgeLength);
        void putWorldAxis(int tag, float length);
        void putMark(int tag, dx::XMFLOAT3* mark, float length);
        void putBlock(int tag, dx::XMFLOAT3* posCenter, float radius);

    private:
        ukive::DrawingObjectManager* drawing_obj_mgr_;
    };

}

#endif  // SHELL_DIRECT3D_GRAPH_CREATOR_H_