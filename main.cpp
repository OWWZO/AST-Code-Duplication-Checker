#include <QtWidgets>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>
#include <stack>
#include <queue>
#include <QMainWindow>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFont>
#include <string>
#include <exception>
#include <QTextOption>
#include <QApplication>
#include <QWheelEvent>
using namespace std;
std::hash<std::string> hasher;
class ASTNode {
public:
    string value; 
    size_t cachedHash = 0;
    ASTNode* parent;
    vector<ASTNode*> children;

    ASTNode(string a) {
        value = a;
        parent = NULL;
        cachedHash = hasher(value);
    }
    size_t hash(){
        return cachedHash;
    }
    void addChild(ASTNode* child) {
        children.push_back(child);
    }
};
size_t calculateSubtreeHash(ASTNode* root) {
    size_t sum = hasher(root->value);
    for (auto child : root->children) {
        sum += calculateSubtreeHash(child) ;
    }
    return root->cachedHash=sum;
}

void traverse (ASTNode* node, bool isFirstTree, map<size_t, pair<int, int>>& hashCount){
    if (!node) return;

    if (isFirstTree) {
        hashCount[node->hash()].first++;
    }
    else {
        hashCount[node->hash()].second++;
    }

    for (auto child : node->children) {
        traverse(child, isFirstTree,hashCount);
      }
    };
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
    if(commonBranches-1>0)
    commonBranches--;
    if(totalBranches-2>0)
    totalBranches -= 2;

    return totalBranches > 0 ? (2.0 * commonBranches) / totalBranches : 0.0;
}


vector<string> tokenize(const string& code) {
    vector<string> tokens;
    string token;
    bool inString = false, inChar = false, inComment = false;
    //分别为 字符串 字符 注释
    for (int i = 0; i < code.size(); ++i) {
        char c = code[i];
        if (c == '#' && token.empty()) {//头文件
            while (i < code.size() && code[i] != '\n') {
                token += code[i++];
            }
            tokens.push_back(token);
            token.clear();
            continue;
        }

        if (inComment) {
            if (c == '\n') inComment = false;
            continue;
        }

        if (inString) {
            token += c;
            if (c == '"' && code[i - 1] != '\\') {//确保不是转义
                inString = false;
                tokens.push_back(token);
                token.clear();
            }
            continue;
        }

        if (inChar) {
            token += c;
            if (c == '\'' && (i == 0 || code[i - 1] != '\\')) {
                inChar = false;
                tokens.push_back(token);
                token.clear();
            }
            continue;
        }

        if (c == '"') {
            inString = true;
            token += c;
            continue;
        }

        if (c == '\'') {//遇到'
            inChar = true;
            token += c;
            continue;
        }


        bool isMultiChar = false;
        if (!isspace(c)) {
            // 先检查三位运算符 如 >>=、<<=
            if (i + 2 < code.size()) {
                string three_chars = string(1, c) + code[i + 1] + code[i + 2];
                if (three_chars == ">>=" || three_chars == "<<=") {
                    if (!token.empty()) {
                        tokens.push_back(token);
                        token.clear();
                    }
                    tokens.push_back(three_chars);
                    i += 2;
                    isMultiChar = true;
                }
            }

            // 检查两位运算符
            if (!isMultiChar && i + 1 < code.size()) {
                string two_chars = string(1, c) + code[i + 1];
                if (two_chars == "++" || two_chars == "--" || two_chars == "&&" || two_chars == "||" ||
                    two_chars == "==" || two_chars == "!=" || two_chars == ">=" || two_chars == "<=" ||
                    two_chars == "+=" || two_chars == "-=" || two_chars == "*=" || two_chars == "/=" ||
                    two_chars == "%=" || two_chars == "->" || two_chars == "<<" || two_chars == ">>") {
                    if (!token.empty()) {
                        tokens.push_back(token);
                        token.clear();
                    }
                    tokens.push_back(two_chars);
                    i++;
                    isMultiChar = true;
                }
            }

            // 检查单独的特殊字符（需要优先于单字符判断）
            if (!isMultiChar) {
                if (c == ':' && i + 1 < code.size() && code[i + 1] == ':') {  // 处理C++范围运算符::
                    if (!token.empty()) {
                        tokens.push_back(token);
                        token.clear();
                    }
                    tokens.push_back("::");
                    i++;
                    isMultiChar = true;
                }
            }
        }

        if (isMultiChar) continue;

        if (c == '/' && i + 1 < code.size() && code[i + 1] == '/') {
            inComment = true;
            i++;
            continue;
        }

        if (isspace(c)) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            continue;
        }
        if (c == ';' || c == '(' || c == ')' || c == '{' || c == '}' ||
            c == ',' || c == '=' || c == '<' || c == '>' ||
            c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
            c == '&' || c == '|' || c == '^' || c == '~' || c == '[' ||
            c == ']' || c == '.' || c == '!' || c == '?' || c == ':' || c == '#') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            tokens.push_back(string(1, c));
        }
        else {
            token += c;
        }
    }

    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}

