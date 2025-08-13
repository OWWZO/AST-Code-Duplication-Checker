#include "CodeProcessor.h"
#include <cctype>
#include <algorithm>

vector<string> tokenize(const string& code) {
    vector<string> tokens;
    string token;
    bool inString = false, inChar = false, inComment = false;
    //�ֱ�Ϊ �ַ��� �ַ� ע��
    for (int i = 0; i < code.size(); ++i) {
        char c = code[i];
        if (c == '#' && token.empty()) {//ͷ�ļ�
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
            if (c == '"' && code[i - 1] != '\\') {//ȷ������ת��
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

        if (c == '\'') {//����'
            inChar = true;
            token += c;
            continue;
        }


        bool isMultiChar = false;
        if (!isspace(c)) {
            // �ȼ����λ����� �� >>=��<<=
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

            // �����λ�����
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

            // ��鵥���������ַ�����Ҫ�����ڵ��ַ��жϣ�
            if (!isMultiChar) {
                if (c == ':' && i + 1 < code.size() && code[i + 1] == ':') {  // ����C++��Χ�����::
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

// ����Ԥ������
string preprocess(const string& code) {
    vector<string> tokens = tokenize(code);
    string result;
    bool inString = false;
    bool inChar = false;

    for (size_t i = 0; i < tokens.size(); ++i) {
        string token = tokens[i];
        if (token.find("#include")!=-1) {
            // ��������include����
            continue;
        }

        if (token.front() == '"') {
            string content;
            // �ϲ������ַ���Ƭ��
            while (i < tokens.size()) {
                content += tokens[i].substr(1);  // ȥ����ͷ����
                if (tokens[i].back() == '"') {
                    content.pop_back();  // ȥ����β����
                    break;
                }
                i++;
            }
            // �����ʽ�ַ���
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
        //������ַ������ַ�������  ֱ�����token
        if (inString || inChar) {
            result += token + " ";
            continue;
        }

        // �滻��ʶ��
        if (isalpha(token[0]) || token[0] == '_') {
            if (keywords.find(token) == keywords.end()) {
                result += "var ";
                continue;
            }
        }

        result += token + " ";
    }

    // �Ƴ�����ո�
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
    string op = opStack.top();//ȡ��������
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
            while (!opStack.empty() && opStack.top() != "(" && //ջ�ǿա�ջ������������
                g_precedence.at(opStack.top()) >= g_precedence.at(token)) {//ջ����������ȼ� >= ��ǰ��������ȼ�
                applyOperator(nodeStack, opStack);
            }
            opStack.push(token);
            pos++;
        }
        else {
            if (pos + 1 < tokens.size() && tokens[pos + 1] == "(") {
                nodeStack.push(parseFunctionCall(tokens, pos, nodeStack));//ר�Ŵ���while�������scanf
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
                pos++; // �������͹ؼ���

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

                    // �������ͽڵ�
                    ASTNode* typeNode = new ASTNode(typeName);

                    if (tokens[pos] == "var") {
                      
                        string varName = tokens[pos++];

                        ASTNode* expNode = new ASTNode("exp");;


                        if (pos < tokens.size() && tokens[pos] == "=") {
                            typeNode->addChild(expNode);
                            // ������ֵ���ʽ��

                            vector<string> exprTokens = { varName ,"=" };
                            pos++; // ����=

                            // �ռ���ֵ���ʽ
                            while (pos < tokens.size() && tokens[pos] != "," && tokens[pos] != ";") {
                                exprTokens.push_back(tokens[pos++]);
                            }

                            // ���ɱ��ʽ��
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
                blockStack.push(ifNode);//����if������
            }
            else if (token == "else") {

                ASTNode* elseNode = blockStack.top();
                pos++; // Skip 'else'

                if (tokens[pos] == "if") {  // ����else if
                    pos++; // Skip 'if'
                    ASTNode* newIf = new ASTNode("elseif");

                    pos++; // Skip '('

                    newIf->addChild(buildExpressionTree(tokens, pos));

                    pos++; // Skip ')'

                    elseNode->addChild(newIf);
                    blockStack.push(newIf);
                }
                else {  // ��ͨelse
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
                pos++; // ����printf

                // �����ʽ�ַ���
                pos++; // ����(

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
                // �����ռ�

                // ������ʽ�ַ�����Ķ���
                if (tokens[pos] == ",") pos++;

                // ������������
                string total;
                while (pos < tokens.size() && tokens[pos] != ")") {
                    if (tokens[pos] == ",") {
                        pos++; // ��������
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
                // ��β����
                if (pos >= tokens.size() || tokens[pos] != ")")
                    throw runtime_error("Expected ')'");
                pos++; // ����)

                if (pos >= tokens.size() || tokens[pos] != ";")
                    throw runtime_error("Expected ';'");
                pos++; // ����;
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
                // ����cases�����ڵ�
                ASTNode* casesNode = new ASTNode("cases");
                switchNode->addChild(casesNode);

                // ѹ���ջ
                blockStack.top()->addChild(switchNode);
                blockStack.push(casesNode); // ��ǰ������Ϊcases����
}
            else if (token == "case" || token == "default") {
                // ����case/default�ڵ�
                ASTNode* caseNode = new ASTNode(token);
                pos++; 

                // ����caseֵ
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

                // ����case�鲢ѹջ
                ASTNode* caseBlock = new ASTNode("block");
                caseBlock->parent = caseNode;
                caseNode->addChild(caseBlock);
                blockStack.top()->addChild(caseNode);
                blockStack.push(caseBlock); // ����case������
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