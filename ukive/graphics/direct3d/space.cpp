#include "space.h"

#include <fstream>

#include "ukive/application.h"
#include "ukive/utils/hresult_utils.h"


namespace ukive {

    void Space::drawObjects(DrawingObjectManager::DrawingObject* object) {
        if (object == nullptr) {
            return;
        }

        auto gdm = Application::getGraphicDeviceManager();

        gdm->getD3DDeviceContext()->IASetVertexBuffers(
            0, 1, &object->vertexBuffer, &object->vertexStructSize, &object->vertexDataOffset);
        gdm->getD3DDeviceContext()->IASetIndexBuffer(object->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        gdm->getD3DDeviceContext()->DrawIndexed(object->indexCount, 0, 0);
    }

    void Space::draw(ID3D11Buffer* vertices, ID3D11Buffer* indices, int structSize, int indexCount) {
        UINT vertexDataOffset = 0;
        UINT vertexStructSize = structSize;

        auto gdm = Application::getGraphicDeviceManager();

        gdm->getD3DDeviceContext()->IASetVertexBuffers(
            0, 1, &vertices, &vertexStructSize, &vertexDataOffset);
        gdm->getD3DDeviceContext()->IASetIndexBuffer(indices, DXGI_FORMAT_R32_UINT, 0);
        gdm->getD3DDeviceContext()->DrawIndexed(indexCount, 0, 0);
    }


    void Space::setVertexShader(ID3D11VertexShader* shader) {
        auto gdm = Application::getGraphicDeviceManager();
        gdm->getD3DDeviceContext()->VSSetShader(shader, 0, 0);
    }

    void Space::setPixelShader(ID3D11PixelShader* shader) {
        auto gdm = Application::getGraphicDeviceManager();
        gdm->getD3DDeviceContext()->PSSetShader(shader, 0, 0);
    }

    void Space::setInputLayout(ID3D11InputLayout* inputLayout) {
        auto gdm = Application::getGraphicDeviceManager();
        gdm->getD3DDeviceContext()->IASetInputLayout(inputLayout);
    }

    void Space::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology) {
        auto gdm = Application::getGraphicDeviceManager();
        gdm->getD3DDeviceContext()->IASetPrimitiveTopology(topology);
    }

    void Space::setConstantBuffers(
        UINT startSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) {
        auto gdm = Application::getGraphicDeviceManager();
        gdm->getD3DDeviceContext()->VSSetConstantBuffers(
            startSlot, NumBuffers, ppConstantBuffers);
    }

    HRESULT Space::createVertexShader(
        const string16 fileName,
        D3D11_INPUT_ELEMENT_DESC* polygonLayout,
        UINT numElements,
        ID3D11VertexShader** vertexShader,
        ID3D11InputLayout** inputLayout) {

        std::ifstream reader(fileName.c_str(), std::ios::binary);
        auto cpos = reader.tellg();
        reader.seekg(0, std::ios_base::end);
        size_t charSize = (size_t)reader.tellg();
        reader.seekg(cpos);

        char* shaderBuf = new char[charSize];
        reader.read(shaderBuf, charSize);

        auto gdm = Application::getGraphicDeviceManager();

        RH(gdm->getD3DDevice()->CreateVertexShader(
            shaderBuf, charSize, 0, vertexShader));

        RH(gdm->getD3DDevice()->CreateInputLayout(
            polygonLayout, numElements,
            shaderBuf, charSize, inputLayout));

        delete[] shaderBuf;

        return S_OK;
    }

    HRESULT Space::createPixelShader(
        const string16 fileName,
        ID3D11PixelShader** pixelShader) {

        std::ifstream reader(fileName.c_str(), std::ios::binary);
        auto cpos = reader.tellg();
        reader.seekg(0, std::ios_base::end);
        size_t charSize = (size_t)reader.tellg();
        reader.seekg(cpos);

        char* shaderBuf = new char[charSize];
        reader.read(shaderBuf, charSize);

        RH(Application::getGraphicDeviceManager()->getD3DDevice()->CreatePixelShader(
            shaderBuf, charSize, 0, pixelShader));

        delete[] shaderBuf;

        return S_OK;
    }


    HRESULT Space::createVertexBuffer(
        void* vertices, UINT structSize, UINT vertexCount, ID3D11Buffer*& vertexBuffer) {

        D3D11_BUFFER_DESC vertexBufferDesc;
        D3D11_SUBRESOURCE_DATA vertexData;

        vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        vertexBufferDesc.ByteWidth = structSize * vertexCount;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        vertexBufferDesc.MiscFlags = 0;
        vertexBufferDesc.StructureByteStride = 0;

        vertexData.pSysMem = vertices;
        vertexData.SysMemPitch = 0;
        vertexData.SysMemSlicePitch = 0;

        RH(Application::getGraphicDeviceManager()->getD3DDevice()
            ->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer));

        return S_OK;
    }

    HRESULT Space::createIndexBuffer(int* indices, UINT indexCount, ID3D11Buffer*& indexBuffer) {
        D3D11_BUFFER_DESC indexBufferDesc;
        D3D11_SUBRESOURCE_DATA indexData;

        // ÉèÖÃË÷Òý»º³åÃèÊö.
        indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        indexBufferDesc.ByteWidth = sizeof(int)* indexCount;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        indexBufferDesc.MiscFlags = 0;
        indexBufferDesc.StructureByteStride = 0;

        // Ö¸Ïò´æÁÙÊ±Ë÷Òý»º³å.
        indexData.pSysMem = indices;
        indexData.SysMemPitch = 0;
        indexData.SysMemSlicePitch = 0;

        // ´´½¨Ë÷Òý»º³å.
        RH(Application::getGraphicDeviceManager()->getD3DDevice()
            ->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer));

        return S_OK;
    }

    HRESULT Space::createConstantBuffer(UINT size, ID3D11Buffer** buffer) {
        HRESULT hr = E_FAIL;
        ID3D11Buffer* _buffer = 0;
        D3D11_BUFFER_DESC constBufferDesc;

        constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        constBufferDesc.ByteWidth = size;
        constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        constBufferDesc.MiscFlags = 0;
        constBufferDesc.StructureByteStride = 0;

        hr = Application::getGraphicDeviceManager()->getD3DDevice()
            ->CreateBuffer(&constBufferDesc, 0, &_buffer);

        *buffer = _buffer;

        return hr;
    }

    D3D11_MAPPED_SUBRESOURCE Space::lockResource(ID3D11Resource* resource) {
        HRESULT hr = E_FAIL;
        D3D11_MAPPED_SUBRESOURCE mappedResource;

        hr = Application::getGraphicDeviceManager()->getD3DDeviceContext()
            ->Map(resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (SUCCEEDED(hr))
            return mappedResource;

        mappedResource.pData = nullptr;
        return mappedResource;
    }

    void Space::unlockResource(ID3D11Resource* resource) {
        Application::getGraphicDeviceManager()->getD3DDeviceContext()->Unmap(resource, 0);
    }

}