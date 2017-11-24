#ifndef UKIVE_GRAPHICS_DRAWING_OBJECT_MANAGER_H_
#define UKIVE_GRAPHICS_DRAWING_OBJECT_MANAGER_H_

#include <list>

#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/utils/string_utils.h"


namespace ukive {

    class Renderer;

    class DrawingObjectManager {
    public:
        struct DrawingObject {
            DrawingObject();
            ~DrawingObject();

            int tag;
            string16 name;
            void *vertices;
            ID3D11Buffer *vertexBuffer;
            ID3D11Buffer *indexBuffer;
            unsigned int vertexCount;
            unsigned int vertexStructSize;
            unsigned int vertexDataOffset;
            unsigned int indexCount;
            unsigned int materialIndex;
        };

    public:
        DrawingObjectManager();
        ~DrawingObjectManager();

        void add(
            void *vertices, int *indices,
            unsigned int structSize, unsigned int vertexCount, unsigned int indexCount, int tag);
        DrawingObject *getByTag(int tag);
        DrawingObject *getByPos(size_t pos);
        size_t getCount();
        bool contains(int tag);
        void removeByTag(int tag);
        void removeByPos(size_t pos);

    private:
        std::list<DrawingObject*> mDrawingObjectList;
    };

}

#endif  // UKIVE_GRAPHICS_DRAWING_OBJECT_MANAGER_H_