#include "lod_view.h"

#include "ukive/application.h"
#include "ukive/window/window.h"

#include "shell/lod/terrain_scene.h"


namespace shell {

    LodView::LodView(ukive::Window *w)
        :Direct3DView(w) {
        initLodView();
    }

    LodView::~LodView() {
        mScene->onSceneDestroy();
        delete mScene;
    }


    void LodView::onDirect3DRender() {
        if (isAttachedToWindow())
            mScene->onSceneRender();
    }

    bool LodView::onInputEvent(ukive::InputEvent *e) {
        mScene->onSceneInput(e);
        return Direct3DView::onInputEvent(e);
    }

    void LodView::onSizeChanged(
        int width, int height, int oldWidth, int oldHeight) {

        mScene->onSceneResize(width, height);
    }

    TerrainScene *LodView::getTerrainScene()
    {
        return mScene;
    }


    void LodView::initLodView() {
        mScene = new TerrainScene(this, 1, 1);
        mScene->onSceneCreate();
    }
}