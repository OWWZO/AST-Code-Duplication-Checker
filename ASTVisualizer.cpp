#include "ASTVisualizer.h"
#include "CodeProcessor.h"

// 静态成员初始化
const QColor ASTVisualizer::LayoutConfig::lineColor = QColor("#605e5c");

void ASTVisualizer::clearColors() {
    nodeColors.clear();
}

void ASTVisualizer::compareAndHighlight(ASTNode* root1, ASTNode* root2) {
    clearColors();
    // 收集两棵树的所有节点哈希
    ::calculateSubtreeHash(root1);
    ::calculateSubtreeHash(root2);

    collectHashes(root1, true);
    collectHashes(root2, false);
    // 标记相同哈希的子树
    markSimilarSubtrees();
}

void ASTVisualizer::visualize(ASTNode* root, QGraphicsScene* scene, int baseX, int baseY) {
    if (!root) return;
    ::NodeLayoutInfo layout;
    // 使用传入的baseY作为初始Y坐标
    calculateLayout(root, layout, baseX, baseY);
    drawLayout(root, scene, layout);
}

void ASTVisualizer::collectHashes(ASTNode* node, bool isFirstTree) {
    if (!node) return;

    auto& entry = hashMap[node->hash()];

    if (isFirstTree) {
        entry.first.append(node);
    }
    else {
        entry.second.append(node);
    }

    for (auto child : node->children) {
        collectHashes(child, isFirstTree);
    }
}

void ASTVisualizer::markSimilarSubtrees() {
    QHash<size_t, QPair<QVector<ASTNode*>, QVector<ASTNode*>>> matchedPairs;


    for (auto it = hashMap.begin(); it != hashMap.end(); ++it) {
        auto& firstNodes = it.value().first;
        auto& secondNodes = it.value().second;

        if (!firstNodes.empty() && !secondNodes.empty()) {

            matchedPairs.insert(it.key(), qMakePair(firstNodes, secondNodes));
        }
    }

    for (auto it = matchedPairs.begin(); it != matchedPairs.end(); ++it) {
        QColor Color = generateColor(it.key());
        // 标记第一棵树节点
        for (ASTNode* node : it.value().first) {
            nodeColors[node] = Color;
        }

        // 标记第二棵树节点
        for (ASTNode* node : it.value().second) {
            nodeColors[node] = Color;
        }
    }
}

QColor ASTVisualizer::generateColor(size_t hash) {
    const uint32_t h = hash;
    const double hue = fmod((h & 0xFF) / 255.0 * 360.0, 360.0);
    const double sat = 70 + (h >> 8 & 0x1F);
    const double lum = 45 + (h >> 16 & 0x1F);
    QColor invertedColor = QColor::fromHslF(
        QColor::fromHsl(hue, sat, lum).hslHueF(),
        QColor::fromHsl(hue, sat, lum).hslSaturationF() + 0.5,
        1.0 - QColor::fromHsl(hue, sat, lum).lightnessF()
    );
    return invertedColor;
}

int ASTVisualizer::calculateLayout(ASTNode* node, ::NodeLayoutInfo& layout, int x, int y) {
    if (!node) return x;

    // 初始化当前X坐标为传入的起始X坐标
    int currentX = x;
    // 存储子节点矩形区域的向量
    QVector<QRect> childRects;

    // 遍历当前节点的所有子节点
    for (auto child : node->children) {
        // 为子节点创建布局信息结构体
        ::NodeLayoutInfo childLayout;
        // 递归计算子节点布局 Y坐标为当前Y+垂直间距
        currentX = calculateLayout(child, childLayout, currentX, y + LayoutConfig::verticalSpacing);

        // 更新子节点信息 子节点的矩形区域添加到列表 用于下列计算
        childRects << childLayout.rect;

        // 将子节点布局信息添加到当前节点的子布局列表
        layout.children.append(childLayout);
    }

    // 获取节点宽度
    const int nodeWidth = LayoutConfig::nodeWidth;
    // 计算当前节点的X坐标 居中显示在子节点上方
    const int nodeX = childRects.isEmpty() ? x :
        (childRects.first().left() + childRects.last().right() - nodeWidth) / 2;

    // 设置当前节点的矩形区域 包含位置和尺寸
    layout.rect = QRect(nodeX, y, nodeWidth, LayoutConfig::nodeHeight);

    // 返回下一个节点的起始X坐标 当前X+水平边距
    return currentX + LayoutConfig::horizontalMargin;
}

void ASTVisualizer::drawLayout(ASTNode* node, QGraphicsScene* scene, ::NodeLayoutInfo& layout) {
    drawNode(scene, layout.rect, node);

    // 2. 绘制当前节点到所有子节点的连接线
    for (int i = 0; i < node->children.size(); ++i) {
        // 计算连接线起点：当前节点矩形的中心点X，底部Y坐标
        QPoint start(layout.rect.center().x(), layout.rect.bottom());

        // 计算连接线终点：子节点矩形的中心点X，顶部Y坐标
        QPoint end(layout.children[i].rect.center().x(), layout.children[i].rect.top());

        // 调用drawConnection绘制带贝塞尔曲线的连接线
        drawConnection(scene, start, end);
    }

    //   遍历子节点列表，对每个子节点调用drawLayout
    for (int i = 0; i < node->children.size(); ++i) {
        drawLayout(node->children[i], scene, layout.children[i]);
    }
}

void ASTVisualizer::drawNode(QGraphicsScene* scene, const QRect& rect, ASTNode* node) {
    // 获取节点颜色
    QColor textColor = nodeColors.value(node, Qt::black);
    // 创建文本项并设置节点值
    auto text = scene->addText(QString::fromStdString(node->value));
    // 设置文本宽度
    text->setTextWidth(rect.width() - 10);
    // 设置文本默认颜色
    text->setDefaultTextColor(textColor);

    // 计算文本居中位置
    // 偏移量计算 用于调整文本居中显示
    const QPoint centerPos = rect.center() - QPoint(rect.width() / 2 - 5, rect.height() / 2 - 5);

    // 设置文本在场景中的位置
    text->setPos(centerPos.x(), centerPos.y());

    // 设置文本对齐方式为水平和垂直居中
    text->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter));

    // 获取当前字体并修改样式
    QFont font = text->font();
    // 设置字体为粗体
    font.setBold(true);
    font.setPointSize(8);
    text->setFont(font);
}

void ASTVisualizer::drawConnection(QGraphicsScene* scene, QPoint start, QPoint end) {
    QPainterPath path;
    path.moveTo(start);
    const int controlOffset = (end.y() - start.y()) / 2;
    path.cubicTo(start.x(), start.y() + controlOffset,
        end.x(), end.y() - controlOffset,
        end.x(), end.y());

    scene->addPath(path, QPen(LayoutConfig::lineColor, 1.2, Qt::DashLine));
}

