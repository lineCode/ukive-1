#include "lod_generator.h"

#include <cmath>
#include <fstream>

#include "ukive/log.h"

#include "shell/lod/qtree_node.h"
#include "shell/lod/terrain_configure.h"


namespace shell {

    LodGenerator::LodGenerator(float edgeLength, int maxLevel) {
        DCHECK(edgeLength > 0 && maxLevel >= 1);

        COE_ROUGH = 2.f;
        COE_DISTANCE = 30.f;

        mMaxLevel = maxLevel;
        mRowVertexCount = std::pow(2, mMaxLevel) + 1;
        mVertexCount = mRowVertexCount * mRowVertexCount;

        mFlag = new char[mVertexCount];
        std::memset(mFlag, 0, mVertexCount);

        std::wstring altitudeFileName(::_wgetcwd(nullptr, 0));
        altitudeFileName.append(L"\\altitude.raw");

        std::ifstream reader(altitudeFileName, std::ios::binary);
        if (reader.fail()) {
            throw std::runtime_error("LodGenerator-Constructor(): read file failed.");
        }
        auto cpos = reader.tellg();
        reader.seekg(0, std::ios_base::end);
        size_t charSize = (size_t)reader.tellg();
        reader.seekg(cpos);

        mAltitude = new char[charSize];
        reader.read(mAltitude, charSize);

        mVertices = new TerrainVertexData[mVertexCount];
        for (int i = 0; i < mVertexCount; ++i) {
            int row = i / mRowVertexCount;
            int column = i % mRowVertexCount;

            int altitudeRow = (int)((ALTITUDE_MAP_SIZE / (float)mRowVertexCount)*row);
            int altitudeColumn = (int)((ALTITUDE_MAP_SIZE / (float)mRowVertexCount)*column);

            int altitude = mAltitude[
                (ALTITUDE_MAP_SIZE - 1 - altitudeRow) * ALTITUDE_MAP_SIZE + altitudeColumn];
            if (altitude < 0)
                altitude += 255;

            mVertices[i].position = dx::XMFLOAT3(
                edgeLength*column / (mRowVertexCount - 1),
                (float)altitude * 2, edgeLength - edgeLength*row / (mRowVertexCount - 1));
        }

        mIndexCount = 0;
        mMaxIndexCount = (mRowVertexCount - 1)*(mRowVertexCount - 1) * 2 * 3;

        mIndices = new int[mMaxIndexCount];

        generateQuadTree();
        determineRoughAndBound(mRootQueue);
    }

    LodGenerator::~LodGenerator() {
        if (mRootQueue) {
            QTreeNode* curQueue = mRootQueue;
            QTreeNode* nextQueue = nullptr;

            do {
                while (curQueue) {
                    QTreeNode* node = curQueue;
                    curQueue = curQueue->next;

                    for (int i = 0; i < 4; ++i) {
                        if (node->child[i]) {
                            enqueue(nextQueue, node->child[i]);
                        }
                    }

                    delete node;
                }

                curQueue = nextQueue;
                nextQueue = nullptr;
            } while (curQueue);
        }

        delete[] mFlag;
        delete[] mIndices;
        delete[] mVertices;
        delete[] mAltitude;
    }


    inline void LodGenerator::enqueue(QTreeNode*& queue, QTreeNode* node) {
        node->next = queue;
        queue = node;
    }

    inline QTreeNode* LodGenerator::dequeue(QTreeNode*& queue) {
        if (queue) {
            QTreeNode* node = queue;
            queue = queue->next;
            return node;
        }

        return nullptr;
    }

    inline void LodGenerator::clearQueue(QTreeNode*& queue) {
        while (queue) {
            QTreeNode* node = queue;
            queue = queue->next;
            delete node;
        }
    }


    inline int LodGenerator::calInnerStep(QTreeNode* node) {
        return (mRowVertexCount - 1) / std::pow(2, node->level + 1);
    }

    inline int LodGenerator::calNeighborStep(QTreeNode* node) {
        return (mRowVertexCount - 1) / std::pow(2, node->level);
    }

    inline int LodGenerator::calChildStep(QTreeNode* node) {
        return (mRowVertexCount - 1) / std::pow(2, node->level + 2);
    }


