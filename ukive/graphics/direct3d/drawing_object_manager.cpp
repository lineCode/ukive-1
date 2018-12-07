#include "drawing_object_manager.h"

#include "ukive/graphics/direct3d/space.h"
#include "ukive/utils/hresult_utils.h"
#include "ukive/utils/stl_utils.h"
#include "ukive/log.h"


namespace ukive {

    DrawingObjectManager::DrawingObjectManager() {
    }

    DrawingObjectManager::~DrawingObjectManager() {
        STLDeleteElements(&drawing_objs_);
    }


    void DrawingObjectManager::add(
        void* vertices, int* indices,
        unsigned int structSize, unsigned int vertexCount, unsigned int indexCount, int tag) {

        if (!vertices || !indices
            || vertexCount == 0 || indexCount == 0) {
            LOG(Log::ERR) << "Invalid params.";
            return;
        }

        for (auto obj : drawing_objs_) {
            if (obj->tag == tag) {
                LOG(Log::ERR) << "Invalid params.";
                return;
            }
        }

        DrawingObject* dObject = new DrawingObject();
        dObject->tag = tag;
        dObject->vertices = vertices;
        dObject->vertexCount = vertexCount;
        dObject->vertexStructSize = structSize;
        dObject->indexCount = indexCount;

        dObject->vertexBuffer = Space::createVertexBuffer(
            vertices, dObject->vertexStructSize, vertexCount);

        dObject->indexBuffer = Space::createIndexBuffer(
            indices, indexCount);

        drawing_objs_.push_back(dObject);
    }

    DrawingObjectManager::DrawingObject* DrawingObjectManager::getByTag(int tag) {
        for (auto obj : drawing_objs_) {
            if (obj->tag == tag) {
                return obj;
            }
        }

        return nullptr;
    }

    DrawingObjectManager::DrawingObject* DrawingObjectManager::getByPos(size_t pos) {
        size_t index = 0;

        for (auto it = drawing_objs_.begin();
            it != drawing_objs_.end(); ++it, ++index) {
            if (pos == index) {
                return *it;
            }
        }

        return nullptr;
    }

    size_t DrawingObjectManager::getCount() {
        return drawing_objs_.size();
    }

    bool DrawingObjectManager::contains(int tag) {
        for (auto obj : drawing_objs_) {
            if (obj->tag == tag) {
                return true;
            }
        }

        return false;
    }

    void DrawingObjectManager::removeByTag(int tag) {
        for (auto it = drawing_objs_.begin();
            it != drawing_objs_.end(); ++it) {
            if ((*it)->tag == tag) {
                drawing_objs_.erase(it);
                return;
            }
        }
    }

    void DrawingObjectManager::removeByPos(size_t pos) {
        size_t index = 0;

        for (auto it = drawing_objs_.begin();
            it != drawing_objs_.end(); ++it, ++index) {
            if (pos == index) {
                drawing_objs_.erase(it);
                return;
            }
        }
    }


    DrawingObjectManager::DrawingObject::DrawingObject()
        :tag(-1),
        name(L"object"),
        vertices(nullptr),
        vertexCount(0),
        vertexStructSize(0),
        vertexDataOffset(0),
        indexCount(0),
        materialIndex(0) {
    }

    DrawingObjectManager::DrawingObject::~DrawingObject() {
        if (vertices) {
            delete[] vertices;
        }
    }

}