const set<string> keywords = {
    "break", "case", "char",  "continue", "default",
    "double", "else", "float", "for", "if",
    "int", "long",  "return", "short",
    "switch", "void","while", "printf", "scanf", "main"
};
bool isDataType(const string& token) {
    return keywords.find(token) != keywords.end();
}
string process_format_string(const string& s) {
    string output;
    bool in_escape = false;// 是否处于转义状态

    for (size_t i = 0; i < s.size();) {
        if (in_escape) {
            in_escape = false;
            i++;
            continue;
        }
        //  识别反斜杠 进入转义状态
        if (s[i] == '\\') {
            in_escape = true;
            i++;
            continue;
        }

        // 处理双百分号 %% 转义为单个百分号
        if (i + 1 < s.size() && s[i] == '%' && s[i + 1] == '%') {
            i += 2;
            continue;
        }

       // 处理格式化占位符 如'%d', '%.2f'
        if (s[i] == '%') {
            size_t start = i;
            size_t end = i + 1;
            bool has_valid_specifier = false;

            while (end < s.size()) {
                const char c = s[end];

                
                if (isalnum(c) || c == '.') {
                    // 记录是否包含有效类型说明符 必须是字母
                    if (isalpha(c)) has_valid_specifier = true;
                    end++;
                }
                else {
                    break;
                }
            }

            // 如果占位符包含有效类型说明符 则添加到输出
            if (has_valid_specifier) {
                output += s.substr(start, end - start) + " ";
            }
            i = end;
        }
        else {
            output += s[i];
            i++;
        }
    }

    return output.empty() ? "" : output.substr(0, output.size());
}



// 代码预处理器
string preprocess(const string& code) {
    vector<string> tokens = tokenize(code);
    string result;
    bool inString = false;
    bool inChar = false;

    for (size_t i = 0; i < tokens.size(); ++i) {
        string token = tokens[i];
        if (token.find("#include")!=-1) {
            // 跳过整个include声明
            continue;
        }

        if (token.front() == '"') {
            string content;
            // 合并后续字符串片段
            while (i < tokens.size()) {
                content += tokens[i].substr(1);  // 去掉开头引号
                if (tokens[i].back() == '"') {
                    content.pop_back();  // 去掉结尾引号
                    break;
                }
                i++;
            }
            // 处理格式字符串
            string processed = process_format_string(content);
            result += "\"" + processed + "\" ";
            continue;
        }


        if (token[0] == '\'') {
            inChar = true;
            result += token + " ";
            if (token.back() == '\'' && token[token.size() - 2] != '\\') inChar = false;
            continue;
        }
        //如果是字符串或字符字面量  直接添加token
        if (inString || inChar) {
            result += token + " ";
            continue;
        }

        // 替换标识符
        if (isalpha(token[0]) || token[0] == '_') {
            if (keywords.find(token) == keywords.end()) {
                result += "var ";
                continue;
            }
        }

        result += token + " ";
    }

    // 移除多余空格
    string cleaned;
    bool space = false;
    for (char c : result) {
        if (isspace(c)) {
            if (!space) cleaned += ' ';
            space = true;
        }
        else {
            cleaned += c;
            space = false;
        }
    }
    return cleaned;
}
map<string, int> g_precedence{
    {"=", 0},      
    {"+=", 1},    
    {"-=", 1},   
    {"*=", 1},    
    {"/=", 1},     
    {"%=", 1}, 
    {">>=", 1},
    {"<<=", 1},
    {"||", 2}, 
    {"&&", 3},         
    {"<", 5},    
    {">", 5},
    {"<=", 5},
    {">=", 5},
    {"==", 5},
    {"!=", 5},
    {"+", 6},   
    {"-", 6},
    {"*", 7},     
    {"/", 7},
    {"%", 7},
    {"++", 8},   
    {"--", 8}    
};