    void LodGenerator::generateQuadTree() {
        int level = 0;
        QTreeNode* curQueue = nullptr;
        QTreeNode* nextQueue = nullptr;

        mRootQueue = new QTreeNode();
        generateRootNodeData(mRootQueue);

        curQueue = mRootQueue;

        while (level < mMaxLevel - 1) {
            QTreeNode* iterator = curQueue;
            while (iterator) {
                QTreeNode* parent = iterator;
                iterator = iterator->next;

                for (int i = 0; i < 4; ++i) {
                    QTreeNode* child = new QTreeNode();
                    generateChildNodeData(parent, child, level + 1, i);

                    parent->child[i] = child;
                    enqueue(nextQueue, child);
                }
            }

            curQueue = nextQueue;
            nextQueue = nullptr;

            ++level;
        }
    }

    void LodGenerator::generateRootNodeData(QTreeNode* root) {
        root->level = 0;
        root->indexX = (mRowVertexCount - 1) / 2;
        root->indexY = (mRowVertexCount - 1) / 2;
    }

    void LodGenerator::generateChildNodeData(
        QTreeNode* parent, QTreeNode* child, int level, int position)
    {
        child->level = level;

        int pIndexX = parent->indexX;
        int pIndexY = parent->indexY;

        int step = calChildStep(parent);

        switch (position)
        {
        case 0:
            child->indexX = pIndexX - step;
            child->indexY = pIndexY - step;
            break;
        case 1:
            child->indexX = pIndexX + step;
            child->indexY = pIndexY - step;
            break;
        case 2:
            child->indexX = pIndexX - step;
            child->indexY = pIndexY + step;
            break;
        case 3:
            child->indexX = pIndexX + step;
            child->indexY = pIndexY + step;
            break;
        }
    }


