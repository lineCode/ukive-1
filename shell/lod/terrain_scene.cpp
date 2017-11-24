﻿#include "terrain_scene.h"

#include <sstream>

#include "ukive/event/input_event.h"
#include "ukive/graphics/renderer.h"
#include "ukive/graphics/drawing_object_manager.h"
#include "ukive/views/text_view.h"
#include "ukive/views/direct3d_view.h"
#include "ukive/window/window.h"
#include "ukive/system/system_clock.h"

#include "shell/lod/lod_generator.h"
#include "shell/lod/terrain_configure.h"
#include "shell/direct3d/camera.h"
#include "shell/third_party/directx_math/Inc/DirectXMath.h"


namespace {
    const int kNormalCube = 0;
    const int kWorldAxis = 1;
}


namespace shell {

    namespace dx = DirectX;

    TerrainScene::TerrainScene(ukive::Direct3DView *d3dView,
        unsigned int width, unsigned int height)
        :Scene(d3dView, width, height)
    {
        mPrevTime = 0;
        mFrameCounter = 0;
        mFramePreSecond = 0;
        mLodInfoTV = nullptr;

        mMouseActionMode = MOUSE_ACTION_NONE;
        mIsCtrlKeyPressed = false;
        mIsShiftKeyPressed = false;
        mIsMouseLeftKeyPressed = false;

        mLodGenerator = new LodGenerator(8192, 5);

        HRESULT hr = ukive::Renderer::createVertexBuffer(
            mLodGenerator->getVertices(), sizeof(TerrainVertexData),
            mLodGenerator->getVertexCount(), mVertexBuffer);
        if (FAILED(hr))
            throw std::runtime_error("");

        hr = ukive::Renderer::createIndexBuffer(
            mLodGenerator->getIndices(),
            mLodGenerator->getMaxIndexCount(), mIndexBuffer);
        if (FAILED(hr))
            throw std::runtime_error("");
    }

    TerrainScene::~TerrainScene()
    {
        mIndexBuffer->Release();
        mVertexBuffer->Release();
        delete mLodGenerator;
    }


    void TerrainScene::updateCube()
    {
        auto *object = this->getDrawingObjectManager()
            ->getByTag(kNormalCube);
        if (object != nullptr)
        {
            D3D11_MAPPED_SUBRESOURCE source = ukive::Renderer::lockResource(object->vertexBuffer);
            if (source.pData != nullptr)
            {
                auto *modelVertexData = (ModelVertexData*)source.pData;

                ((ModelVertexData*)object->vertices)[0].position.x += 0.1f;

                for (unsigned int i = 0; i < object->vertexCount; ++i)
                {
                    modelVertexData[i].position = ((ModelVertexData*)object->vertices)[i].position;
                    modelVertexData[i].color = ((ModelVertexData*)object->vertices)[i].color;
                    modelVertexData[i].normal = ((ModelVertexData*)object->vertices)[i].normal;
                    modelVertexData[i].texcoord = ((ModelVertexData*)object->vertices)[i].texcoord;
                }

                ukive::Renderer::unlockResource(object->vertexBuffer);
            }
        }
    }

    void TerrainScene::updateLodTerrain()
    {
        D3D11_MAPPED_SUBRESOURCE source
            = ukive::Renderer::lockResource(mIndexBuffer);
        if (source.pData != nullptr)
        {
            int* indices = (int*)source.pData;

            const dx::XMFLOAT3 *vPosition = getCamera()->getCameraPos();

            dx::XMFLOAT4X4 wvpMatrix;
            getCamera()->getWVPMatrix(&wvpMatrix);

            mLodGenerator->renderLodTerrain(*vPosition, wvpMatrix, indices);

            ukive::Renderer::unlockResource(mIndexBuffer);
        }
    }