ASTNode* parseFunctionCall(const vector<std::string>& tokens, int& pos, stack<ASTNode*>& nodeStack) {
    string funcName = tokens[pos++];
    ASTNode* funcNode = new ASTNode(funcName);
    pos++;
    ASTNode* formatNode = new ASTNode(tokens[pos++]);
    string total;
    while (pos < tokens.size() && tokens[pos] != ")") {
        if (tokens[pos] == "," || tokens[pos] == "&") {
            pos++;
            continue;
        }
        total += " " + tokens[pos];
        pos++;
    }
    formatNode->addChild(new ASTNode(total));
    pos++;
    funcNode->addChild(formatNode);
    return funcNode;
}
void applyOperator(stack<ASTNode*>& nodeStack, stack<string>& opStack) {
    string op = opStack.top();//取出操作符
    opStack.pop();

    ASTNode* node = new ASTNode(op);
    if (op == "!") {
        node->addChild(nodeStack.top());
        nodeStack.pop();
    }
    else {
        int flag1 = 0;
        int flag2 = 0;
        ASTNode* left = NULL;
        ASTNode* right = NULL;
        if (nodeStack.size()) {
            right = nodeStack.top();
            nodeStack.pop();
            flag1 = 1;
        }
        if (nodeStack.size()) {
            left = nodeStack.top();
            nodeStack.pop();
            flag2 = 1;
        }
        if (flag2)
            node->addChild(left);

        if (flag1)
            node->addChild(right);

    }
    nodeStack.push(node);
}
ASTNode* parseSubExpression(const vector<string>& tokens, int& pos) {
    stack<ASTNode*> nodeStack;
    stack<string> opStack;

    while (pos < tokens.size()) {
        string token = tokens[pos];
        if (token == ";" || token == ")") break;

        if (token == "(") {
            opStack.push(token);
            pos++;
        }
        else if (g_precedence.count(token)) {
            while (!opStack.empty() && opStack.top() != "(" && //栈非空、栈顶不是左括号
                g_precedence.at(opStack.top()) >= g_precedence.at(token)) {//栈顶运算符优先级 >= 当前运算符优先级
                applyOperator(nodeStack, opStack);
            }
            opStack.push(token);
            pos++;
        }
        else {
            if (pos + 1 < tokens.size() && tokens[pos + 1] == "(") {
                nodeStack.push(parseFunctionCall(tokens, pos, nodeStack));//专门处理while（）里加scanf
            }
            else {
                nodeStack.push(new ASTNode(token));
                pos++;
            }
        }
    }

    while (!opStack.empty()) {
        applyOperator(nodeStack, opStack);
    }

    return nodeStack.empty() ? nullptr : nodeStack.top();
}
ASTNode* buildExpressionTree(const vector<string>& tokens, int& pos) {
    ASTNode* root = new ASTNode("condition");
    root->addChild(parseSubExpression(tokens, pos));
    return root;
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

ASTNode* parseCode(const vector<string>& tokens) {
    int pos = 0;
    stack<ASTNode*> blockStack;
    ASTNode* mainNode = NULL;
        while (pos < tokens.size()) {
            string token = tokens[pos];
           if (token == "int" && pos + 1 < tokens.size() && tokens[pos + 1] == "main") {
               pos += 2;
                mainNode = new ASTNode("main");
                blockStack.push(mainNode);
                pos += 2;

           }
           else if (token == "return") {
               pos += 3;
               break;
            }
            else if (token == "int" || token == "float"|| token == "char") {
                string typeName = token;
                pos++; // 跳过类型关键字

                while (pos < tokens.size() && tokens[pos] != ";") {
                    int flag = 0;
                    ASTNode* varDecl = NULL;
                    for (int i = 0; i < blockStack.top()->children.size(); i++) {
                        if (blockStack.top()->children[i]->value == "variable") {
                            varDecl = blockStack.top()->children[i];
                            flag = 1;
                            break;
                        }
                    }
                    if (flag == 0) {
                        varDecl = new ASTNode("variable");
                        blockStack.top()->addChild(varDecl);
                    }

                    // 创建类型节点
                    ASTNode* typeNode = new ASTNode(typeName);

                    if (tokens[pos] == "var") {
                      
                        string varName = tokens[pos++];

                        ASTNode* expNode = new ASTNode("exp");;


                        if (pos < tokens.size() && tokens[pos] == "=") {
                            typeNode->addChild(expNode);
                            // 构建赋值表达式树

                            vector<string> exprTokens = { varName ,"=" };
                            pos++; // 跳过=

                            // 收集右值表达式
                            while (pos < tokens.size() && tokens[pos] != "," && tokens[pos] != ";") {
                                exprTokens.push_back(tokens[pos++]);
                            }

                            // 生成表达式树
                            int exprPos = 0;
                            if (ASTNode* expr = parseSubExpression(exprTokens, exprPos)) {
                                expNode->addChild(expr);
                            }
                            int flag = 1;
                            for (int i = 0; i < varDecl->children.size(); i++) {
                                if (isSameAST(varDecl->children[i], typeNode)) {
                                    flag = 0;
                                    break;
                                }
                            }
                            if (flag) {
                                varDecl->addChild(typeNode);
                            }
                           
                            while (tokens[pos] != ";") {
                                if (tokens[pos] == ",") {
                                    pos++;
                                    exprTokens.clear();
                            while (pos < tokens.size() && tokens[pos] != "," && tokens[pos] != ";") {
                                        exprTokens.push_back(tokens[pos++]);
                                    }
                                    exprPos = 0;
                      if (ASTNode* expr = parseSubExpression(exprTokens,exprPos)) {
                                 int flag5 = 1;
                       for (int i = 0; i < expNode->children.size(); i++) {
                              if (isSameAST(expr, expNode->children[i])) {
                                       flag5 = 0;
                                            }
                                      }
                                if (flag5) {
                                            expNode->addChild(expr);
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            expNode->value = token;
                            varDecl->addChild(expNode);
                        }
                    }

                    if (pos < tokens.size() && tokens[pos] == ",") pos++;
                    else break;
                }
                if (pos < tokens.size() && tokens[pos] == ";") pos++;

            }
        
            else if (token == "{") {
                ASTNode* newBlock = new ASTNode("block");
                blockStack.top()->addChild(newBlock);
                if (blockStack.size())
                    blockStack.pop();
                blockStack.push(newBlock);
                pos++;
            }
            else if (token == "}") {
                blockStack.pop();
                pos++;
            }
            else if (token == "if") {
                ASTNode* ifNode = new ASTNode("if");
                pos++; // Skip 'if'

                pos++; // Skip '('

                ifNode->addChild(buildExpressionTree(tokens, pos));

                pos++; // Skip ')'

                blockStack.top()->addChild(ifNode);
                blockStack.push(ifNode);//进入if作用域
            }
            else if (token == "else") {

                ASTNode* elseNode = blockStack.top();
                pos++; // Skip 'else'

                if (tokens[pos] == "if") {  // 处理else if
                    pos++; // Skip 'if'
                    ASTNode* newIf = new ASTNode("elseif");

                    pos++; // Skip '('

                    newIf->addChild(buildExpressionTree(tokens, pos));

                    pos++; // Skip ')'

                    elseNode->addChild(newIf);
                    blockStack.push(newIf);
                }
                else {  // 普通else
                    ASTNode* elseBlock = new ASTNode("else");
                    elseNode->addChild(elseBlock);
                    blockStack.push(elseBlock);
                }
            }
            else if (token == "for") {
                ASTNode* forNode = new ASTNode("for");
                pos++; // Skip 'for'
                pos++; // Skip '('

                if (tokens[pos] != ";") {
                    forNode->addChild(buildExpressionTree(tokens, pos));
                }
                pos++; // Skip ';'

                // Parse condition
                if (tokens[pos] != ";") {
                    forNode->addChild(buildExpressionTree(tokens, pos));
                }
                pos++; // Skip ';'

                // Parse increment
                if (tokens[pos] != ")") {
                    forNode->addChild(buildExpressionTree(tokens, pos));
                }
                pos++; // Skip ')'

                blockStack.top()->addChild(forNode);
                blockStack.push(forNode);
            }
            else if (token == "printf" || token == "scanf") {
                int flag = 1;
                int mowei = 1;
                ASTNode* sentenceNode = NULL;
                ASTNode* printfNode = NULL;
                for (int i = 0; i < blockStack.top()->children.size(); i++) {
                    if (blockStack.top()->children[i]->value == "sentence") {
                        flag = 0;
                        mowei = 0;
                        sentenceNode = blockStack.top()->children[i];
                        break;
                    }
                }
                if (flag) {
                    sentenceNode = new ASTNode("sentence");
                    printfNode = new ASTNode(token);
                    sentenceNode->addChild(printfNode);
                }
                else {
                    int flag1 = 1;
                    for (int i = 0; i < sentenceNode->children.size(); i++) {
                        if (sentenceNode->children[i]->value == token) {
                            printfNode = sentenceNode->children[i];
                            flag1 = 0;
                            break;
                        }
                    }
                    if (flag1) {
                        printfNode = new ASTNode(token);
                        sentenceNode->addChild(printfNode);
                    }
                }
                pos++; // 跳过printf

                // 处理格式字符串
                pos++; // 跳过(

                int flag4 = 1;
                ASTNode* formatNode = NULL;
                for (int i = 0; i < printfNode->children.size(); i++) {
                    if (printfNode->children[i]->value == tokens[pos]) {
                        formatNode = printfNode->children[i];
                        pos++;
                        flag4 = 0;
                        break;
                    }
                }
                if (flag4) {
                    formatNode = new ASTNode(tokens[pos++]);//++
                    printfNode->addChild(formatNode);
                }
                // 参数收集

                // 跳过格式字符串后的逗号
                if (tokens[pos] == ",") pos++;

                // 遍历变量参数
                string total;
                while (pos < tokens.size() && tokens[pos] != ")") {
                    if (tokens[pos] == ",") {
                        pos++; // 跳过逗号
                        continue;
                    }

                    if (tokens[pos] != "&")
                        total += " " + tokens[pos++];
                    else {
                        pos++;
                    }
                }
                int flag2 = 1;
                for (int i = 0; i < formatNode->children.size(); i++) {
                    if (formatNode->children[i]->value == total) {
                        flag2 = 0;
                        break;
                    }
                }
                if (flag2)
                    formatNode->addChild(new ASTNode(total));
                // 收尾处理
                if (pos >= tokens.size() || tokens[pos] != ")")
                    throw runtime_error("Expected ')'");
                pos++; // 跳过)

                if (pos >= tokens.size() || tokens[pos] != ";")
                    throw runtime_error("Expected ';'");
                pos++; // 跳过;
                if (mowei)
                    blockStack.top()->addChild(sentenceNode);
            }

            else if (token == "var") {
                int flag = 1;
                ASTNode* expNode = NULL;
                for (int i = 0; i < blockStack.top()->children.size(); i++) {
                    if (blockStack.top()->children[i]->value == "exp") {
                        flag = 0;
                        expNode = blockStack.top()->children[i];
                        break;
                    }
                }
                if(flag)
                expNode = new ASTNode("exp");;
                pos++; // Skip 'var'

                vector<string> exprTokens = { token ,tokens[pos] };
                pos++;
                while (pos < tokens.size() && tokens[pos] != "," && tokens[pos] != ";") {
                    exprTokens.push_back(tokens[pos++]);
                }
                int exprPos = 0;
                if (ASTNode* expr = parseSubExpression(exprTokens, exprPos)) {
                    expNode->addChild(expr);
                }

                if (pos >= tokens.size() || tokens[pos] != ";")
                    throw runtime_error("Expected ';' after expression");
                pos++; // Skip ';'
                if(flag)
                blockStack.top()->addChild(expNode);
            }
            else if (token == "while") {
                ASTNode* whileNode = new ASTNode("while");
                pos++; // Skip 'while'

                pos++; // Skip '('

                whileNode->addChild(buildExpressionTree(tokens, pos));

                pos++; // Skip ')'

                blockStack.top()->addChild(whileNode);
                blockStack.push(whileNode);
            }
            else if (token == "switch") {
                ASTNode* switchNode = new ASTNode("switch");
                pos++; // Skip 'switch'
                pos++; // Skip '('
                switchNode->addChild(buildExpressionTree(tokens, pos));
                pos++; // Skip ')'
                // 创建cases容器节点
                ASTNode* casesNode = new ASTNode("cases");
                switchNode->addChild(casesNode);

                // 压入块栈
                blockStack.top()->addChild(switchNode);
                blockStack.push(casesNode); // 当前作用域为cases容器
}
            else if (token == "case" || token == "default") {
                // 创建case/default节点
                ASTNode* caseNode = new ASTNode(token);
                pos++; 

                // 解析case值
                if (token == "case") {
                    vector<string> constExpr;
                    while (pos < tokens.size() && tokens[pos] != ":") {
                        constExpr.push_back(tokens[pos++]);
                    }
                    int exprPos = 0;
                    if (ASTNode* constVal = parseSubExpression(constExpr, exprPos)) {
                        caseNode->addChild(constVal);
                    }
                }
                pos++; // Skip ':'

                // 创建case块并压栈
                ASTNode* caseBlock = new ASTNode("block");
                caseBlock->parent = caseNode;
                caseNode->addChild(caseBlock);
                blockStack.top()->addChild(caseNode);
                blockStack.push(caseBlock); // 进入case作用域
}
            else if (token == "break") {
               
    ASTNode* breakNode = new ASTNode("break");
    blockStack.top()->addChild(breakNode);
    pos++; 

    if (!blockStack.empty() && blockStack.top()->parent!=NULL &&
        blockStack.top()->value == "block" &&
        blockStack.top()->parent->value == "case") {
        blockStack.pop(); 
    }
    pos++;
    }
        }

    return mainNode;
}

class ASTVisualizer {
public:
    void clearColors() {
        nodeColors.clear();
    }
    void compareAndHighlight(ASTNode* root1, ASTNode* root2) {
        clearColors();
        // 收集两棵树的所有节点哈希
        calculateSubtreeHash(root1);
        calculateSubtreeHash(root2);

        collectHashes(root1, true);
        collectHashes(root2, false);
        // 标记相同哈希的子树
        markSimilarSubtrees();
    }
    void visualize(ASTNode* root, QGraphicsScene* scene, int baseX = 0, int baseY = 0) {
        if (!root) return;
        NodeLayoutInfo layout;
        // 使用传入的baseY作为初始Y坐标
        calculateLayout(root, layout, baseX, baseY);
        drawLayout(root, scene, layout);
    }
    void collectHashes(ASTNode* node, bool isFirstTree) {
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


    void markSimilarSubtrees() {
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
private:
    QColor generateColor(size_t hash) {
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

    QHash<size_t, QPair<QVector<ASTNode*>, QVector<ASTNode*>>> hashMap;
    QHash<ASTNode*, QColor> nodeColors;
    
    struct LayoutConfig {
        const int nodeWidth = 150;// 节点宽度
        const int nodeHeight = 50;// 节点高度
        const int horizontalMargin = 40; // 水平边距
        const int verticalSpacing = 80;// 垂直间距
        const QColor lineColor = QColor("#605e5c"); //连接线颜色
    };


    // 节点布局信息
    struct NodeLayoutInfo {
        QRect rect; // 节点矩形区域
        QVector<NodeLayoutInfo> children;
    };

    LayoutConfig cfg;// 布局配置实例

    int calculateLayout(ASTNode* node, NodeLayoutInfo& layout, int x, int y) {
        if (!node) return x;

        // 初始化当前X坐标为传入的起始X坐标
        int currentX = x;
        // 存储子节点矩形区域的向量
        QVector<QRect> childRects;

        // 遍历当前节点的所有子节点
        for (auto child : node->children) {
            // 为子节点创建布局信息结构体
            NodeLayoutInfo childLayout;
            // 递归计算子节点布局 Y坐标为当前Y+垂直间距
            currentX = calculateLayout(child, childLayout, currentX, y + cfg.verticalSpacing);

            // 更新子节点信息 子节点的矩形区域添加到列表 用于下列计算
            childRects << childLayout.rect;

            // 将子节点布局信息添加到当前节点的子布局列表
            layout.children.append(childLayout);
        }

        // 获取节点宽度
        const int nodeWidth = cfg.nodeWidth;
        // 计算当前节点的X坐标 居中显示在子节点上方
        const int nodeX = childRects.isEmpty() ? x :
            (childRects.first().left() + childRects.last().right() - nodeWidth) / 2;

        // 设置当前节点的矩形区域 包含位置和尺寸
        layout.rect = QRect(nodeX, y, nodeWidth, cfg.nodeHeight);

        // 返回下一个节点的起始X坐标 当前X+水平边距
        return currentX + cfg.horizontalMargin;
    }

    void drawLayout(ASTNode* node, QGraphicsScene* scene, NodeLayoutInfo& layout) {
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


    void drawNode(QGraphicsScene* scene, const QRect& rect, ASTNode* node) {
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

    void drawConnection(QGraphicsScene* scene, QPoint start, QPoint end) {
        QPainterPath path;
        path.moveTo(start);
        const int controlOffset = (end.y() - start.y()) / 2;
        path.cubicTo(start.x(), start.y() + controlOffset,
            end.x(), end.y() - controlOffset,
            end.x(), end.y());

        scene->addPath(path, QPen(cfg.lineColor, 1.2, Qt::DashLine));
    }
};

QTextEdit* createReadOnlyCodeView(const string& code) {
    QTextEdit* view = new QTextEdit;
    view->setPlainText(QString::fromStdString(code));
    view->setReadOnly(true);
    view->setLineWrapMode(QTextEdit::NoWrap);
    view->setStyleSheet(R"(
        QTextEdit {
            background: #f8f9fa;
            border: 1px solid #ced4da;
            font-family: 'Consolas';
            padding: 8px;
        }
    )");
    return view;
}
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        //创建页面按钮
        setupUI();
        //连接信号与槽 设置组件交互
        connectActions();
    }
    string demoCode1 = R"(
#include "ast.h"
#include "ast.h"
int main(){
         while(a>b&&a<c){
       for(int i=0;i<10;i++){
        int a=10;
}
}
    )";

    string demoCode2 = R"(
#include "ast.h"
int main(){
            while(1){
       for(int i=0;i<10;i++){
        int a=10;
}
}

     
    )";
    void setupUI() {
        setWindowTitle("代码查重");
        setMinimumSize(1600, 800);// 设置窗口最小尺寸 宽 高
        setStyleSheet("background-color: #f0f0f0;");


        QWidget* mainWidget = new QWidget;//创建主容器
        QHBoxLayout* mainLayout = new QHBoxLayout; //创建左右分栏

        QVBoxLayout* controlLayout = new QVBoxLayout;//创建垂直布局 控件上下排列
        QWidget* leftPanel = new QWidget;//创建左侧控件容器 承载controlLayout
        leftPanel->setFixedWidth(220);//定宽

       //为左侧垂直布局加按钮
        showSimilarBtn = createButton("显示相似代码树", "#f39c12");// 橙
        importButton = createButton("插入代码", "#9b59b6"); // 紫
        showButton = createButton("展示语法树", "#2ecc71");// 绿
        calcButton = createButton("计算查重率", "#3498db");// 蓝
        preprocessButton = createButton("预处理代码", "#7f8c8d"); // 灰
        clearButton = createButton("清除屏幕", "#e74c3c"); // 红
        controlLayout->addWidget(showSimilarBtn);
        controlLayout->addWidget(importButton);
        controlLayout->addWidget(calcButton);
        controlLayout->addWidget(showButton);
        controlLayout->addWidget(preprocessButton);
        controlLayout->addWidget(clearButton);
        controlLayout->addSpacing(20);//分隔

        resultLabel = new QLabel("");
        resultLabel->setAlignment(Qt::AlignCenter); // 内容居中对齐
        //美化信息展示标签
        resultLabel->setStyleSheet(R"(
            QLabel {
                background: #ffffff;
                border: 2px solid #bdc3c7;
                border-radius: 8px;
                padding: 12px;
                font: 14px 'Microsoft YaHei';
                color: #2c3e50;
            }
        )");
        controlLayout->addWidget(resultLabel);
        controlLayout->addStretch(); //底部留白

        leftPanel->setLayout(controlLayout);// 为左侧面板设置垂直布局

        // 右侧可视化区域 right
        view = new QGraphicsView; //用于显示图形
        view->viewport()->installEventFilter(this);
        scene = new QGraphicsScene(this);//承载绘制内容
        view->setScene(scene);

        view->setRenderHint(QPainter::Antialiasing, true);
        view->setDragMode(QGraphicsView::ScrollHandDrag); //抗锯齿和拖动
        view->setStyleSheet("background: white; border: 1px solid #bdc3c7;");

        view->setSceneRect(-800, -600, 6600, 6200); //设置场景矩形大小 

      
        mainLayout->addWidget(leftPanel);//左
        mainLayout->addWidget(view, 1);//右

        mainWidget->setLayout(mainLayout);
        setCentralWidget(mainWidget); // 设置主容器为窗口中心部件
    }

    void connectActions() {
        connect(showSimilarBtn, &QPushButton::clicked,
            this, &MainWindow::toggleHighlighting);

        connect(importButton, &QPushButton::clicked,
            this, &MainWindow::showImportDialog);

        connect(calcButton, &QPushButton::clicked,
            this, &MainWindow::calculateRate);

        connect(showButton, &QPushButton::clicked,
            this, &MainWindow::showAST);
        connect(preprocessButton, &QPushButton::clicked,
            this, &MainWindow::showPreprocessedCode);
        connect(clearButton, &QPushButton::clicked, [this] {
            scene->clear();
            resultLabel->setText("清除成功");
            });

    }


private slots:
    //接受信号然后触发
    void showPreprocessedCode() {
  
            // 创建对话框
            QDialog dialog(this);
            dialog.setWindowTitle("预处理后的代码");
            dialog.setMinimumSize(800, 600);

            QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);

            // 创建带滚动区域的文本显示框
            QTextEdit* code1View = createReadOnlyCodeView(preprocess(demoCode1));
            QTextEdit* code2View = createReadOnlyCodeView(preprocess(demoCode2));

            // 添加标签和视图
            mainLayout->addWidget(new QLabel("代码1预处理结果:"));
            mainLayout->addWidget(code1View);
            mainLayout->addWidget(new QLabel("代码2预处理结果:"));
            mainLayout->addWidget(code2View);

            // 显示对话框
            dialog.exec();
      
    }
    void toggleHighlighting() {
        highlightingEnabled = !highlightingEnabled;
        // 更新按钮颜色
        showSimilarBtn->setStyleSheet(QString("background: %1;")
            .arg(highlightingEnabled ? "#f39c12" : "#95a5a6"));

        showAST(); // 重新生成可视化
    }

    void showImportDialog() {
        //创建一个对话框
        QDialog dialog(this);
        dialog.setWindowTitle("选择相应位置插入代码");
        dialog.setMinimumSize(600, 600); // 设定尺寸
        QVBoxLayout* mainLayout = new QVBoxLayout(&dialog); //设置为对话框的主布局

        // 统一文本格式
        QTextOption codeOption;
        codeOption.setAlignment(Qt::AlignLeft);//左对齐
        codeOption.setFlags(QTextOption::Flag::IncludeTrailingSpaces);//保留空格
        codeOption.setWrapMode(QTextOption::NoWrap);//取消自动换行

        // 代码编辑框1配置
        QLabel* label1 = new QLabel("代码1:");
        QTextEdit* code1Edit = new QTextEdit();  // 创建编辑框
        code1Edit->setPlainText(QString::fromStdString(demoCode1));

        code1Edit->setWordWrapMode(QTextOption::NoWrap);  // 禁用单词换行
        code1Edit->setLineWrapMode(QTextEdit::NoWrap);     // 强制横向滚动
        code1Edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);//强制显示水平滚动条
        code1Edit->setTabStopDistance(40); // 设置制表符宽度

        code1Edit->document()->setDefaultTextOption(codeOption);//应用到文档里

        // 代码编辑框2 一样的操作
        QLabel* label2 = new QLabel("代码2:");
        QTextEdit* code2Edit = new QTextEdit();
        code2Edit->setPlainText(QString::fromStdString(demoCode2));
        code2Edit->setAcceptRichText(false);
        code2Edit->setWordWrapMode(QTextOption::NoWrap);
        code2Edit->setLineWrapMode(QTextEdit::NoWrap);
        code2Edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        code2Edit->setTabStopDistance(40);
        code2Edit->document()->setDefaultTextOption(codeOption);


        // 创建按钮盒
        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
            | QDialogButtonBox::Cancel); //有取消和确定

        // 加上所有东西
        mainLayout->addWidget(label1);
        mainLayout->addWidget(code1Edit);
        mainLayout->addWidget(label2);
        mainLayout->addWidget(code2Edit);
        mainLayout->addWidget(buttonBox);

        //关闭对话框并返回Accepted状态
        connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);

        //关闭对话框并返回Rejected状态
        connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);


        if (dialog.exec() == QDialog::Accepted) {
            // 将编辑框中的内容转换为C++字符串，更新变量Code1/Code2
            demoCode1 = code1Edit->toPlainText().toStdString();
            demoCode2 = code2Edit->toPlainText().toStdString();
            resultLabel->setText("插入成功！\n");
        }
    }

    void calculateRate() {  // 移除参数
        try {
            // 在函数内部解析AST
            auto tokens1 = tokenize(preprocess(demoCode1));
            auto tokens2 = tokenize(preprocess(demoCode2));
            ASTNode* root1 = parseCode(tokens1);
            ASTNode* root2 = parseCode(tokens2);

            double rate = calculateDuplicationRate(root1, root2);
            resultLabel->setText(QString("查重率: %1%").arg(rate * 100, 0, 'f', 2));

            delete root1;
            delete root2;
        }
        catch (const exception& e) {
            showError("no", e.what());
        }
    }

    void showAST() {
        scene->clear();
        try {
   
            
            auto tokens1 = tokenize(preprocess(demoCode1));
            ASTNode* astRoot1 = parseCode(tokens1);

            auto tokens2 = tokenize(preprocess(demoCode2));
            ASTNode* astRoot2 = parseCode(tokens2);

            
            ASTVisualizer visualizer;
            if (highlightingEnabled) {
                visualizer.compareAndHighlight(astRoot1, astRoot2);
            }
            else {
                visualizer.clearColors(); // 关闭时清除颜色
            }

            // 清空并重置视图
            view->resetTransform();
            view->setRenderHint(QPainter::Antialiasing, true);
            view->setDragMode(QGraphicsView::ScrollHandDrag);
            view->setInteractive(true);

            // 第一棵树：左侧布局
            visualizer.visualize(astRoot1, scene, 200, 50);
            // 第二棵树：右侧布局
            visualizer.visualize(astRoot2, scene, 200, 850);

            // 自动适配视图
            QRectF totalRect = scene->itemsBoundingRect();
            totalRect.adjust(-200, -200, 300, 300); // 增加边界
            qreal scaleFactor = qMin(
                view->viewport()->width() / totalRect.width() * 3.5,
                view->viewport()->height() / totalRect.height() * 3.5
            );
            view->scale(scaleFactor, scaleFactor);

            // 强制刷新视图
            view->viewport()->update();

            delete astRoot1;
            delete astRoot2;
        }
        catch (const exception& e) {
            showError("可视化错误", e.what());
        }
    }

private:
    // UI组件
    double zoomFactor = 1.0;
    bool highlightingEnabled = false;
    QPushButton* showSimilarBtn;
    QGraphicsView* view;
    QGraphicsScene* scene;
    QLabel* resultLabel;//查重率的框

    QPushButton* preprocessButton;
    QPushButton* calcButton;
    QPushButton* showButton;
    QPushButton* clearButton;
    QPushButton* importButton;

    bool eventFilter(QObject* obj, QEvent* event) {
        if (obj == view->viewport() && event->type() == QEvent::Wheel) {
            QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
            if (wheelEvent->modifiers() & Qt::ControlModifier) {
                // 计算缩放比例
                const double zoomStep = 0.1;
                double zoom = 1.0 + (wheelEvent->angleDelta().y() > 0 ? zoomStep : -zoomStep);

                // 限制缩放范围
                zoomFactor *= zoom;
                zoomFactor = qBound(0.2, zoomFactor, 5.0);  // 限制在20%-500%之间

                // 应用缩放
                QTransform transform;
                transform.scale(zoomFactor, zoomFactor);
                view->setTransform(transform);

                return true; 
            }
        }
        return QMainWindow::eventFilter(obj, event);
    }

    // 工具方法
    QPushButton* createButton(const QString& text, const QString& color) {
        QPushButton* btn = new QPushButton(text);
        btn->setStyleSheet(QString(R"(
            QPushButton {
                background: %1;
                color: white;
                border: none;
                padding: 12px;
                border-radius: 6px;
                font: bold 14px 'Microsoft YaHei';
            }
            QPushButton:hover {
                background: %2;
            }
        )").arg(color).arg(QColor(color).darker(120).name()));
        return btn;
    }

    void showError(const QString& title, const QString& msg) {
        QMessageBox::critical(this, title,
            QString("<b>错误详情：</b><br>%1").arg(msg));
    }

};
    int main(int argc, char* argv[]) {
        QApplication app(argc, argv);
        QFont font("Microsoft YaHei", 10);
        QApplication::setFont(font);

        MainWindow window;
        window.show();

        return app.exec();
    }

#include "main.moc"