    void LodGenerator::determineRoughAndBound(QTreeNode* node)
    {
        //使用stack和while循环代替递归，防止调用栈溢出。

        //用于保存递归上下文的结构体。
        struct Snapshot
        {
            QTreeNode* node;

            float d0, d1, d2, d3, d4;
            float d5, d6, d7, d8;
            float nodeSize;
            float maxD;
            dx::XMFLOAT3 minC;
            dx::XMFLOAT3 maxC;

            int stage;
            Snapshot* behind;
        };

        //栈顶指针。
        Snapshot* recursionStack = nullptr;

        Snapshot* current = new Snapshot();
        current->node = node;
        current->stage = 0;
        current->behind = nullptr;

        recursionStack = current;

        //返回值
        float diff;
        dx::XMFLOAT3 mincoord;
        dx::XMFLOAT3 maxcoord;

        while (recursionStack)
        {
            //栈pop.
            current = recursionStack;
            recursionStack = recursionStack->behind;

            switch (current->stage)
            {
            case 0:  //在第一次递归之前。
            {
                int innerStep = calInnerStep(current->node);
                TerrainVertexData *vData[9];

                vData[0] = &mVertices[
                    (current->node->indexY - innerStep)*mRowVertexCount + current->node->indexX - innerStep];
                vData[1] = &mVertices[
                    (current->node->indexY - innerStep)*mRowVertexCount + current->node->indexX];
                vData[2] = &mVertices[
                    (current->node->indexY - innerStep)*mRowVertexCount + current->node->indexX + innerStep];
                vData[3] = &mVertices[
                    current->node->indexY*mRowVertexCount + current->node->indexX - innerStep];
                vData[4] = &mVertices[
                    current->node->indexY*mRowVertexCount + current->node->indexX];
                vData[5] = &mVertices[
                    current->node->indexY*mRowVertexCount + current->node->indexX + innerStep];
                vData[6] = &mVertices[
                    (current->node->indexY + innerStep)*mRowVertexCount + current->node->indexX - innerStep];
                vData[7] = &mVertices[
                    (current->node->indexY + innerStep)*mRowVertexCount + current->node->indexX];
                vData[8] = &mVertices[
                    (current->node->indexY + innerStep)*mRowVertexCount + current->node->indexX + innerStep];

                float minX, minY, minZ;
                float maxX, maxY, maxZ;
                for (int i = 0; i < 9; ++i)
                {
                    float x = vData[i]->position.x;
                    float y = vData[i]->position.y;
                    float z = vData[i]->position.z;

                    if (i == 0) {
                        minX = maxX = x;
                        minY = maxY = y;
                        minZ = maxZ = z;
                    } else {
                        if (x < minX) minX = x;
                        if (x > maxX) maxX = x;

                        if (y < minY) minY = y;
                        if (y > maxY) maxY = y;

                        if (z < minZ) minZ = z;
                        if (z > maxZ) maxZ = z;
                    }
                }

                current->minC = dx::XMFLOAT3(minX, minY, minZ);
                current->maxC = dx::XMFLOAT3(maxX, maxY, maxZ);

                float topHor = (vData[0]->position.y + vData[2]->position.y) / 2.f;
                float rightHor = (vData[2]->position.y + vData[8]->position.y) / 2.f;
                float bottomHor = (vData[8]->position.y + vData[6]->position.y) / 2.f;
                float leftHor = (vData[6]->position.y + vData[0]->position.y) / 2.f;

                current->d0 = std::abs(vData[1]->position.y - topHor);
                current->d1 = std::abs(vData[5]->position.y - rightHor);
                current->d2 = std::abs(vData[7]->position.y - bottomHor);
                current->d3 = std::abs(vData[3]->position.y - leftHor);
                current->d4 = std::abs(vData[4]->position.y - (topHor + rightHor + bottomHor + leftHor) / 4.f);

                dx::XMVECTOR length = dx::XMVector3Length(dx::XMVectorSubtract(
                    dx::XMLoadFloat3(&vData[2]->position),
                    dx::XMLoadFloat3(&vData[0]->position)));
                current->nodeSize = dx::XMVectorGetX(length);

                if (current->node->child[0]) {
                    current->stage = 1;
                    current->behind = recursionStack;
                    recursionStack = current;

                    //递归，第一个子节点。
                    Snapshot* next = new Snapshot();
                    next->node = current->node->child[0];
                    next->stage = 0;
                    next->behind = recursionStack;
                    recursionStack = next;
                } else {
                    current->maxD = current->d0;

                    if (current->d1 > current->maxD) current->maxD = current->d1;
                    if (current->d2 > current->maxD) current->maxD = current->d2;
                    if (current->d3 > current->maxD) current->maxD = current->d3;
                    if (current->d4 > current->maxD) current->maxD = current->d4;

                    current->stage = 4;
                    current->behind = recursionStack;
                    recursionStack = current;
                }

                break;
            }

            case 1:  //第一次调用返回后。
            {
                current->d5 = diff;

                if (mincoord.x < current->minC.x) current->minC.x = mincoord.x;
                if (mincoord.y < current->minC.y) current->minC.y = mincoord.y;
                if (mincoord.z < current->minC.z) current->minC.z = mincoord.z;

                if (maxcoord.x > current->maxC.x) current->maxC.x = maxcoord.x;
                if (maxcoord.y > current->maxC.y) current->maxC.y = maxcoord.y;
                if (maxcoord.z > current->maxC.z) current->maxC.z = maxcoord.z;

                current->stage = 2;
                current->behind = recursionStack;
                recursionStack = current;

                //递归，第二个子节点。
                Snapshot* next = new Snapshot();
                next->node = current->node->child[1];
                next->stage = 0;
                next->behind = recursionStack;
                recursionStack = next;

                break;
            }

            case 2:  //第二次调用返回后。
            {
                current->d6 = diff;

                if (mincoord.x < current->minC.x) current->minC.x = mincoord.x;
                if (mincoord.y < current->minC.y) current->minC.y = mincoord.y;
                if (mincoord.z < current->minC.z) current->minC.z = mincoord.z;

                if (maxcoord.x > current->maxC.x) current->maxC.x = maxcoord.x;
                if (maxcoord.y > current->maxC.y) current->maxC.y = maxcoord.y;
                if (maxcoord.z > current->maxC.z) current->maxC.z = maxcoord.z;

                current->stage = 3;
                current->behind = recursionStack;
                recursionStack = current;

                //递归，第三个子节点。
                Snapshot* next = new Snapshot();
                next->node = current->node->child[2];
                next->stage = 0;
                next->behind = recursionStack;
                recursionStack = next;

                break;
            }

            case 3:  //第三次调用返回后。
            {
                current->d7 = diff;

                if (mincoord.x < current->minC.x) current->minC.x = mincoord.x;
                if (mincoord.y < current->minC.y) current->minC.y = mincoord.y;
                if (mincoord.z < current->minC.z) current->minC.z = mincoord.z;

                if (maxcoord.x > current->maxC.x) current->maxC.x = maxcoord.x;
                if (maxcoord.y > current->maxC.y) current->maxC.y = maxcoord.y;
                if (maxcoord.z > current->maxC.z) current->maxC.z = maxcoord.z;

                current->stage = 4;
                current->behind = recursionStack;
                recursionStack = current;

                //递归，第四个子节点。
                Snapshot *next = new Snapshot();
                next->node = current->node->child[3];
                next->stage = 0;
                next->behind = recursionStack;
                recursionStack = next;

                break;
            }

            case 4:   //第四次调用返回后。
            {
                if (current->node->child[0]) {
                    current->d8 = diff;
                    current->maxD = current->d0;

                    if (mincoord.x < current->minC.x) current->minC.x = mincoord.x;
                    if (mincoord.y < current->minC.y) current->minC.y = mincoord.y;
                    if (mincoord.z < current->minC.z) current->minC.z = mincoord.z;

                    if (maxcoord.x > current->maxC.x) current->maxC.x = maxcoord.x;
                    if (maxcoord.y > current->maxC.y) current->maxC.y = maxcoord.y;
                    if (maxcoord.z > current->maxC.z) current->maxC.z = maxcoord.z;

                    if (current->d1 > current->maxD) current->maxD = current->d1;
                    if (current->d2 > current->maxD) current->maxD = current->d2;
                    if (current->d3 > current->maxD) current->maxD = current->d3;
                    if (current->d4 > current->maxD) current->maxD = current->d4;
                    if (current->d5 > current->maxD) current->maxD = current->d5;
                    if (current->d6 > current->maxD) current->maxD = current->d6;
                    if (current->d7 > current->maxD) current->maxD = current->d7;
                    if (current->d8 > current->maxD) current->maxD = current->d8;
                }

                diff = current->maxD;
                mincoord = current->minC;
                maxcoord = current->maxC;

                current->node->rough = current->maxD / current->nodeSize;
                current->node->mincoord = current->minC;
                current->node->maxcoord = current->maxC;

                delete current;
                break;
            }
            }
        }
    }


