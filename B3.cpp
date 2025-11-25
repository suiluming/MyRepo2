#include <iostream>
#include <vector>
#include <string>
#include <fstream>

// 定义状态枚举
enum class ParserState {
    OUTSIDE_STRING,
    INSIDE_STRING,
    ESCAPE_SEQUENCE
};

/**
 * 从输入字符串中提取所有被双引号包裹的字符串，并处理转义字符。
 * @param input 包含源代码的字符串
 * @return 提取出的字符串列表
 */
std::vector<std::string> extractStrings(const std::string& input) {
    std::vector<std::string> stringsFound;
    std::string currentString;
    ParserState state = ParserState::OUTSIDE_STRING;

    for (char c : input) {
        switch (state) {
            case ParserState::OUTSIDE_STRING:
                if (c == '"') {
                    // 发现字符串开始，切换状态并准备收集字符
                    state = ParserState::INSIDE_STRING;
                    currentString.clear(); 
                }
                // 忽略其他字符
                break;

            case ParserState::INSIDE_STRING:
                if (c == '\\') {
                    // 发现转义符，切换到转义状态
                    state = ParserState::ESCAPE_SEQUENCE;
                } else if (c == '"') {
                    // 发现字符串结束符，保存结果并切换状态
                    stringsFound.push_back(currentString);
                    state = ParserState::OUTSIDE_STRING;
                } else {
                    // 普通字符，直接添加到当前字符串
                    currentString += c;
                }
                break;

            case ParserState::ESCAPE_SEQUENCE:
                // 根据C++标准处理常见的转义序列
                // 对于我们的问题，核心是处理 \"
                if (c == '"' || c == '\\') {
                    currentString += c;
                } else if (c == 'n') {
                    currentString += '\n';
                } else if (c == 't') {
                    currentString += '\t';
                } else if (c == 'r') {
                    currentString += '\r';
                } else {
                    // 对于其他未定义的转义序列，通常原样保留
                    currentString += '\\';
                    currentString += c;
                }
                // 处理完转义字符，回到字符串内部状态
                state = ParserState::INSIDE_STRING;
                break;
        }
    }

    // 注意：如果文件结束时仍处于 INSIDE_STRING 或 ESCAPE_SEQUENCE 状态，
    // 按照标准，这是一个语法错误。此处我们简单地忽略这种情况。

    return stringsFound;
}

int main() {
    // 从文件读取或直接使用示例字符串
    std::string sourceCode = R"(
        #include <iostream>
        int main() {
            // 这是一个字符串："Hello, World!"
            std::string greeting = "Hello, World!";
            std::string message = "一般学习一门编程语言的时候，基本上第一个程序是输出\"Hello World!\"";
            std::string path = "C:\\Users\\Document\\file.txt"; // 测试反斜杠转义
            std::cout << greeting << std::endl;
            return 0;
        }
    )";

    std::cout << "--- 原始源代码 ---" << std::endl;
    std::cout << sourceCode << std::endl;
    std::cout << "--- 提取到的字符串 ---" << std::endl;

    std::vector<std::string> extracted = extractStrings(sourceCode);

    if (extracted.empty()) {
        std::cout << "未找到任何字符串。" << std::endl;
    } else {
        for (size_t i = 0; i < extracted.size(); ++i) {
            std::cout << i + 1 << ": \"" << extracted[i] << "\"" << std::endl;
        }
    }

    return 0;
}