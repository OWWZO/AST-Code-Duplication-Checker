#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>
#include <stack>
#include <queue>
#include <iostream>
#include <functional>

using namespace std;

// AST�ڵ���
class ASTNode {
public:
    string value; 
    size_t cachedHash = 0;
    ASTNode* parent;
    vector<ASTNode*> children;

    ASTNode(string a);
    size_t hash();
    void addChild(ASTNode* child);
};

// ��������
size_t calculateSubtreeHash(ASTNode* root);
void traverse(ASTNode* node, bool isFirstTree, map<size_t, pair<int, int>>& hashCount);
double calculateDuplicationRate(ASTNode* root1, ASTNode* root2);
bool isSameAST(const ASTNode* root1, const ASTNode* root2); 