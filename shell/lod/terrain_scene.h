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

    class TerrainScene : public ukive::Scene {
    public:
        TerrainScene();
        ~TerrainScene();

        void recreate(int level);
        void reevaluate(float c1, float c2);

        void onSceneCreate(ukive::Direct3DView* d3d_view) override;
        void onSceneResize(unsigned int width, unsigned int height) override;
        void onSceneInput(ukive::InputEvent* e) override;
        void onSceneRender() override;
        void onSceneDestroy() override;

        Camera* getCamera();
        GraphCreator* getGraphCreator();
        ukive::DrawingObjectManager* getDrawingObjectManager();

    private:
        enum {
            MOUSE_ACTION_NONE = 1,
            MOUSE_ACTION_MOVED = 2,
            MOUSE_ACTION_MOVING = 3,
        };

        void updateCube();
        void updateLodTerrain();
        void elementAwareness(int ex, int ey);
        void getPickLine(int sx, int sy, dx::XMVECTOR* lineOrig, dx::XMVECTOR* lineDir);

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

        LodGenerator* mLodGenerator;
        AssistConfigure* mAssistConfigure;
        ModelConfigure* mModelConfigure;
        TerrainConfigure* mTerrainConfigure;

        ukive::TextView* mLodInfoTV;
        ukive::ComPtr<ID3D11Buffer> mIndexBuffer;
        ukive::ComPtr<ID3D11Buffer> mVertexBuffer;

        Camera* mCamera;
        GraphCreator* mGraphCreator;

        ukive::Direct3DView* d3d_view_;
        ukive::DrawingObjectManager* mDrawingObjectManager;
    };

}

#endif  // SHELL_LOD_TERRAIN_SCENE_H_