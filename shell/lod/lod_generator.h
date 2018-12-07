#ifndef SHELL_LOD_LOD_GENERATOR_H_
#define SHELL_LOD_LOD_GENERATOR_H_

#define ALTITUDE_MAP_SIZE 1024

#include "shell/third_party/directx_math/Inc/DirectXMath.h"


namespace shell {

    namespace dx = DirectX;

    class QTreeNode;
    struct TerrainVertexData;

    class LodGenerator {
    public:
        LodGenerator(float edgeLength, int maxLevel);
        ~LodGenerator();

        void setCoefficient(float c1, float c2);

        int getLevel();
        float getCoef1();
        float getCoef2();

        void renderLodTerrain(
            dx::XMFLOAT3 viewPosition, dx::XMFLOAT4X4 wvpMatrix, int* indexBuffer);

        TerrainVertexData* getVertices();
        int getVertexCount();
        int getRowVertexCount();

        int* getIndices();
        int getIndexCount();
        int getMaxIndexCount();

    private:
        void enqueue(QTreeNode*& queue, QTreeNode* node);
        QTreeNode* dequeue(QTreeNode*& queue);
        void clearQueue(QTreeNode*& queue);

        int calInnerStep(QTreeNode* node);
        int calNeighborStep(QTreeNode* node);
        int calChildStep(QTreeNode* node);

        void generateQuadTree();
        void generateRootNodeData(QTreeNode* root);
        void generateChildNodeData(
            QTreeNode* parent, QTreeNode* child, int level, int position);

        void determineRoughAndBound(QTreeNode* node);

        bool checkNodeCanDivide(QTreeNode* node);
        bool assessNodeCanDivide(
            QTreeNode* node, dx::XMFLOAT3 viewPosition);

        void drawNode(QTreeNode* node, int* indexBuffer);

        bool cullNodeWithBound(QTreeNode* node, dx::XMFLOAT4X4 wvpMatrix);
        void constructNodeBound(QTreeNode* node, dx::XMFLOAT3* bound);

        int max_level_;
        int vertex_count_;
        int row_vertex_count_;

        float coe_rough_;
        float coe_distance_;
        QTreeNode* root_queue_;

        char* flags_;
        char* altitude_;
        TerrainVertexData* vertices_;

        int* indices_;
        int index_count_;
        int max_index_count_;
    };

}

#endif  // SHELL_LOD_LOD_GENERATOR_H_