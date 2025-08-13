#include "Utils.h"
#include <cctype>
#include <algorithm>
#include <QString>
#include <QTextOption>

// 全局常量定义
const std::set<std::string> keywords = {
    "break", "case", "char",  "continue", "default",
    "double", "else", "float", "for", "if",
    "int", "long",  "return", "short",
    "switch", "void","while", "printf", "scanf", "main"
};

std::map<std::string, int> g_precedence{
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

bool isDataType(const std::string& token) {
    return keywords.find(token) != keywords.end();
}

std::string process_format_string(const std::string& s) {
    std::string output;
    bool in_escape = false;

    for (size_t i = 0; i < s.size();) {
        if (in_escape) {
            in_escape = false;
            i++;
            continue;
        }
        
        if (s[i] == '\\') {
            in_escape = true;
            i++;
            continue;
        }

        if (i + 1 < s.size() && s[i] == '%' && s[i + 1] == '%') {
            i += 2;
            continue;
        }

        if (s[i] == '%') {
            size_t start = i;
            size_t end = i + 1;
            bool has_valid_specifier = false;

            while (end < s.size()) {
                const char c = s[end];
                
                if (isalnum(c) || c == '.') {
                    if (isalpha(c)) has_valid_specifier = true;
                    end++;
                }
                else {
                    break;
                }
            }

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

QTextEdit* createReadOnlyCodeView(const std::string& code) {
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