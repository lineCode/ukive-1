#include "terrain_scene.h"

#include "ukive/event/input_event.h"
#include "ukive/graphics/direct3d/space.h"
#include "ukive/graphics/direct3d/drawing_object_manager.h"
#include "ukive/views/text_view.h"
#include "ukive/views/direct3d_view.h"
#include "ukive/window/window.h"
#include "ukive/log.h"

#include "shell/lod/lod_generator.h"
#include "shell/lod/terrain_configure.h"
#include "shell/direct3d/camera.h"


namespace {
    const int kNormalCube = 0;
    const int kWorldAxis = 1;
}


namespace shell {

    namespace dx = DirectX;

    TerrainScene::TerrainScene()
        : mPrevX(0),
          mPrevY(0),
          mWidth(0),
          mHeight(0),
          mAssistConfigure(nullptr),
          mModelConfigure(nullptr),
          mTerrainConfigure(nullptr),
          d3d_view_(nullptr) {

        mMouseActionMode = MOUSE_ACTION_NONE;
        mIsCtrlKeyPressed = false;
        mIsShiftKeyPressed = false;
        mIsMouseLeftKeyPressed = false;

        drawing_obj_mgr_ = new ukive::DrawingObjectManager();
        graph_creator_ = new GraphCreator(drawing_obj_mgr_);

        camera_ = new Camera(1, 1);
        mLodGenerator = new LodGenerator(8192, 5);

        mVertexBuffer = ukive::Space::createVertexBuffer(
            mLodGenerator->getVertices(), sizeof(TerrainVertexData),
            mLodGenerator->getVertexCount());

        mIndexBuffer = ukive::Space::createIndexBuffer(
            mLodGenerator->getIndices(),
            mLodGenerator->getMaxIndexCount());
    }

    TerrainScene::~TerrainScene() {
        delete mLodGenerator;

        delete drawing_obj_mgr_;
        delete graph_creator_;
        delete camera_;
    }


    void TerrainScene::setRenderListener(std::function<void()>&& l) {
        on_render_handler_ = l;
    }

    void TerrainScene::updateCube() {
        auto object = getDrawingObjectManager()->getByTag(kNormalCube);
        if (object) {
            D3D11_MAPPED_SUBRESOURCE source = ukive::Space::lockResource(object->vertexBuffer.get());
            if (source.pData) {
                auto locked_vd = reinterpret_cast<ModelVertexData*>(source.pData);
                auto object_vd = reinterpret_cast<ModelVertexData*>(object->vertices);

                object_vd[0].position.x += 0.1f;

                for (unsigned int i = 0; i < object->vertexCount; ++i) {
                    locked_vd[i].position = object_vd[i].position;
                    locked_vd[i].color = object_vd[i].color;
                    locked_vd[i].normal = object_vd[i].normal;
                    locked_vd[i].texcoord = object_vd[i].texcoord;
                }

                ukive::Space::unlockResource(object->vertexBuffer.get());
            }
        }
    }

    void TerrainScene::updateLodTerrain() {
        D3D11_MAPPED_SUBRESOURCE source
            = ukive::Space::lockResource(mIndexBuffer.get());
        if (source.pData) {
            int* indices = reinterpret_cast<int*>(source.pData);

            const dx::XMFLOAT3 *vPosition = getCamera()->getCameraPos();

            dx::XMFLOAT4X4 wvpMatrix;
            getCamera()->getWVPMatrix(&wvpMatrix);

            mLodGenerator->renderLodTerrain(*vPosition, wvpMatrix, indices);

            ukive::Space::unlockResource(mIndexBuffer.get());
        }
    }

    void TerrainScene::elementAwareness(int ex, int ey) {
        dx::XMVECTOR ori;
        dx::XMVECTOR dir;
        getPickLine(ex, ey, &ori, &dir);

        bool isHitVer = false;
        dx::XMVECTOR vPos;
        ukive::DrawingObjectManager::DrawingObject *object
            = getDrawingObjectManager()->getByTag(kNormalCube);
        if (object) {
            ModelVertexData *vData = (ModelVertexData*)object->vertices;
            for (unsigned int i = 0; i < object->vertexCount; ++i) {
                bool hit = false;
                dx::XMVECTOR pos = DirectX::XMLoadFloat3(&vData[i].position);
                dx::XMVECTOR distance = DirectX::XMVector3LinePointDistance(
                    ori, DirectX::XMVectorAdd(ori, dir), pos);
                if (DirectX::XMVectorGetX(distance) < 20.f) {
                    vPos = pos;
                    isHitVer = true;
                }
            }

            if (isHitVer) {
                //getGraphCreator()->putBlock(155, &vPos, 10.f);
                d3d_view_->invalidate();
            } else {
                if (getDrawingObjectManager()->contains(155)) {
                    getDrawingObjectManager()->removeByTag(155);
                    d3d_view_->invalidate();
                }
            }
        }
    }

