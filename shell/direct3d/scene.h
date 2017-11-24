#ifndef SHELL_DIRECT3D_SCENE_H_
#define SHELL_DIRECT3D_SCENE_H_

#include "shell/third_party/directx_math/Inc/DirectXMath.h"


namespace ukive {
    class InputEvent;
    class Direct3DView;
    class DrawingObjectManager;
}

namespace shell {

    namespace dx = DirectX;

    class Camera;
    class GraphCreator;

    class Scene
    {
    private:
        int mPrevX;
        int mPrevY;

    private:
        unsigned int mWidth;
        unsigned int mHeight;

        Camera *mCamera;
        GraphCreator *mGraphCreator;
        ukive::DrawingObjectManager *mDrawingObjectManager;
        ukive::Direct3DView *mD3DView;

    public:
        Scene(ukive::Direct3DView *d3dView,
            unsigned int width, unsigned int height);
        ~Scene();

        void refresh();
        void getPickLine(int sx, int sy, dx::XMVECTOR *lineOrig, dx::XMVECTOR *lineDir);

        virtual void onSceneCreate();
        virtual void onSceneInput(ukive::InputEvent *e);
        virtual void onSceneResize(unsigned int width, unsigned int height);
        virtual void onSceneRender() = 0;
        virtual void onSceneDestroy();

        unsigned int getSceneWidth();
        unsigned int getSceneHeight();

        Camera *getCamera();
        GraphCreator *getGraphCreator();
        ukive::DrawingObjectManager *getDrawingObjectManager();
        ukive::Direct3DView *getView();
    };

}

#endif  // SHELL_DIRECT3D_SCENE_H_