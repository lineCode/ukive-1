#ifndef SHELL_LOD_TERRAIN_SCENE_H_
#define SHELL_LOD_TERRAIN_SCENE_H_

#include "ukive/graphics/direct3d/scene.h"

#include "shell/direct3d/graph_creator.h"
#include "shell/third_party/directx_math/Inc/DirectXMath.h"


namespace ukive {
    class TextView;
    class InputEvent;
    class Direct3DView;
    class DrawingObjectManager;
}

namespace shell {

    class Camera;
    class GraphCreator;
    class LodGenerator;
    class AssistConfigure;
    class ModelConfigure;
    class TerrainConfigure;

    class TerrainScene : public ukive::Scene
    {
    private:
        ukive::TextView *mLodInfoTV;
        ID3D11Buffer* mIndexBuffer;
        ID3D11Buffer* mVertexBuffer;

        Camera *mCamera;
        GraphCreator *mGraphCreator;

        ukive::Direct3DView* d3d_view_;
        ukive::DrawingObjectManager *mDrawingObjectManager;

    private:
        int mPrevX;
        int mPrevY;
        int mFrameCounter;
        int mFramePreSecond;
        ULONG64 mPrevTime;

        unsigned int mWidth;
        unsigned int mHeight;

        int mMouseActionMode;
        bool mIsCtrlKeyPressed;
        bool mIsShiftKeyPressed;
        bool mIsMouseLeftKeyPressed;

        const static int MOUSE_ACTION_NONE = 1;
        const static int MOUSE_ACTION_MOVED = 2;
        const static int MOUSE_ACTION_MOVING = 3;

    private:
        LodGenerator *mLodGenerator;
        AssistConfigure *mAssistConfigure;
        ModelConfigure *mModelConfigure;
        TerrainConfigure *mTerrainConfigure;

        void updateCube();
        void updateLodTerrain();
        void elementAwareness(int ex, int ey);
        void getPickLine(int sx, int sy, dx::XMVECTOR *lineOrig, dx::XMVECTOR *lineDir);

    public:
        TerrainScene();
        ~TerrainScene();

        void recreate(int level);
        void reevaluate(float c1, float c2);

        virtual void onSceneCreate(ukive::Direct3DView* d3d_view) override;
        virtual void onSceneResize(unsigned int width, unsigned int height) override;
        virtual void onSceneInput(ukive::InputEvent *e) override;
        virtual void onSceneRender() override;
        virtual void onSceneDestroy() override;

        Camera *getCamera();
        GraphCreator *getGraphCreator();
        ukive::DrawingObjectManager *getDrawingObjectManager();
    };

}

#endif  // SHELL_LOD_TERRAIN_SCENE_H_