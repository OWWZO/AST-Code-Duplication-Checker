#include "ASTNode.h"

std::hash<std::string> hasher;

ASTNode::ASTNode(string a) {
    value = a;
    parent = NULL;
    cachedHash = hasher(value);
}

size_t ASTNode::hash() {
    return cachedHash;
}

void ASTNode::addChild(ASTNode* child) {
    children.push_back(child);
}

size_t calculateSubtreeHash(ASTNode* root) {
    size_t sum = hasher(root->value);
    for (auto child : root->children) {
        sum += calculateSubtreeHash(child);
    }
    return root->cachedHash = sum;
}

void traverse(ASTNode* node, bool isFirstTree, map<size_t, pair<int, int>>& hashCount) {
    if (!node) return;

    if (isFirstTree) {
        hashCount[node->hash()].first++;
    }
    else {
        hashCount[node->hash()].second++;
    }

    for (auto child : node->children) {
        traverse(child, isFirstTree, hashCount);
    }
}

double calculateDuplicationRate(ASTNode* root1, ASTNode* root2) {
    calculateSubtreeHash(root1);
    calculateSubtreeHash(root2);
    map<size_t, pair<int, int>> hashCount;

    // 遍历两棵树
    traverse(root1, true, hashCount);
    traverse(root2, false, hashCount);

    // 计算重复分支和总分支
    int commonBranches = 0;
    int totalBranches = 0;

    for (auto& entry : hashCount) {
        auto& counts = entry.second;
        commonBranches += min(counts.first, counts.second);
        totalBranches += (counts.first + counts.second);
    }
    if (commonBranches - 1 > 0)
        commonBranches--;
    if (totalBranches - 2 > 0)
        totalBranches -= 2;

    return totalBranches > 0 ? (2.0 * commonBranches) / totalBranches : 0.0;
}

bool isSameAST(const ASTNode* root1, const ASTNode* root2) {
    if (!root1 && !root2) return true;

    if (!root1 || !root2) return false;

    if (root1->value != root2->value)
        return false;

    auto& children1 = root1->children;
    auto& children2 = root2->children;

    if (children1.size() != children2.size())
        return false;

    for (size_t i = 0; i < children1.size(); ++i) {
        if (!isSameAST(children1[i], children2[i]))
            return false;
    }

    return true;
}