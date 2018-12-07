#ifndef SHELL_LOD_TERRAIN_SCENE_H_
#define SHELL_LOD_TERRAIN_SCENE_H_

#include <functional>

#include "ukive/graphics/direct3d/scene.h"

#include "shell/direct3d/graph_creator.h"
#include "shell/third_party/directx_math/Inc/DirectXMath.h"


namespace ukive {
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

        void setRenderListener(std::function<void()>&& l);

        void recreate(int level);
        void reevaluate(float c1, float c2);

        void onSceneCreate(ukive::Direct3DView* d3d_view) override;
        void onSceneResize(int width, int height) override;
        void onSceneInput(ukive::InputEvent* e) override;
        void onSceneRender() override;
        void onSceneDestroy() override;

        Camera* getCamera() const;
        GraphCreator* getGraphCreator() const;
        ukive::DrawingObjectManager* getDrawingObjectManager() const;
        LodGenerator* getLodGenerator() const;

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
        int mWidth;
        int mHeight;

        int mMouseActionMode;
        bool mIsCtrlKeyPressed;
        bool mIsShiftKeyPressed;
        bool mIsMouseLeftKeyPressed;

        LodGenerator* mLodGenerator;
        AssistConfigure* mAssistConfigure;
        ModelConfigure* mModelConfigure;
        TerrainConfigure* mTerrainConfigure;

        ukive::ComPtr<ID3D11Buffer> mIndexBuffer;
        ukive::ComPtr<ID3D11Buffer> mVertexBuffer;

        Camera* camera_;
        GraphCreator* graph_creator_;

        ukive::Direct3DView* d3d_view_;
        ukive::DrawingObjectManager* drawing_obj_mgr_;

        std::function<void()> on_render_handler_;
    };

}

#endif  // SHELL_LOD_TERRAIN_SCENE_H_