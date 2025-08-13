#pragma once

#include <string>
#include <set>
#include <map>
#include <QTextEdit>

// 全局常量声明
extern const std::set<std::string> keywords;
extern std::map<std::string, int> g_precedence;

// 工具函数声明
std::string process_format_string(const std::string& s);
bool isDataType(const std::string& token);
QTextEdit* createReadOnlyCodeView(const std::string& code); 