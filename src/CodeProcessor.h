#pragma once

#include "ASTNode.h"
#include "Utils.h"
#include <string>
#include <vector>
#include <stack>
#include <stdexcept>

using namespace std;

// 代码处理相关函数声明
vector<string> tokenize(const string& code);
string preprocess(const string& code);
ASTNode* parseCode(const vector<string>& tokens);
ASTNode* parseFunctionCall(const vector<std::string>& tokens, int& pos, stack<ASTNode*>& nodeStack);
void applyOperator(stack<ASTNode*>& nodeStack, stack<string>& opStack);
ASTNode* parseSubExpression(const vector<string>& tokens, int& pos);
ASTNode* buildExpressionTree(const vector<string>& tokens, int& pos); 