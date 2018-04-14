#include "graph_creator.h"

#include "ukive/graphics/direct3d/drawing_object_manager.h"


namespace shell {

    GraphCreator::GraphCreator(ukive::DrawingObjectManager* mgr)
        :drawing_obj_mgr_(mgr) {}

    GraphCreator::~GraphCreator() {}

    void GraphCreator::calculateNormalVector(
        ModelVertexData* vertices, int vertexCount, int* indices, int indexCount) {

        int triangleCount = indexCount / 3;

        dx::XMVECTOR e0;
        dx::XMVECTOR e1;
        dx::XMVECTOR faceNormal;

        for (int i = 0; i < triangleCount; ++i) {
            int i0 = indices[i * 3 + 0];
            int i1 = indices[i * 3 + 1];
            int i2 = indices[i * 3 + 2];

            dx::XMVECTOR i0Vec
                = dx::XMLoadFloat3(&vertices[i0].position);

            e0 = dx::XMVectorSubtract(
                dx::XMLoadFloat3(&vertices[i1].position),
                i0Vec);

            e1 = dx::XMVectorSubtract(
                dx::XMLoadFloat3(&vertices[i2].position),
                i0Vec);

            faceNormal = dx::XMVector3Cross(e0, e1);

            dx::XMStoreFloat3(
                &vertices[i0].normal,
                dx::XMVectorAdd(
                    dx::XMLoadFloat3(&vertices[i0].normal),
                    faceNormal));
            dx::XMStoreFloat3(
                &vertices[i1].normal,
                dx::XMVectorAdd(
                    dx::XMLoadFloat3(&vertices[i1].normal),
                    faceNormal));
            dx::XMStoreFloat3(
                &vertices[i2].normal,
                dx::XMVectorAdd(
                    dx::XMLoadFloat3(&vertices[i2].normal),
                    faceNormal));
        }

        for (int i = 0; i < vertexCount; ++i) {
            dx::XMVECTOR normalVec = dx::XMLoadFloat3(&vertices[i].normal);
            dx::XMStoreFloat3(
                &vertices[i].normal,
                dx::XMVector3Normalize(normalVec));
        }
    }


    void GraphCreator::putWorldAxis(int tag, float length) {
        UINT vertexCount = 6;
        UINT indexCount = 6;

        if (length < 10.0f) {
            length = 10.0f;
        }

        auto vertexData = new AssistVertexData[vertexCount];
        vertexData[0].position = dx::XMFLOAT3(-length, 0, 0);
        vertexData[0].color = dx::XMFLOAT4(1, 0, 0, 1);
        vertexData[1].position = dx::XMFLOAT3(length, 0, 0);
        vertexData[1].color = dx::XMFLOAT4(1, 0, 0, 1);

        vertexData[2].position = dx::XMFLOAT3(0, -length, 0);
        vertexData[2].color = dx::XMFLOAT4(0, 1, 0, 1);
        vertexData[3].position = dx::XMFLOAT3(0, length, 0);
        vertexData[3].color = dx::XMFLOAT4(0, 1, 0, 1);

        vertexData[4].position = dx::XMFLOAT3(0, 0, -length);
        vertexData[4].color = dx::XMFLOAT4(0, 0, 1, 1);
        vertexData[5].position = dx::XMFLOAT3(0, 0, length);
        vertexData[5].color = dx::XMFLOAT4(0, 0, 1, 1);

        auto indices = new int[indexCount] {
            0, 1, 2, 3, 4, 5
        };

        drawing_obj_mgr_->add(
            vertexData, indices, sizeof(AssistVertexData), vertexCount, indexCount, tag);
    }

    void GraphCreator::putLine(dx::XMFLOAT3* point1, dx::XMFLOAT3* point2, int tag) {
        UINT vertexCount = 2;
        UINT indexCount = 2;

        auto vertexData = new AssistVertexData[vertexCount];
        vertexData[0].position = *point1;
        vertexData[0].color = dx::XMFLOAT4(0.5f, 0, 0, 1);
        vertexData[1].position = *point2;
        vertexData[1].color = dx::XMFLOAT4(0.5f, 0, 0, 1);

        auto indices = new int[indexCount] {
            0, 1
        };

        drawing_obj_mgr_->add(
            vertexData, indices, sizeof(AssistVertexData), vertexCount, indexCount, tag);
    }

    void GraphCreator::putMark(int tag, dx::XMFLOAT3* mark, float length) {
        UINT vertexCount = 4;
        UINT indexCount = 4;

        auto vertexData = new AssistVertexData[vertexCount];
        vertexData[0].position = dx::XMFLOAT3(mark->x - length, 0, mark->z);
        vertexData[0].color = dx::XMFLOAT4(0.5f, 0, 0, 1);
        vertexData[1].position = dx::XMFLOAT3(mark->x + length, 0, mark->z);
        vertexData[1].color = dx::XMFLOAT4(0.5f, 0, 0, 1);
        vertexData[2].position = dx::XMFLOAT3(mark->x, 0, mark->z - length);
        vertexData[2].color = dx::XMFLOAT4(0.5f, 0, 0, 1);
        vertexData[3].position = dx::XMFLOAT3(mark->x, 0, mark->z + length);
        vertexData[3].color = dx::XMFLOAT4(0.5f, 0, 0, 1);

        auto indices = new int[indexCount] {
            0, 1, 2, 3
        };

        drawing_obj_mgr_->add(
            vertexData, indices, sizeof(AssistVertexData), vertexCount, indexCount, tag);
    }

