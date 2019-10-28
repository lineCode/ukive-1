#ifndef UKIVE_GRAPHICS_DIRECT3D_SCENE_H_
#define UKIVE_GRAPHICS_DIRECT3D_SCENE_H_


namespace ukive {

    class InputEvent;
    class Direct3DView;

    class Scene {
    public:
        virtual ~Scene() = default;

        virtual void onSceneCreate(Direct3DView* d3d_view) {}
        virtual bool onSceneInput(InputEvent* e) { return false; }
        virtual void onSceneResize(int width, int height) {}
        virtual void onSceneRender() {}
        virtual void onSceneDestroy() {}
    };

}

#endif  // UKIVE_GRAPHICS_DIRECT3D_SCENE_H_