    void TerrainScene::elementAwareness(int ex, int ey)
    {
        dx::XMVECTOR ori;
        dx::XMVECTOR dir;
        getPickLine(ex, ey, &ori, &dir);

        bool isHitVer = false;
        dx::XMVECTOR vPos;
        ukive::DrawingObjectManager::DrawingObject *object
            = getDrawingObjectManager()->getByTag(kNormalCube);
        if (object)
        {
            ModelVertexData *vData = (ModelVertexData*)object->vertices;
            for (unsigned int i = 0; i < object->vertexCount; ++i)
            {
                bool hit = false;
                dx::XMVECTOR pos = DirectX::XMLoadFloat3(&vData[i].position);
                dx::XMVECTOR distance = DirectX::XMVector3LinePointDistance(
                    ori, DirectX::XMVectorAdd(ori, dir), pos);
                if (DirectX::XMVectorGetX(distance) < 20.f)
                {
                    vPos = pos;
                    isHitVer = true;
                }
            }

            if (isHitVer)
            {
                //getGraphCreator()->putBlock(155, &vPos, 10.f);
                refresh();
            }
            else
            {
                if (getDrawingObjectManager()->contains(155))
                {
                    getDrawingObjectManager()->removeByTag(155);
                    refresh();
                }
            }
        }
    }


    void TerrainScene::recreate(int level)
    {
        if (level > 0 && level != mLodGenerator->getLevel())
        {
            mIndexBuffer->Release();
            mVertexBuffer->Release();
            delete mLodGenerator;

            mLodGenerator = new LodGenerator(8192, level);

            HRESULT hr = ukive::Renderer::createVertexBuffer(
                mLodGenerator->getVertices(), sizeof(TerrainVertexData),
                mLodGenerator->getVertexCount(), mVertexBuffer);
            if (FAILED(hr))
                throw std::runtime_error("");

            hr = ukive::Renderer::createIndexBuffer(
                mLodGenerator->getIndices(),
                mLodGenerator->getMaxIndexCount(), mIndexBuffer);
            if (FAILED(hr))
                throw std::runtime_error("");
        }
    }

    void TerrainScene::reevaluate(float c1, float c2)
    {
        mLodGenerator->setCoefficient(c1, c2);
        updateLodTerrain();
    }


    void TerrainScene::onSceneCreate()
    {
        Scene::onSceneCreate();

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

        hr = getGraphCreator()->putWorldAxis(kWorldAxis, 1024);
        if (FAILED(hr))
            throw std::runtime_error("");

        updateLodTerrain();
    }


    void TerrainScene::onSceneResize(unsigned int width, unsigned int height)
    {
        Scene::onSceneResize(width, height);

        updateLodTerrain();
    }


    void TerrainScene::onSceneInput(ukive::InputEvent *e)
    {
        Scene::onSceneInput(e);

        switch (e->getEvent())
        {
        case ukive::InputEvent::EVM_DOWN:
            if (e->getMouseKey() == ukive::InputEvent::MK_LEFT)
            {
                mIsMouseLeftKeyPressed = true;
                mPrevX = e->getMouseX();
                mPrevY = e->getMouseY();
            }
            break;

        case ukive::InputEvent::EVM_MOVE:
            if (mIsMouseLeftKeyPressed == true)
            {
                mMouseActionMode = MOUSE_ACTION_MOVED;

                int dx = e->getMouseX() - mPrevX;
                int dy = e->getMouseY() - mPrevY;

                if (::GetKeyState(VK_SHIFT) < 0)
                {
                    getCamera()->circuleCamera2(
                        (float)-dx / this->getSceneWidth(),
                        (float)-dy / this->getSceneHeight());

                    updateLodTerrain();
                    refresh();
                }
                else if (::GetKeyState(VK_CONTROL) < 0)
                {
                    getCamera()->moveCamera((float)-dx, (float)dy);

                    updateLodTerrain();
                    refresh();
                }

                mPrevX = e->getMouseX();
                mPrevY = e->getMouseY();
            }
            else
                elementAwareness(e->getMouseX(), e->getMouseY());
            break;

        case ukive::InputEvent::EVM_UP:
            if (e->getMouseKey() == ukive::InputEvent::MK_LEFT)
            {
                if (mMouseActionMode != MOUSE_ACTION_MOVED)
                    updateCube();

                mIsMouseLeftKeyPressed = false;
            }

            mMouseActionMode = MOUSE_ACTION_NONE;
            break;

        case ukive::InputEvent::EVK_DOWN:
            if (e->getKeyboardKey() == VK_SHIFT)
                mIsShiftKeyPressed = true;
            if (e->getKeyboardKey() == VK_CONTROL)
                mIsCtrlKeyPressed = true;
            break;

        case ukive::InputEvent::EVK_UP:
            if (e->getKeyboardKey() == VK_SHIFT)
                mIsShiftKeyPressed = false;
            if (e->getKeyboardKey() == VK_CONTROL)
                mIsCtrlKeyPressed = false;
            break;

        case ukive::InputEvent::EVM_WHEEL:
            this->getCamera()->scaleCamera(e->getMouseWheel() > 0 ? 0.9f : 1.1f);
            updateLodTerrain();
            refresh();
            break;
        }
    }