    void TerrainScene::getPickLine(int sx, int sy, dx::XMVECTOR* lineOrig, dx::XMVECTOR* lineDir) {
        auto worldMatrix = camera_->getWorldMatrix();
        auto viewMatrix = camera_->getViewMatrix();
        auto projectionMatrix = camera_->getProjectionMatrix();

        auto vx = (2.0f * sx / mWidth - 1.0f) / projectionMatrix->_11;
        auto vy = (-2.0f * sy / mHeight + 1.0f) / projectionMatrix->_22;

        dx::XMVECTOR rayDir = dx::XMVectorSet(vx, vy, 1.0f, 0);
        dx::XMVECTOR rayOrigin = dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0);

        dx::XMMATRIX world = dx::XMLoadFloat4x4(worldMatrix);
        dx::XMMATRIX view = dx::XMLoadFloat4x4(viewMatrix);

        dx::XMMATRIX inverseView = dx::XMMatrixInverse(nullptr, view);
        rayDir = dx::XMVector3TransformNormal(rayDir, inverseView);
        rayOrigin = dx::XMVector3TransformCoord(rayOrigin, inverseView);

        dx::XMMATRIX inverseWorld = dx::XMMatrixInverse(nullptr, world);
        rayDir = dx::XMVector3TransformNormal(rayDir, inverseWorld);
        rayOrigin = dx::XMVector3TransformCoord(rayOrigin, inverseWorld);