    bool LodGenerator::checkNodeCanDivide(QTreeNode* node)
    {
        int step = calNeighborStep(node);

        bool leftAdjacent = false;
        bool topAdjacent = false;
        bool rightAdjacent = false;
        bool bottomAdjacent = false;

        if (node->indexX - step < 0
            || mFlag[node->indexY*mRowVertexCount + node->indexX - step] != 0) {
            leftAdjacent = true;
        }

        if (node->indexY - step < 0
            || mFlag[(node->indexY - step)*mRowVertexCount + node->indexX] != 0) {
            topAdjacent = true;
        }

        if (node->indexX + step > mRowVertexCount - 1
            || mFlag[node->indexY*mRowVertexCount + node->indexX + step] != 0) {
            rightAdjacent = true;
        }

        if (node->indexY + step > mRowVertexCount - 1
            || mFlag[(node->indexY + step)*mRowVertexCount + node->indexX] != 0) {
            bottomAdjacent = true;
        }

        return (leftAdjacent && topAdjacent && rightAdjacent && bottomAdjacent);
    }

    bool LodGenerator::assessNodeCanDivide(
        QTreeNode* node, dx::XMFLOAT3 viewPosition)
    {
        int innerStep = calInnerStep(node);

        int centerIndex = node->indexY*mRowVertexCount + node->indexX;
        int leftTopIndex = (node->indexY - innerStep)*mRowVertexCount + node->indexX - innerStep;
        int rightTopIndex = (node->indexY - innerStep)*mRowVertexCount + node->indexX + innerStep;

        dx::XMFLOAT3 nCenter = mVertices[centerIndex].position;

        dx::XMVECTOR length = dx::XMVector3Length(dx::XMVectorSubtract(
            dx::XMLoadFloat3(&nCenter),
            dx::XMLoadFloat3(&viewPosition)));
        float distance = dx::XMVectorGetX(length);

        dx::XMVECTOR size = dx::XMVector3Length(dx::XMVectorSubtract(
            dx::XMLoadFloat3(&mVertices[rightTopIndex].position),
            dx::XMLoadFloat3(&mVertices[leftTopIndex].position)));
        float nodeSize = dx::XMVectorGetX(size);

        return (distance / (nodeSize * node->rough * COE_DISTANCE * COE_ROUGH)) < 1.f;
    }


