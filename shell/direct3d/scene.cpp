#include "scene.h"

#include "ukive/graphics/drawing_object_manager.h"
#include "ukive/views/direct3d_view.h"

#include "shell/direct3d/camera.h"
#include "shell/direct3d/graph_creator.h"


namespace shell {

    Scene::Scene(ukive::Direct3DView *d3dView,
        unsigned int width, unsigned int height)
    {
        mWidth = width;
        mHeight = height;
        mD3DView = d3dView;

        mDrawingObjectManager = new ukive::DrawingObjectManager();

        mGraphCreator = new GraphCreator(mDrawingObjectManager);

        mCamera = new Camera();
        mCamera->init(width, height);
    }

    Scene::~Scene()
    {
        delete mDrawingObjectManager;
        mDrawingObjectManager = nullptr;

        delete mGraphCreator;
        mGraphCreator = nullptr;

        delete mCamera;
        mCamera = nullptr;
    }


    void Scene::refresh()
    {
        mD3DView->invalidate();
    }


    UINT Scene::getSceneWidth()
    {
        return mWidth;
    }

    UINT Scene::getSceneHeight()
    {
        return mHeight;
    }

    void Scene::onSceneCreate()
    {
    }

    void Scene::onSceneInput(ukive::InputEvent *e)
    {
    }

    void Scene::onSceneResize(unsigned int width, unsigned int height)
    {
        mWidth = width;
        mHeight = height;

        mCamera->resize(width, height);
    }

    void Scene::onSceneDestroy()
    {
    }

    void Scene::getPickLine(int sx, int sy, dx::XMVECTOR *lineOrig, dx::XMVECTOR *lineDir)
    {
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

    Camera *Scene::getCamera()
    {
        return mCamera;
    }

    GraphCreator *Scene::getGraphCreator()
    {
        return mGraphCreator;
    }

    ukive::DrawingObjectManager *Scene::getDrawingObjectManager()
    {
        return mDrawingObjectManager;
    }

    ukive::Direct3DView *Scene::getView()
    {
        return mD3DView;
    }

}