        *lineOrig = rayOrigin;
        *lineDir = rayDir;
    }


    void TerrainScene::recreate(int level) {
        if (level > 0 && level != mLodGenerator->getLevel()) {
            delete mLodGenerator;

            mLodGenerator = new LodGenerator(8192, level);

            mVertexBuffer = ukive::Space::createVertexBuffer(
                mLodGenerator->getVertices(), sizeof(TerrainVertexData),
                mLodGenerator->getVertexCount());

            mIndexBuffer = ukive::Space::createIndexBuffer(
                mLodGenerator->getIndices(),
                mLodGenerator->getMaxIndexCount());
        }
    }

    void TerrainScene::reevaluate(float c1, float c2) {
        mLodGenerator->setCoefficient(c1, c2);
        updateLodTerrain();
    }


    void TerrainScene::onSceneCreate(ukive::Direct3DView* d3d_view) {
        d3d_view_ = d3d_view;

        getCamera()->setCameraPosition(1024, 1024, -1024);
        getCamera()->circuleCamera2(1.f, -0.2f);

        mAssistConfigure = new AssistConfigure();
        mAssistConfigure->init();

        mModelConfigure = new ModelConfigure();
        mModelConfigure->init();

        mTerrainConfigure = new TerrainConfigure();
        mTerrainConfigure->init();

        getGraphCreator()->putWorldAxis(kWorldAxis, 1024);

        updateLodTerrain();
    }


    void TerrainScene::onSceneResize(int width, int height) {
        Scene::onSceneResize(width, height);

        mWidth = width;
        mHeight = height;

        camera_->resize(width, height);

        updateLodTerrain();
    }


    void TerrainScene::onSceneInput(ukive::InputEvent *e) {
        Scene::onSceneInput(e);

        switch (e->getEvent()) {
        case ukive::InputEvent::EVM_DOWN:
            if (e->getMouseKey() == ukive::InputEvent::MK_LEFT) {
                mIsMouseLeftKeyPressed = true;
                mPrevX = e->getX();
                mPrevY = e->getY();
            }
            break;

        case ukive::InputEvent::EVM_MOVE:
            if (mIsMouseLeftKeyPressed == true) {
                mMouseActionMode = MOUSE_ACTION_MOVED;

                int dx = e->getX() - mPrevX;
                int dy = e->getY() - mPrevY;

                if (::GetKeyState(VK_SHIFT) < 0) {
                    getCamera()->circuleCamera2(
                        (float)-dx / mWidth,
                        (float)-dy / mHeight);

                    updateLodTerrain();
                    d3d_view_->invalidate();
                } else if (::GetKeyState(VK_CONTROL) < 0) {
                    getCamera()->moveCamera((float)-dx, (float)dy);

                    updateLodTerrain();
                    d3d_view_->invalidate();
                }

                mPrevX = e->getX();
                mPrevY = e->getY();
            } else {
                elementAwareness(e->getX(), e->getY());
            }
            break;

        case ukive::InputEvent::EVM_UP:
            if (e->getMouseKey() == ukive::InputEvent::MK_LEFT) {
                if (mMouseActionMode != MOUSE_ACTION_MOVED) {
                    updateCube();
                }

                mIsMouseLeftKeyPressed = false;
            }

            mMouseActionMode = MOUSE_ACTION_NONE;
            break;

        case ukive::InputEvent::EVK_DOWN:
            if (e->getKeyboardVirtualKey() == VK_SHIFT) {
                mIsShiftKeyPressed = true;
            }
            if (e->getKeyboardVirtualKey() == VK_CONTROL) {
                mIsCtrlKeyPressed = true;
            }
            break;

        case ukive::InputEvent::EVK_UP:
            if (e->getKeyboardVirtualKey() == VK_SHIFT) {
                mIsShiftKeyPressed = false;
            }
            if (e->getKeyboardVirtualKey() == VK_CONTROL) {
                mIsCtrlKeyPressed = false;
            }
            break;

        case ukive::InputEvent::EVM_WHEEL:
            getCamera()->scaleCamera(e->getMouseWheel() > 0 ? 0.9f : 1.1f);
            updateLodTerrain();
            d3d_view_->invalidate();
            break;
        default:
            break;
        }
    }

    void TerrainScene::onSceneRender() {
        on_render_handler_();

        dx::XMFLOAT4X4 wvpMatrix;
        dx::XMFLOAT4X4 wvpMatrixTransposed;

        getCamera()->getWVPMatrix(&wvpMatrix);
        DirectX::XMStoreFloat4x4(&wvpMatrixTransposed, DirectX::XMMatrixTranspose(
            DirectX::XMLoadFloat4x4(&wvpMatrix)));

        mAssistConfigure->active();
        mAssistConfigure->setMatrix(wvpMatrixTransposed);

        ukive::Space::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
        ukive::Space::drawObjects(getDrawingObjectManager()->getByTag(kWorldAxis));

        mTerrainConfigure->active();
        mTerrainConfigure->setMatrix(wvpMatrixTransposed);

        ukive::Space::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        ukive::Space::draw(
            mVertexBuffer.get(), mIndexBuffer.get(),
            sizeof(TerrainVertexData), mLodGenerator->getIndexCount());

        //ukive::Renderer::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //ukive::Renderer::drawObjects(getDrawingObjectManager()->getByTag(155));

        //mModelLightEngine->active();
        //mModelLightEngine->setMatrix(wvpMatrixTransposed);

        //ukive::Renderer::setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //ukive::Renderer::drawObjects(getDrawingObjectManager()->getByTag(U3DGraphId::NORMAL_CUBE));
    }

    void TerrainScene::onSceneDestroy() {
        Scene::onSceneDestroy();

        if (mAssistConfigure) {
            mAssistConfigure->close();
            delete mAssistConfigure;
            mAssistConfigure = nullptr;
        }

        if (mModelConfigure) {
            mModelConfigure->close();
            delete mModelConfigure;
            mModelConfigure = nullptr;
        }

        if (mTerrainConfigure) {
            mTerrainConfigure->close();
            delete mTerrainConfigure;
            mTerrainConfigure = nullptr;
        }
    }

    Camera* TerrainScene::getCamera() const {
        return camera_;
    }

    GraphCreator* TerrainScene::getGraphCreator() const {
        return graph_creator_;
    }

    ukive::DrawingObjectManager* TerrainScene::getDrawingObjectManager() const {
        return drawing_obj_mgr_;
    }

    LodGenerator* TerrainScene::getLodGenerator() const {
        return mLodGenerator;
    }

}