    void GraphCreator::putBlock(int tag, dx::XMFLOAT3* posCenter, float radius) {
        UINT vertexCount = 8;
        UINT indexCount = 36;

        auto vertexData = new AssistVertexData[vertexCount];
        vertexData[0].position = dx::XMFLOAT3(
            posCenter->x - radius, posCenter->y - radius, posCenter->z - radius);
        vertexData[0].color = dx::XMFLOAT4(1, 1, 0, 1);

        vertexData[1].position = dx::XMFLOAT3(
            posCenter->x + radius, posCenter->y - radius, posCenter->z - radius);
        vertexData[1].color = dx::XMFLOAT4(1, 1, 0, 1);

        vertexData[2].position = dx::XMFLOAT3(
            posCenter->x + radius, posCenter->y - radius, posCenter->z + radius);
        vertexData[2].color = dx::XMFLOAT4(1, 1, 0, 1);

        vertexData[3].position = dx::XMFLOAT3(
            posCenter->x - radius, posCenter->y - radius, posCenter->z + radius);
        vertexData[3].color = dx::XMFLOAT4(1, 1, 0, 1);

        vertexData[4].position = dx::XMFLOAT3(
            posCenter->x - radius, posCenter->y + radius, posCenter->z + radius);
        vertexData[4].color = dx::XMFLOAT4(1, 1, 0, 1);

        vertexData[5].position = dx::XMFLOAT3(
            posCenter->x - radius, posCenter->y + radius, posCenter->z - radius);
        vertexData[5].color = dx::XMFLOAT4(1, 1, 0, 1);

        vertexData[6].position = dx::XMFLOAT3(
            posCenter->x + radius, posCenter->y + radius, posCenter->z - radius);
        vertexData[6].color = dx::XMFLOAT4(1, 1, 0, 1);

        vertexData[7].position = dx::XMFLOAT3(
            posCenter->x + radius, posCenter->y + radius, posCenter->z + radius);
        vertexData[7].color = dx::XMFLOAT4(1, 1, 0, 1);

        auto indices = new int[indexCount] {
            0, 5, 1, 1, 5, 6, 1, 6, 2, 2, 6, 7, 2, 7, 3, 3, 7, 4, 3, 4, 0, 0, 4, 5, 3, 0, 2, 2, 0, 1, 5, 4, 6, 6, 4, 7
        };

        drawing_obj_mgr_->add(
            vertexData, indices, sizeof(AssistVertexData), vertexCount, indexCount, tag);
    }

    void GraphCreator::putCube(int tag, float edgeLength) {
        UINT vertexCount = 8;
        UINT indexCount = 36;

        float half = edgeLength / 2.f;

        auto modelVertexData = new ModelVertexData[vertexCount];
        modelVertexData[0].position = dx::XMFLOAT3(-half, -half, -half);
        modelVertexData[0].color = dx::XMFLOAT4(1, 0, 0, 1);
        modelVertexData[0].texcoord = dx::XMFLOAT2(0, 1);

        modelVertexData[1].position = dx::XMFLOAT3(half, -half, -half);
        modelVertexData[1].color = dx::XMFLOAT4(1, 0, 0, 1);
        modelVertexData[1].texcoord = dx::XMFLOAT2(1, 1);

        modelVertexData[2].position = dx::XMFLOAT3(half, -half, half);
        modelVertexData[2].color = dx::XMFLOAT4(0, 1, 0, 1);
        modelVertexData[2].texcoord = dx::XMFLOAT2(1, 0);

        modelVertexData[3].position = dx::XMFLOAT3(-half, -half, half);
        modelVertexData[3].color = dx::XMFLOAT4(0, 1, 0, 1);
        modelVertexData[3].texcoord = dx::XMFLOAT2(0, 0);

        modelVertexData[4].position = dx::XMFLOAT3(-half, half, half);
        modelVertexData[4].color = dx::XMFLOAT4(0, 0, 1, 1);
        modelVertexData[4].texcoord = dx::XMFLOAT2(0, 0);

        modelVertexData[5].position = dx::XMFLOAT3(-half, half, -half);
        modelVertexData[5].color = dx::XMFLOAT4(0, 0, 1, 1);
        modelVertexData[5].texcoord = dx::XMFLOAT2(0, 1);

        modelVertexData[6].position = dx::XMFLOAT3(half, half, -half);
        modelVertexData[6].color = dx::XMFLOAT4(0, 0, 1, 1);
        modelVertexData[6].texcoord = dx::XMFLOAT2(1, 1);

        modelVertexData[7].position = dx::XMFLOAT3(half, half, half);
        modelVertexData[7].color = dx::XMFLOAT4(0, 0, 1, 1);
        modelVertexData[7].texcoord = dx::XMFLOAT2(1, 0);

        auto indices = new int[indexCount] {
            0, 5, 1,
            1, 5, 6,
            1, 6, 2,
            2, 6, 7,
            2, 7, 3,
            3, 7, 4,
            3, 4, 0,
            0, 4, 5,
            3, 0, 2,
            2, 0, 1,
            5, 4, 6,
            6, 4, 7
        };

        calculateNormalVector(modelVertexData, vertexCount, indices, indexCount);

        drawing_obj_mgr_->add(
            modelVertexData, indices, sizeof(ModelVertexData), vertexCount, indexCount, tag);
    }

}