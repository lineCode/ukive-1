#include "lod_view.h"

#include "ukive/application.h"

#include "shell/lod/terrain_scene.h"


namespace shell {

    LodView::LodView(ukive::Window *wnd)
        :Direct3DView(wnd) {
        initLodView();
    }

    LodView::LodView(ukive::Window *wnd, int id)
        : Direct3DView(wnd, id) {
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

        mScene->onSceneResize(
            ukive::Application::dpToPxX(width),
            ukive::Application::dpToPxY(height));
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