    void LodGenerator::drawNode(QTreeNode* node, int* indexBuffer)
    {
        int step = calNeighborStep(node);

        if (!indexBuffer) {
            indexBuffer = mIndices;
        }

        bool skipLeft = false;
        bool skipTop = false;
        bool skipRight = false;
        bool skipBottom = false;

        if (node->indexX - step < 0
            || mFlag[node->indexY*mRowVertexCount + node->indexX - step] == 0) {
            skipLeft = true;
        }

        if (node->indexY - step < 0
            || mFlag[(node->indexY - step)*mRowVertexCount + node->indexX] == 0) {
            skipTop = true;
        }

        if (node->indexX + step > mRowVertexCount - 1
            || mFlag[node->indexY*mRowVertexCount + node->indexX + step] == 0) {
            skipRight = true;
        }

        if (node->indexY + step > mRowVertexCount - 1
            || mFlag[(node->indexY + step)*mRowVertexCount + node->indexX] == 0) {
            skipBottom = true;
        }

        int nodeStep = calInnerStep(node);

        int centerIndex = node->indexY*mRowVertexCount + node->indexX;
        int leftTopIndex = (node->indexY - nodeStep)*mRowVertexCount + node->indexX - nodeStep;
        int rightTopIndex = (node->indexY - nodeStep)*mRowVertexCount + node->indexX + nodeStep;
        int leftBottomIndex = (node->indexY + nodeStep)*mRowVertexCount + node->indexX - nodeStep;
        int rightBottomIndex = (node->indexY + nodeStep)*mRowVertexCount + node->indexX + nodeStep;

        if (!skipLeft) {
            int leftIndex = node->indexY*mRowVertexCount + node->indexX - nodeStep;

            indexBuffer[mIndexCount++] = centerIndex;
            indexBuffer[mIndexCount++] = leftBottomIndex;
            indexBuffer[mIndexCount++] = leftIndex;

            indexBuffer[mIndexCount++] = centerIndex;
            indexBuffer[mIndexCount++] = leftIndex;
            indexBuffer[mIndexCount++] = leftTopIndex;
        } else {
            indexBuffer[mIndexCount++] = centerIndex;
            indexBuffer[mIndexCount++] = leftBottomIndex;
            indexBuffer[mIndexCount++] = leftTopIndex;
        }

        if (!skipTop) {
            int topIndex = (node->indexY - nodeStep)*mRowVertexCount + node->indexX;

            indexBuffer[mIndexCount++] = centerIndex;
            indexBuffer[mIndexCount++] = leftTopIndex;
            indexBuffer[mIndexCount++] = topIndex;

            indexBuffer[mIndexCount++] = centerIndex;
            indexBuffer[mIndexCount++] = topIndex;
            indexBuffer[mIndexCount++] = rightTopIndex;
        } else {
            indexBuffer[mIndexCount++] = centerIndex;
            indexBuffer[mIndexCount++] = leftTopIndex;
            indexBuffer[mIndexCount++] = rightTopIndex;
        }

        if (!skipRight) {
            int rightIndex = node->indexY*mRowVertexCount + node->indexX + nodeStep;

            indexBuffer[mIndexCount++] = centerIndex;
            indexBuffer[mIndexCount++] = rightTopIndex;
            indexBuffer[mIndexCount++] = rightIndex;

            indexBuffer[mIndexCount++] = centerIndex;
            indexBuffer[mIndexCount++] = rightIndex;
            indexBuffer[mIndexCount++] = rightBottomIndex;
        } else {
            indexBuffer[mIndexCount++] = centerIndex;
            indexBuffer[mIndexCount++] = rightTopIndex;
            indexBuffer[mIndexCount++] = rightBottomIndex;
        }

        if (!skipBottom) {
            int bottomIndex = (node->indexY + nodeStep)*mRowVertexCount + node->indexX;

            indexBuffer[mIndexCount++] = centerIndex;
            indexBuffer[mIndexCount++] = rightBottomIndex;
            indexBuffer[mIndexCount++] = bottomIndex;

            indexBuffer[mIndexCount++] = centerIndex;
            indexBuffer[mIndexCount++] = bottomIndex;
            indexBuffer[mIndexCount++] = leftBottomIndex;
        } else {
            indexBuffer[mIndexCount++] = centerIndex;
            indexBuffer[mIndexCount++] = rightBottomIndex;
            indexBuffer[mIndexCount++] = leftBottomIndex;
        }
    }

