#include "terrain_scene.h"

#include <sstream>

#include "ukive/event/input_event.h"
#include "ukive/graphics/direct3d/space.h"
#include "ukive/graphics/direct3d/drawing_object_manager.h"
#include "ukive/views/text_view.h"
#include "ukive/views/direct3d_view.h"
#include "ukive/window/window.h"
#include "ukive/system/system_clock.h"

#include "shell/lod/lod_generator.h"
#include "shell/lod/terrain_configure.h"
#include "shell/direct3d/camera.h"


namespace {
    const int kNormalCube = 0;
    const int kWorldAxis = 1;
}


namespace shell {

    namespace dx = DirectX;

    TerrainScene::TerrainScene() {
        mPrevTime = 0;
        mFrameCounter = 0;
        mFramePreSecond = 0;
        mLodInfoTV = nullptr;

        mMouseActionMode = MOUSE_ACTION_NONE;
        mIsCtrlKeyPressed = false;
        mIsShiftKeyPressed = false;
        mIsMouseLeftKeyPressed = false;

        mDrawingObjectManager = new ukive::DrawingObjectManager();

        mGraphCreator = new GraphCreator(mDrawingObjectManager);

        mCamera = new Camera();
        mCamera->init(1, 1);

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

        delete mDrawingObjectManager;
        delete mGraphCreator;
        delete mCamera;
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

    void TerrainScene::getPickLine(int sx, int sy, dx::XMVECTOR *lineOrig, dx::XMVECTOR *lineDir) {
        float vx, vy;
        const dx::XMFLOAT4X4 *worldMatrix;
        const dx::XMFLOAT4X4 *viewMatrix;
        const dx::XMFLOAT4X4 *projectionMatrix;

        worldMatrix = mCamera->getWorldMatrix();
        viewMatrix = mCamera->getViewMatrix();
        projectionMatrix = mCamera->getProjectionMatrix();

        vx = (2.0f * sx / mWidth - 1.0f) / projectionMatrix->_11;
        vy = (-2.0f * sy / mHeight + 1.0f) / projectionMatrix->_22;

        dx::XMVECTOR rayDir = dx::XMVectorSet(vx, vy, 1.0f, 0);
        dx::XMVECTOR rayOrigin = dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0);

        dx::XMMATRIX world = dx::XMLoadFloat4x4(worldMatrix);
        dx::XMMATRIX view = dx::XMLoadFloat4x4(viewMatrix);

        dx::XMMATRIX inverseView = dx::XMMatrixInverse(0, view);
        rayDir = dx::XMVector3TransformNormal(rayDir, inverseView);
        rayOrigin = dx::XMVector3TransformCoord(rayOrigin, inverseView);

        dx::XMMATRIX inverseWorld = dx::XMMatrixInverse(0, world);
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
        HRESULT hr = mAssistConfigure->init();
        if (FAILED(hr))
            throw std::runtime_error("");

        mModelConfigure = new ModelConfigure();
        hr = mModelConfigure->init();
        if (FAILED(hr))
            throw std::runtime_error("");

        mTerrainConfigure = new TerrainConfigure();
        hr = mTerrainConfigure->init();
        if (FAILED(hr))
            throw std::runtime_error("");

        getGraphCreator()->putWorldAxis(kWorldAxis, 1024);

        updateLodTerrain();
    }


    void TerrainScene::onSceneResize(unsigned int width, unsigned int height) {
        Scene::onSceneResize(width, height);

        mWidth = width;
        mHeight = height;

        mCamera->resize(width, height);

        updateLodTerrain();
    }


    void TerrainScene::onSceneInput(ukive::InputEvent *e) {
        Scene::onSceneInput(e);

        switch (e->getEvent()) {
        case ukive::InputEvent::EVM_DOWN:
            if (e->getMouseKey() == ukive::InputEvent::MK_LEFT) {
                mIsMouseLeftKeyPressed = true;
                mPrevX = e->getMouseX();
                mPrevY = e->getMouseY();
            }
            break;

        case ukive::InputEvent::EVM_MOVE:
            if (mIsMouseLeftKeyPressed == true) {
                mMouseActionMode = MOUSE_ACTION_MOVED;

                int dx = e->getMouseX() - mPrevX;
                int dy = e->getMouseY() - mPrevY;

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

                mPrevX = e->getMouseX();
                mPrevY = e->getMouseY();
            } else {
                elementAwareness(e->getMouseX(), e->getMouseY());
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
            if (e->getKeyboardKey() == VK_SHIFT) {
                mIsShiftKeyPressed = true;
            }
            if (e->getKeyboardKey() == VK_CONTROL) {
                mIsCtrlKeyPressed = true;
            }
            break;

        case ukive::InputEvent::EVK_UP:
            if (e->getKeyboardKey() == VK_SHIFT) {
                mIsShiftKeyPressed = false;
            }
            if (e->getKeyboardKey() == VK_CONTROL) {
                mIsCtrlKeyPressed = false;
            }
            break;

        case ukive::InputEvent::EVM_WHEEL:
            getCamera()->scaleCamera(e->getMouseWheel() > 0 ? 0.9f : 1.1f);
            updateLodTerrain();
            d3d_view_->invalidate();
            break;
        }
    }

    void TerrainScene::onSceneRender() {
        ULONG64 currentTime = ukive::SystemClock::upTimeMillis();
        if (mPrevTime > 0) {
            ++mFrameCounter;
            if (currentTime - mPrevTime > 500) {
                mFramePreSecond = (int)(((double)mFrameCounter / (currentTime - mPrevTime)) * 1000);
                mFrameCounter = 0;
                mPrevTime = currentTime;
            }
        } else {
            mPrevTime = currentTime;
        }

        std::wstringstream ss;
        ss << "FPS: " << mFramePreSecond
            << "\nTerrain Size: " << mLodGenerator->getRowVertexCount()
            << "x" << mLodGenerator->getRowVertexCount()
            << "\nTriangle Count: " << mLodGenerator->getMaxIndexCount() / 3
            << "\nRendered Triangle Count: " << mLodGenerator->getIndexCount() / 3;

        if (mLodInfoTV) {
            mLodInfoTV->setText(ss.str());
        } else {
            mLodInfoTV = (ukive::TextView*)d3d_view_->getWindow()->findViewById(0x010);
        }

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

    Camera* TerrainScene::getCamera() {
        return mCamera;
    }

    GraphCreator* TerrainScene::getGraphCreator() {
        return mGraphCreator;
    }

    ukive::DrawingObjectManager* TerrainScene::getDrawingObjectManager() {
        return mDrawingObjectManager;
    }

}