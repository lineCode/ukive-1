#include "drawing_object_manager.h"

#include "ukive/graphics/renderer.h"
#include "ukive/utils/hresult_utils.h"
#include "ukive/log.h"


namespace ukive {

    DrawingObjectManager::DrawingObjectManager() {
    }

    DrawingObjectManager::~DrawingObjectManager() {
    }


    void DrawingObjectManager::add(
        void *vertices, int *indices,
        unsigned int structSize, unsigned int vertexCount, unsigned int indexCount, int tag) {

        if (vertices == nullptr || indices == nullptr
            || vertexCount == 0 || indexCount == 0) {
            Log::e(L"invalid params.");
            return;
        }

        for (auto it = mDrawingObjectList.begin();
            it != mDrawingObjectList.end(); ++it) {
            if ((*it)->tag == tag) {
                Log::e(L"invalid params.");
                return;
            }
        }

        DrawingObject *dObject = new DrawingObject();
        dObject->tag = tag;
        dObject->vertices = vertices;
        dObject->vertexCount = vertexCount;
        dObject->vertexStructSize = structSize;
        dObject->indexCount = indexCount;

        HRESULT hr = Renderer::createVertexBuffer(
            vertices, dObject->vertexStructSize, vertexCount, dObject->vertexBuffer);
        if (FAILED(hr)) {
            Log::e(L"invalid params.");
            return;
        }

        hr = Renderer::createIndexBuffer(
            indices, indexCount, dObject->indexBuffer);
        if (FAILED(hr)) {
            Log::e(L"invalid params.");
            return;
        }

        mDrawingObjectList.push_back(dObject);
    }

    DrawingObjectManager::DrawingObject *DrawingObjectManager::getByTag(int tag) {
        for (auto it = mDrawingObjectList.begin();
            it != mDrawingObjectList.end(); ++it) {
            if ((*it)->tag == tag)
                return *it;
        }

        return nullptr;
    }

    DrawingObjectManager::DrawingObject *DrawingObjectManager::getByPos(size_t pos) {
        size_t index = 0;

        for (auto it = mDrawingObjectList.begin();
            it != mDrawingObjectList.end(); ++it, ++index) {
            if (pos == index)
                return *it;
        }

        return nullptr;
    }

    size_t DrawingObjectManager::getCount() {
        return mDrawingObjectList.size();
    }

    bool DrawingObjectManager::contains(int tag) {
        for (auto it = mDrawingObjectList.begin();
            it != mDrawingObjectList.end(); ++it) {
            if ((*it)->tag == tag)
                return true;
        }

        return false;
    }

    void DrawingObjectManager::removeByTag(int tag) {
        for (auto it = mDrawingObjectList.begin();
            it != mDrawingObjectList.end(); ++it) {
            if ((*it)->tag == tag) {
                mDrawingObjectList.erase(it);
                return;
            }
        }
    }

    void DrawingObjectManager::removeByPos(size_t pos) {
        size_t index = 0;

        for (auto it = mDrawingObjectList.begin();
            it != mDrawingObjectList.end(); ++it, ++index) {
            if (pos == index) {
                mDrawingObjectList.erase(it);
                return;
            }
        }
    }


    DrawingObjectManager::DrawingObject::DrawingObject() {
        tag = -1;
        name = L"object";
        vertices = nullptr;
        vertexBuffer = nullptr;
        indexBuffer = nullptr;
        vertexCount = 0;
        vertexStructSize = 0;
        vertexDataOffset = 0;
        indexCount = 0;
        materialIndex = 0;
    }

    DrawingObjectManager::DrawingObject::~DrawingObject() {
        if (vertices) {
            delete[] vertices;
        }
        if (vertexBuffer) {
            vertexBuffer->Release();
        }
        if (indexBuffer) {
            indexBuffer->Release();
        }
    }

}