    bool LodGenerator::cullNodeWithBound(QTreeNode *node, dx::XMFLOAT4X4 wvpMatrix)
    {
        bool result = false;
        dx::XMVECTOR plane[6];

        dx::XMVECTOR col0 = dx::XMVectorSet(wvpMatrix._11, wvpMatrix._21, wvpMatrix._31, wvpMatrix._41);
        dx::XMVECTOR col1 = dx::XMVectorSet(wvpMatrix._12, wvpMatrix._22, wvpMatrix._32, wvpMatrix._42);
        dx::XMVECTOR col2 = dx::XMVectorSet(wvpMatrix._13, wvpMatrix._23, wvpMatrix._33, wvpMatrix._43);
        dx::XMVECTOR col3 = dx::XMVectorSet(wvpMatrix._14, wvpMatrix._24, wvpMatrix._34, wvpMatrix._44);

        plane[0] = col2;
        plane[1] = dx::XMVectorSubtract(col3, col2);
        plane[2] = dx::XMVectorAdd(col3, col0);
        plane[3] = dx::XMVectorSubtract(col3, col0);
        plane[4] = dx::XMVectorSubtract(col3, col1);
        plane[5] = dx::XMVectorAdd(col3, col1);

        dx::XMFLOAT3 p, q;
        for (unsigned int i = 0; i < 6; i++) {
            plane[i] = dx::XMVector4Normalize(plane[i]);

            dx::XMFLOAT4 planeStore;
            dx::XMStoreFloat4(&planeStore, plane[i]);

            if (planeStore.x > 0) {
                q.x = node->maxcoord.x;
                p.x = node->mincoord.x;
            } else {
                p.x = node->maxcoord.x;
                q.x = node->mincoord.x;
            }

            if (planeStore.y > 0) {
                q.y = node->maxcoord.y;
                p.y = node->mincoord.y;
            } else {
                p.y = node->maxcoord.y;
                q.y = node->mincoord.y;
            }

            if (planeStore.z > 0) {
                q.z = node->maxcoord.z;
                p.z = node->mincoord.z;
            } else {
                p.z = node->maxcoord.z;
                q.z = node->mincoord.z;
            }

            dx::XMVECTOR dot = dx::XMVector3Dot(plane[i], dx::XMLoadFloat3(&q));
            if (dx::XMVectorGetX(dot) + planeStore.w < 0) {
                result = true;
                break;
            }
        }

        return result;
    }

