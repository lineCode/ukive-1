#ifndef SHELL_LOD_TERRAIN_SCENE_H_
#define SHELL_LOD_TERRAIN_SCENE_H_

#include "shell/direct3d/scene.h"
#include "shell/direct3d/graph_creator.h"


namespace ukive {
    class TextView;
    class InputEvent;
}

namespace shell {

    class LodGenerator;
    class AssistConfigure;
    class ModelConfigure;
    class TerrainConfigure;

    class TerrainScene : public Scene
    {
    private:
        ukive::TextView *mLodInfoTV;
        ID3D11Buffer* mIndexBuffer;
        ID3D11Buffer* mVertexBuffer;

    private:
        int mPrevX;
        int mPrevY;
        int mFrameCounter;
        int mFramePreSecond;
        ULONG64 mPrevTime;

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

    public:
        TerrainScene(ukive::Direct3DView *d3dView,
            unsigned int width, unsigned int height);
        ~TerrainScene();

        void recreate(int level);
        void reevaluate(float c1, float c2);

        virtual void onSceneCreate() override;
        virtual void onSceneResize(unsigned int width, unsigned int height) override;
        virtual void onSceneInput(ukive::InputEvent *e) override;
        virtual void onSceneRender() override;
        virtual void onSceneDestroy() override;
    };

}

#endif  // SHELL_LOD_TERRAIN_SCENE_H_