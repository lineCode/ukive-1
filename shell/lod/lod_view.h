#ifndef SHELL_LOD_LOD_VIEW_H_
#define SHELL_LOD_LOD_VIEW_H_

#include "ukive/views/direct3d_view.h"


namespace ukive {
    class Window;
    class InputEvent;
}

namespace shell {

    class TerrainScene;

    class LodView : public ukive::Direct3DView {
    public:
        LodView(ukive::Window *w);
        ~LodView();

        void onDirect3DRender() override;

        TerrainScene *getTerrainScene();

    protected:
        bool onInputEvent(ukive::InputEvent *e) override;

        void onSizeChanged(
            int width, int height, int oldWidth, int oldHeight) override;

    private:
        void initLodView();

        TerrainScene *mScene;
    };

}

#endif  // SHELL_LOD_LOD_VIEW_H_