    void LodGenerator::constructNodeBound(QTreeNode *node, dx::XMFLOAT3 *bound)
    {
        int innerStep = calInnerStep(node);
        TerrainVertexData* vData[9];

        vData[0] = &mVertices[
            (node->indexY - innerStep)*mRowVertexCount + node->indexX - innerStep];
        vData[1] = &mVertices[
            (node->indexY - innerStep)*mRowVertexCount + node->indexX];
        vData[2] = &mVertices[
            (node->indexY - innerStep)*mRowVertexCount + node->indexX + innerStep];
        vData[3] = &mVertices[
            node->indexY*mRowVertexCount + node->indexX - innerStep];
        vData[4] = &mVertices[
            node->indexY*mRowVertexCount + node->indexX];
        vData[5] = &mVertices[
            node->indexY*mRowVertexCount + node->indexX + innerStep];
        vData[6] = &mVertices[
            (node->indexY + innerStep)*mRowVertexCount + node->indexX - innerStep];
        vData[7] = &mVertices[
            (node->indexY + innerStep)*mRowVertexCount + node->indexX];
        vData[8] = &mVertices[
            (node->indexY + innerStep)*mRowVertexCount + node->indexX + innerStep];


        float minX = 0.f;
        float maxX = 0.f;
        float minY = 0.f;
        float maxY = 0.f;
        float minZ = 0.f;
        float maxZ = 0.f;

        for (int i = 0; i < 9; ++i) {
            if (i == 0) {
                minX = maxX = vData[i]->position.x;
                minY = maxY = vData[i]->position.y;
                minZ = maxZ = vData[i]->position.z;
            } else {
                float x = vData[i]->position.x;
                float y = vData[i]->position.y;
                float z = vData[i]->position.z;

                if (x < minX) minX = x;
                if (x > maxX) maxX = x;

                if (y < minY) minY = y;
                if (y > maxY) maxY = y;

                if (z < minZ) minZ = z;
                if (z > maxZ) maxZ = z;
            }
        }

        bound[0].x = minX;
    }


    void LodGenerator::renderLodTerrain(
        dx::XMFLOAT3 viewPosition, dx::XMFLOAT4X4 wvpMatrix, int* indexBuffer)
    {
        int level = 0;
        mIndexCount = 0;
        QTreeNode* curQueue = nullptr;
        QTreeNode* nextQueue = nullptr;

        QTreeNode* rootNode = mRootQueue;
        mFlag[rootNode->indexY*mRowVertexCount + rootNode->indexX] = 1;
        curQueue = rootNode;

        while (level <= mMaxLevel - 1)
        {
            QTreeNode* iterator = curQueue;
            while (iterator)
            {
                QTreeNode* parent = iterator;
                iterator = iterator->next;

                if (cullNodeWithBound(parent, wvpMatrix))
                {
                    int innerStep = calInnerStep(parent);
                    for (int i = -innerStep + 1; i < innerStep; ++i)
                    {
                        ::memset(&mFlag[(parent->indexY + i)*mRowVertexCount
                            + parent->indexX - innerStep + 1], -1, innerStep * 2 - 2);
                    }
                    continue;
                }
                else if (level == mMaxLevel - 1)
                {
                    drawNode(parent, indexBuffer);
                }
                else if (checkNodeCanDivide(parent)
                    && assessNodeCanDivide(parent, viewPosition))
                {
                    for (int i = 0; i < 4; ++i)
                    {
                        enqueue(nextQueue, parent->child[i]);

                        mFlag[parent->child[i]->indexY*mRowVertexCount
                            + parent->child[i]->indexX] = 1;
                    }
                }
                else
                {
                    int step = calChildStep(parent);

                    mFlag[(parent->indexY - step)*mRowVertexCount
                        + parent->indexX - step] = 0;
                    mFlag[(parent->indexY - step)*mRowVertexCount
                        + parent->indexX + step] = 0;
                    mFlag[(parent->indexY + step)*mRowVertexCount
                        + parent->indexX - step] = 0;
                    mFlag[(parent->indexY + step)*mRowVertexCount
                        + parent->indexX + step] = 0;

                    drawNode(parent, indexBuffer);
                }
            }

            curQueue = nextQueue;
            nextQueue = nullptr;

            ++level;
        }
    }

    void LodGenerator::setCoefficient(float c1, float c2) {
        COE_ROUGH = c1;
        COE_DISTANCE = c2;
    }

    int LodGenerator::getLevel() {
        return mMaxLevel;
    }

    float LodGenerator::getCoef1() {
        return COE_ROUGH;
    }

    float LodGenerator::getCoef2() {
        return COE_DISTANCE;
    }


    TerrainVertexData *LodGenerator::getVertices() {
        return mVertices;
    }

    int LodGenerator::getVertexCount() {
        return mVertexCount;
    }

    int LodGenerator::getRowVertexCount() {
        return mRowVertexCount;
    }

    int* LodGenerator::getIndices() {
        return mIndices;
    }

    int LodGenerator::getIndexCount() {
        return mIndexCount;
    }

    int LodGenerator::getMaxIndexCount() {
        return mMaxIndexCount;
    }

}