    void TerrainScene::onSceneRender()
    {
        ULONG64 currentTime = ukive::SystemClock::upTimeMillis();
        if (mPrevTime > 0)
        {
            ++mFrameCounter;
            if (currentTime - mPrevTime > 500)
            {
                mFramePreSecond = (int)(((double)mFrameCounter / (currentTime - mPrevTime)) * 1000);
                mFrameCounter = 0;
                mPrevTime = currentTime;
            }
        }
        else
            mPrevTime = currentTime;

        std::wstringstream ss;
        ss << "FPS: " << mFramePreSecond
            << "\nTerrain Size: " << mLodGenerator->getRowVertexCount() << "x" << mLodGenerator->getRowVertexCount()
            << "\nTriangle Count: " << mLodGenerator->getMaxIndexCount() / 3
            << "\nRendered Triangle Count: " << mLodGenerator->getIndexCount() / 3;

        if (mLodInfoTV)
            mLodInfoTV->setText(ss.str());
        else
            mLodInfoTV = (ukive::TextView*)getView()->getWindow()->findViewById(0x010);


        dx::XMFLOAT4X4 wvpMatrix;
        dx::XMFLOAT4X4 wvpMatrixTransposed;

        getCamera()->getWVPMatrix(&wvpMatrix);
        DirectX::XMStoreFloat4x4(&wvpMatrixTransposed, DirectX::XMMatrixTranspose(
            DirectX::XMLoadFloat4x4(&wvpMatrix)));

        mAssistConfigure->active();
        mAssistConfigure->setMatrix(wvpMatrixTransposed);

        ukive::Renderer::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
        ukive::Renderer::drawObjects(getDrawingObjectManager()->getByTag(kWorldAxis));

        mTerrainConfigure->active();
        mTerrainConfigure->setMatrix(wvpMatrixTransposed);

        ukive::Renderer::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        ukive::Renderer::draw(mVertexBuffer, mIndexBuffer, sizeof(TerrainVertexData), mLodGenerator->getIndexCount());

        //ukive::Renderer::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //ukive::Renderer::drawObjects(getDrawingObjectManager()->getByTag(155));

        //mModelLightEngine->active();
        //mModelLightEngine->setMatrix(wvpMatrixTransposed);

        //ukive::Renderer::setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //ukive::Renderer::drawObjects(getDrawingObjectManager()->getByTag(U3DGraphId::NORMAL_CUBE));
    }

    void TerrainScene::onSceneDestroy()
    {
        Scene::onSceneDestroy();

        if (mAssistConfigure)
        {
            mAssistConfigure->close();
            delete mAssistConfigure;
            mAssistConfigure = nullptr;
        }

        if (mModelConfigure)
        {
            mModelConfigure->close();
            delete mModelConfigure;
            mModelConfigure = nullptr;
        }

        if (mTerrainConfigure)
        {
            mTerrainConfigure->close();
            delete mTerrainConfigure;
            mTerrainConfigure = nullptr;
        }
    }

}