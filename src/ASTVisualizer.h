#pragma once

#include "ASTNode.h"
#include <QtWidgets>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHash>
#include <QPair>
#include <QVector>
#include <QColor>
#include <QPainterPath>
#include <QPen>
#include <QFont>
#include <QTextOption>
#include <QTextEdit>
#include <QString>
#include <cmath>

// �ڵ㲼����Ϣ
struct NodeLayoutInfo {
    QRect rect; // �ڵ��������
    QVector<NodeLayoutInfo> children;
};

class ASTVisualizer {
public:
    void clearColors();
    void compareAndHighlight(ASTNode* root1, ASTNode* root2);
    void visualize(ASTNode* root, QGraphicsScene* scene, int baseX = 0, int baseY = 0);
    void collectHashes(ASTNode* node, bool isFirstTree);
    void markSimilarSubtrees();

private:
    QColor generateColor(size_t hash);
    int calculateLayout(ASTNode* node, ::NodeLayoutInfo& layout, int x, int y);
    void drawLayout(ASTNode* node, QGraphicsScene* scene, ::NodeLayoutInfo& layout);
    void drawNode(QGraphicsScene* scene, const QRect& rect, ASTNode* node);
    void drawConnection(QGraphicsScene* scene, QPoint start, QPoint end);

    QHash<size_t, QPair<QVector<ASTNode*>, QVector<ASTNode*>>> hashMap;
    QHash<ASTNode*, QColor> nodeColors;
    
    struct LayoutConfig {
        static const int nodeWidth = 150;// �ڵ���
        static const int nodeHeight = 50;// �ڵ�߶�
        static const int horizontalMargin = 40; // ˮƽ�߾�
        static const int verticalSpacing = 80;// ��ֱ���
        static const QColor lineColor;// ��������ɫ
    };

    LayoutConfig cfg;// ��������ʵ��
}; 