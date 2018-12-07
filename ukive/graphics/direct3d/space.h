#ifndef UKIVE_GRAPHICS_DIRECT3D_SPACE_H_
#define UKIVE_GRAPHICS_DIRECT3D_SPACE_H_

#include "ukive/graphics/direct3d/drawing_object_manager.h"
#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    class Space {
    public:
        static void drawObjects(DrawingObjectManager::DrawingObject* object);
        static void draw(ID3D11Buffer* vertices, ID3D11Buffer* indices, int structSize, int indexCount);

        static void setVertexShader(ID3D11VertexShader* shader);
        static void setPixelShader(ID3D11PixelShader* shader);
        static void setInputLayout(ID3D11InputLayout* inputLayout);
        static void setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);
        static void setConstantBuffers(UINT startSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers);

        static void createVertexShader(
            const string16& file_name,
            D3D11_INPUT_ELEMENT_DESC* layout,
            UINT layout_count,
            ID3D11VertexShader** vertex_shader,
            ID3D11InputLayout** input_layout);
        static void createPixelShader(
            const string16& file_name,
            ID3D11PixelShader** pixel_shader);

        static ComPtr<ID3D11Buffer> createVertexBuffer(
            void* vertices, UINT struct_size, UINT vertex_count);
        static ComPtr<ID3D11Buffer> createIndexBuffer(int* indices, UINT index_count);
        static ComPtr<ID3D11Buffer> createConstantBuffer(UINT size);

        static D3D11_MAPPED_SUBRESOURCE lockResource(ID3D11Resource* resource);
        static void unlockResource(ID3D11Resource* resource);
    };

}

#endif  // UKIVE_GRAPHICS_DIRECT3D_SPACE_H_