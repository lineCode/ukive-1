#ifndef UKIVE_GRAPHICS_DIRECT3D_SPACE_H_
#define UKIVE_GRAPHICS_DIRECT3D_SPACE_H_

#include "ukive/graphics/drawing_object_manager.h"
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

        static HRESULT createVertexShader(
            const string16 fileName,
            D3D11_INPUT_ELEMENT_DESC* polygonLayout,
            UINT numElements,
            ID3D11VertexShader** vertexShader,
            ID3D11InputLayout** inputLayout);
        static HRESULT createPixelShader(
            const string16 fileName,
            ID3D11PixelShader** pixelShader);

        static HRESULT createVertexBuffer(
            void* vertices, UINT structSize, UINT vertexCount, ID3D11Buffer*& vertexBuffer);
        static HRESULT createIndexBuffer(int* indices, UINT indexCount, ID3D11Buffer*& indexBuffer);
        static HRESULT createConstantBuffer(UINT size, ID3D11Buffer** buffer);

        static D3D11_MAPPED_SUBRESOURCE lockResource(ID3D11Resource* resource);
        static void unlockResource(ID3D11Resource* resource);
    };

}

#endif  // UKIVE_GRAPHICS_DIRECT3D_SPACE_H_