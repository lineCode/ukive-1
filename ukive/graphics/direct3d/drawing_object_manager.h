#ifndef UKIVE_GRAPHICS_DIRECT3D_DRAWING_OBJECT_MANAGER_H_
#define UKIVE_GRAPHICS_DIRECT3D_DRAWING_OBJECT_MANAGER_H_

#include <list>

#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/utils/string_utils.h"


namespace ukive {

    class DrawingObjectManager {
    public:
        struct DrawingObject {
            DrawingObject();
            ~DrawingObject();

            int tag;
            string16 name;
            void* vertices;
            ComPtr<ID3D11Buffer> vertexBuffer;
            ComPtr<ID3D11Buffer> indexBuffer;
            unsigned int vertexCount;
            unsigned int vertexStructSize;
            unsigned int vertexDataOffset;
            unsigned int indexCount;
            unsigned int materialIndex;
        };

        DrawingObjectManager();
        ~DrawingObjectManager();

        void add(
            void* vertices, int* indices,
            unsigned int structSize, unsigned int vertexCount, unsigned int indexCount, int tag);
        DrawingObject* getByTag(int tag);
        DrawingObject* getByPos(size_t pos);
        size_t getCount();
        bool contains(int tag);
        void removeByTag(int tag);
        void removeByPos(size_t pos);

    private:
        std::list<DrawingObject*> drawing_objs_;
    };

}

#endif  // UKIVE_GRAPHICS_DIRECT3D_DRAWING_OBJECT_MANAGER_H_