/*********************************************************
*  模块名：    LexAnalyser
#  文件名：    LexAnalyser.cpp
*  依赖文件：  LexAnalyser.h
*  作 用：     对于C/C++语言进行词法分析
*  作 者：     陈骁飞
*  时 间：     2020.5.25
*********************************************************/
#include "LexAnalyser.h"
#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <stack>
using namespace std;

/***************************************
* 函数名：  openFile
* 作用：    打开高级程序文件
* 参数：    dir： 高级程序的路径[in]
****************************************/
void openFile(string dir) {
    // ifstream inFile(dir);
    if (!inFile.is_open())
    {
        cout << "未成功打开文件" << endl;
    }
    return inFile;
}

/***************************************
* 函数名：  recordSpace
* 作用：    记录每一行的缩进个数，记录在spaceRecorder中
* 参数：    无
****************************************/
void recordSpace() {
    int it = 0, count = 0;
    while (buffer[it] == ' ' || buffer[it] == '\t') {
        count += (buffer[it++] == '\t') ? TAB_SPACE : 1;
    }
}

/***************************************
* 函数名：  loadBuffer
* 作用：    在Main Buffer中更新一行，并将全局指针指向Main Buffer的首地址
* 参数：    fin:  高级程序文件输入流指针[in/out]
****************************************/
bool loadBuffer(ifstream* fin) {
    if ((*fin).is_open() && !(*fin).eof()) {
        (*fin).getline(buffer, MAX_BUFFER_SIZE);
        curLine++;
        recordSpace();
        return true;
    }
    return false;
}

/***************************************
* 函数名：  init
* 作用：    初始化LexAnalyser，分配相应内存
* 参数：    无
****************************************/
void init() {
    buffer = (char*)malloc(MAX_BUFFER_SIZE * sizeof(1));
    if (buffer!=NULL) {
        memset(buffer, '\0', MAX_BUFFER_SIZE * sizeof(1));
    }
}

/***************************************
* 函数名：  shiftToNextToken
* 作用：    从当前位置开始寻找下一个token的首位字符，存储在buffer[cur]中
* 参数：    无
****************************************/
void shiftToNextToken() {
    while (buffer[cur] == ' ' || buffer[cur] == '\0' || buffer[cur] == '\n' || buffer[cur] == '\r' || buffer[cur] == '\t') {
        if (buffer[cur] == '\0' || buffer[cur] == '\n') { // 如果遇到回车符，或者程序刚刚开始，刷新缓冲区，重置cur指针
            if ((*this->fin).eof()) { // 高级程序读取结束时，直接中断
                return;
            }
            loadBuffer(this->fin);
            cur = 0;
        }
        if (buffer[cur] == ' ' || buffer[cur] == '\t') {
            cur++;
        }
    }
}

/***************************************
* 函数名：  searchForNoteEndType_1
* 作用：    对于多行注释进行处理
* 参数：    无
****************************************/
bool searchForNoteEndType_1() {
    // cout << "进入注释.Line:" << curLine << endl;
    cur += 2;
    while (true) {
        if (buffer[cur] == '*' && buffer[cur + 1] == '/') { // 注释结束
            // cout << "注释结束.Line:" << curLine << endl;
            cur += 2;
            return true;
        }
        else if ((buffer[cur] == '\0' || buffer[cur] == '\n') && (*this->fin).eof()) { // 程序结束
            break;
        }
        else { // 注释内部
            cur++;
            shiftToNextToken();
        }
    }
    return false;
}

/***************************************
* 函数名：  searchForNoteEndType_2
* 作用：    对于单行注释进行处理
* 参数：    无
****************************************/
bool searchForNoteEndType_2() {
    // cout << "进入第二类注释" << endl;
    while (buffer[cur] != '\n' && buffer[cur] != '\0') {
        cur++;
    }
    if (buffer[cur] == '\0' || (*this->fin).eof()) { // 搜索到行尾，或者程序结束
        shiftToNextToken();
        return true;
    }
    else { // 出现错误-非法字符‘\0’
        return false;
    }
}
/***************************************
* 函数名：  extractMacro
* 作用：    提取该宏
* 参数：    无
****************************************/
void extractMacro() {
    macroBin.push_back(string(buffer).substr((long)cur + 1, string(buffer).size() - 1));
    loadBuffer(this->fin);
}
/***************************************
* 函数名：  handleSeperator
* 作用：    对于buffer[cur]所在的界符进行处理
* 参数：    无
****************************************/
void handleSeperator() {
    if (buffer[cur] == ':' && buffer[cur + 1] == ':') { // 特例,::算作运算符，:算作界符
        storeLex(LEX_TYPE::OPERATOR, "::");
        cur += 2;
    }
    else {
        storeLex(LEX_TYPE::SEPERATOR, string(1, buffer[cur]));
        cur++;
    }
}
/***************************************
* 函数名：  isOp
* 作用：    判断是否为运算符
* 参数：    op:     待判定运算符[in]
****************************************/
inline bool isOp(char op) {
    return op == '+' || op == '-' || op == '/' || op == '%' || op == '=' || op == '|' || op == '<' || op == '>' || op == '!' || op == '.' || op == '~' || op == '?' || op == '^';
}

/***************************************
* 函数名：  handleOperator
* 作用：    对于buffer[cur]开始的运算符进行处理
* 参数：    无
****************************************/
void handleOperator() {
    char op1 = buffer[cur];
    switch (op1)
    {
    case '<':
        if (buffer[cur + 1] == '<') {
            if (buffer[cur + 2] == '=') {
                storeLex(LEX_TYPE::OPERATOR, "<<=");
                cur += 3;
            }
            else if (isOp(buffer[cur + 2])) {
                raiseSyntaxError(curLine,SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "<<");
                cur += 2;
            }
        }
        else if (buffer[cur + 1] == '=') {
            if (isOp(buffer[cur + 2])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "<=");
                cur += 2;
            }
        }
        else {
            if (isOp(buffer[cur + 1])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "<");
                cur++;
            }
        }
        break;
    case '>':
        if (buffer[cur + 1] == '>') {
            if (buffer[cur + 2] == '=') {
                storeLex(LEX_TYPE::OPERATOR, ">>=");
                cur += 3;
            }
            else if (isOp(buffer[cur + 2])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, ">>");
                cur += 2;
            }
        }
        else if (buffer[cur + 1] == '=') {
            if (isOp(buffer[cur + 2])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, ">=");
                cur += 2;
            }
        }
        else {
            if (isOp(buffer[cur + 1])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, ">");
                cur++;
            }
        }
        break;
    case '+':
        if (buffer[cur + 1] == '=') {
            if (isOp(buffer[cur + 2])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "+=");
                cur += 2;
            }
        }
        else if (buffer[cur + 1] == '+') {
            if (isOp(buffer[cur + 2])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "++");
                cur += 2;
            }
        }
        else {
            if (isOp(buffer[cur + 1])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "+");
                cur++;
            }
        }
        break;
    case '-':
        if (buffer[cur + 1] == '=') {
            if (isOp(buffer[cur + 2])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "-=");
                cur += 2;
            }
        }
        else if (buffer[cur + 1] == '-') {
            if (isOp(buffer[cur + 2])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "--");
                cur += 2;
            }
        }
        else if (buffer[cur + 1] == '>') {
            if (isOp(buffer[cur + 2])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "->");
                cur += 2;
            }
        }
        else {
            if (isOp(buffer[cur + 1])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "-");
                cur++;
            }
        }
        break;
    case '*':
        if (buffer[cur + 1] == '=') {
            if (isOp(buffer[cur + 2])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "*=");
                cur += 2;
            }
        }
        else {
            if (isOp(buffer[cur + 1])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "*");
                cur++;
            }
        }
        break;
    case '/':
        if (buffer[cur + 1] == '=') {
            if (isOp(buffer[cur + 2])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "/=");
                cur += 2;
            }
        }
        else {
            if (isOp(buffer[cur + 1])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "/");
                cur++;
            }
        }
        break;
    case '%':
        if (buffer[cur + 1] == '=') {
            if (isOp(buffer[cur + 2])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "%=");
                cur += 2;
            }
        }
        else {
            if (isOp(buffer[cur + 1])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "%");
                cur++;
            }
        }
        break;
    case '&':
        if (buffer[cur + 1] == '&') {
            if (isOp(buffer[cur + 2])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "&&");
                cur += 2;
            }
        }
        else if (buffer[cur + 1] == '=') {
            if (isOp(buffer[cur + 2])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "&=");
                cur += 2;
            }
        }
        else {
            if (isOp(buffer[cur + 1])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "&");
                cur++;
            }
        }
        break;
    case '|':
        if (buffer[cur + 1] == '|') {
            if (isOp(buffer[cur + 2])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "||");
                cur += 2;
            }
        }
        else if (buffer[cur + 1] == '=') {
            if (isOp(buffer[cur + 2])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "|=");
                cur += 2;
            }
        }
        else {
            if (isOp(buffer[cur + 1])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "|");
                cur++;
            }
        }
        break;
    case'=':
        if (buffer[cur + 1] == '=') {
            if (isOp(buffer[cur + 2])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "==");
                cur += 2;
            }
        }
        else {
            if (isOp(buffer[cur + 1])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "=");
                cur++;
            }
        }
        break;
    case '?':
        if (isOp(buffer[cur + 1])) {
            raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
        }
        else {
            storeLex(LEX_TYPE::OPERATOR, "?");
            cur++;
        }
        break;
    case'.':
        if (isOp(buffer[cur + 1])) {
            raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
        }
        else {
            storeLex(LEX_TYPE::OPERATOR, ".");
            cur++;
        }
        break;
    case'!':
        if (buffer[cur + 1] == '=') {
            if (isOp(buffer[cur + 2])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "!=");
                cur += 2;
            }
        }
        else {
            if (isOp(buffer[cur + 1])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "!");
                cur++;
            }
        }
        break;
    case'^':
        if (buffer[cur + 1] == '=') {
            if (isOp(buffer[cur + 2])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "^=");
                cur += 2;
            }
        }
        else {
            if (isOp(buffer[cur + 1])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::OPERATOR, "^");
                cur++;
            }
        }
        break;
    case'~':
        if (isOp(buffer[cur + 1])) {
            raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
        }
        else {
            storeLex(LEX_TYPE::OPERATOR, "~");
            cur++;
        }
        break;
    default:
        break;
    }
}
/***************************************
* 函数名：  searchKeyword
* 作用：    查找关键词表是否存在该表项
* 参数：    id:    待匹配表项[in]
****************************************/
bool searchKeyword(string id) {
    for (string kw : keyword) {
        if (kw == id) {
            return true;
        }
    }
    return false;
}
/***************************************
* 函数名：  isValidID
* 作用：    判定是否为字母，数字，下划线
* 参数：    id:    待匹配字符[in]
****************************************/
inline bool isValidID(char id) {
    return (id >= 'a' && id <= 'z') || (id >= 'A' && id <= 'Z') || id == '_' || (id >= '0' && id <= '9');
}
/***************************************
* 函数名：  handlerIdentifier
* 作用：    对于buffer[cur]开始的标识符进行处理
* 参数：    无
****************************************/
void handlerIdentifier() {
    string secondBuffer = "";
    secondBuffer += buffer[cur++];
    while (isValidID(buffer[cur])) {
        secondBuffer += buffer[cur++];
    }
    if (searchKeyword(secondBuffer)) {
        storeLex(LEX_TYPE::KEYWORD, secondBuffer);
    }
    else {
        storeLex(LEX_TYPE::IDENTIFIER, secondBuffer);
    }
}

/***************************************
* 函数名：  handleNumber
* 作用：    对于buffer[cur]开始的数字常量进行处理
* 参数：    无
****************************************/
void handleNumber() {
    string secondBuffer = "";
    if (buffer[cur] == '0' && (buffer[cur + 1] == 'x' || buffer[cur + 1] == 'X' || buffer[cur + 1] == 'O' || buffer[cur + 1] == 'o' || buffer[cur + 1] == 'B' || buffer[cur + 1] == 'b')) {
        switch (buffer[cur + 1])
        {
        case'x':case'X': // 十六进制
            secondBuffer += buffer[cur++];
            secondBuffer += buffer[cur++];
            if ((buffer[cur] >= '0' && buffer[cur] <= '9') || (buffer[cur] >= 'A' && buffer[cur] <= 'F') || (buffer[cur] >= 'a' && buffer[cur] <= 'f')) {
                while ((buffer[cur] >= '0' && buffer[cur] <= '9') || (buffer[cur] >= 'A' && buffer[cur] <= 'F') || (buffer[cur] >= 'a' && buffer[cur] <= 'f')) {
                    secondBuffer += buffer[cur++];
                }
                if (isValidID(buffer[cur])) {
                    raiseSyntaxError(curLine, SYNTAX_ERROR::NUM_CONST_FORMAT_ERROR);
                }
                else {
                    storeLex(LEX_TYPE::CONST, secondBuffer);
                }
            }
            else {
                raiseSyntaxError(curLine, SYNTAX_ERROR::NUM_CONST_UNCOMPELET_ERROR);
            }
            break;
        case'o':case'O': // 八进制
            secondBuffer += buffer[cur++];
            secondBuffer += buffer[cur++];
            if (buffer[cur] >= '0' && buffer[cur] <= '7') {
                while (buffer[cur] >= '0' && buffer[cur] <= '7') {
                    secondBuffer += buffer[cur++];
                }
                if (isValidID(buffer[cur])) {
                    raiseSyntaxError(curLine, SYNTAX_ERROR::NUM_CONST_FORMAT_ERROR);
                }
                else {
                    storeLex(LEX_TYPE::CONST, secondBuffer);
                }
            }
            else {
                raiseSyntaxError(curLine, SYNTAX_ERROR::NUM_CONST_UNCOMPELET_ERROR);
            }
            break;
        case'b':case'B':
            secondBuffer += buffer[cur++];
            secondBuffer += buffer[cur++];
            if (buffer[cur] >= '0' && buffer[cur] <= '1') {
                while (buffer[cur] >= '0' && buffer[cur] <= '1') {
                    secondBuffer += buffer[cur++];
                }
                if (isValidID(buffer[cur])) {
                    raiseSyntaxError(curLine, SYNTAX_ERROR::NUM_CONST_FORMAT_ERROR);
                }
                else {
                    storeLex(LEX_TYPE::CONST, secondBuffer);
                }
            }
            else {
                raiseSyntaxError(curLine, SYNTAX_ERROR::NUM_CONST_UNCOMPELET_ERROR);
            }
            break;
        default:
            break;
        }
    }
    else {
        while (buffer[cur] >= '0' && buffer[cur] <= '9') { // 扫描到第一个不为数字的字符
            secondBuffer += buffer[cur++];
        }
        if (buffer[cur] == '.') { // 浮点
            secondBuffer += buffer[cur++];
            if (buffer[cur] >= '0' && buffer[cur] <= '9') {
                while (buffer[cur] >= '0' && buffer[cur] <= '9') {
                    secondBuffer += buffer[cur++];
                }
                if (buffer[cur] == 'F') {
                    secondBuffer += buffer[cur++];
                    if (isValidID(buffer[cur])) {
                        raiseSyntaxError(curLine, SYNTAX_ERROR::NUM_CONST_FORMAT_ERROR);
                    }
                    else {
                        storeLex(LEX_TYPE::CONST, secondBuffer);
                    }
                }
                else if (buffer[cur] == 'E' || buffer[cur] == 'e') { // 科学计数法
                    secondBuffer += buffer[cur++];
                    if (buffer[cur] == '+' || buffer[cur] == '-') {
                        secondBuffer += buffer[cur++];
                    }
                    if (buffer[cur] >= '0' && buffer[cur] <= '9') {
                        while (buffer[cur] >= '0' && buffer[cur] <= '9') {
                            secondBuffer += buffer[cur++];
                        }
                        if (isValidID(buffer[cur]) || buffer[cur] == '.') {
                            raiseSyntaxError(curLine, SYNTAX_ERROR::NUM_CONST_FORMAT_ERROR);
                        }
                        else {
                            storeLex(LEX_TYPE::CONST, secondBuffer);
                        }
                    }
                    else {
                        if (isValidID(buffer[cur])) {
                            raiseSyntaxError(curLine, SYNTAX_ERROR::NUM_CONST_FORMAT_ERROR);
                        }
                        else {
                            raiseSyntaxError(curLine, SYNTAX_ERROR::NUM_CONST_UNCOMPELET_ERROR);
                        }
                    }
                }
                else if (isValidID(buffer[cur])) {
                    raiseSyntaxError(curLine, SYNTAX_ERROR::NUM_CONST_FORMAT_ERROR);
                }
                else {
                    storeLex(LEX_TYPE::CONST, secondBuffer);
                }
            }
            else {
                if (isValidID(buffer[cur])) {
                    raiseSyntaxError(curLine, SYNTAX_ERROR::NUM_CONST_FORMAT_ERROR);
                }
                else {
                    raiseSyntaxError(curLine, SYNTAX_ERROR::NUM_CONST_UNCOMPELET_ERROR);
                }
            }
        }
        else if (buffer[cur] == 'E' || buffer[cur] == 'e') { // 科学计数法
            secondBuffer += buffer[cur++];
            if (buffer[cur] == '+' || buffer[cur] == '-') {
                secondBuffer += buffer[cur++];
            }
            if (buffer[cur] >= '0' && buffer[cur] <= '9') {
                while (buffer[cur] >= '0' && buffer[cur] <= '9') {
                    secondBuffer += buffer[cur++];
                }
                if (isValidID(buffer[cur]) || buffer[cur]=='.') {
                    raiseSyntaxError(curLine, SYNTAX_ERROR::NUM_CONST_FORMAT_ERROR);
                }
                else {
                    storeLex(LEX_TYPE::CONST, secondBuffer);
                }
            }
            else {
                if (isValidID(buffer[cur])) {
                    raiseSyntaxError(curLine, SYNTAX_ERROR::NUM_CONST_FORMAT_ERROR);
                }
                else {
                    raiseSyntaxError(curLine, SYNTAX_ERROR::NUM_CONST_UNCOMPELET_ERROR);
                }
            }
        }
        else if (buffer[cur] == 'L') { // 为浮点数
            secondBuffer += buffer[cur++];
            if (isValidID(buffer[cur])) {
                raiseSyntaxError(curLine, SYNTAX_ERROR::NUM_CONST_FORMAT_ERROR);
            }
            else {
                storeLex(LEX_TYPE::CONST, secondBuffer);
            }
        }
        else if (isValidID(buffer[cur])) {
            raiseSyntaxError(curLine, SYNTAX_ERROR::NUM_CONST_FORMAT_ERROR);
        }
        else { // 判定为十进制整数
            storeLex(LEX_TYPE::CONST, secondBuffer);
        }
    }
}

/***************************************
* 函数名：  handleCharConst
* 作用：    对于buffer[cur]开始的字符常量进行处理
* 参数：    无
****************************************/
void handleCharConst() {
    string secondBuffer = "";
    if (buffer[(++cur)] == '\\') { // 含有转义字符
        if (buffer[cur + 2] == '\'') {
            secondBuffer += buffer[(++cur)];
            storeLex(LEX_TYPE::CONST, secondBuffer);
            cur += 2;
        }
        else {
            raiseSyntaxError(curLine, SYNTAX_ERROR::CHAR_SINGLE_QUOTE_UNMATCH_ERROR);
        }
    }
    else { // 不含有转义字符
        if (buffer[cur + 1] == '\'') { // 
            secondBuffer += buffer[cur++];
            storeLex(LEX_TYPE::CONST, secondBuffer);
            cur++;
        }
        else {
            raiseSyntaxError(curLine, SYNTAX_ERROR::CHAR_SINGLE_QUOTE_UNMATCH_ERROR);
        }
    }
}

/***************************************
* 函数名：  handleStringConst
* 作用：    对于buffer[cur]开始的字符串常量进行处理
* 参数：    无
****************************************/
void handleStringConst() {
    string secondBuffer = "";
    cur++;
    while (buffer[cur] != '\"') {
        if (buffer[cur] == '\0') { // 一行结束，C/C++不支持字符串换行
            raiseSyntaxError(curLine, SYNTAX_ERROR::STRING_UNCLOSE_ERROR);
        }
        else {
            if (buffer[cur] == '\'') {
                secondBuffer += buffer[cur++];
            }
            secondBuffer += buffer[cur++];
        }
    }
    cur++;
    storeLex(LEX_TYPE::CONST, secondBuffer);
}

/***************************************
* 函数名：  isTemplateClass
* 作用：    查找泛型表是否存在该表项
* 参数：    name:    待匹配表项[in]
****************************************/
bool isTemplateClass(string name) {
    for (string kw : existTemplate) {
        if (name==kw) {
            return true;
        }
    }
    return false;
}

/***************************************
* 函数名：  handleLeftRightAngleBra
* 作用：    对于buffer[cur]为<>的情况，区分泛型和运算符
* 参数：    无
****************************************/
void handleLeftRightAngleBra() {
	auto token = tokenRes[tokenRes.size() - 1];
    if (isTemplateClass(token.second.second)) { // 泛型处理逻辑:查找前一个标识符（类名）是否存在泛型
        char bra = buffer[cur]; // 存储是否为<还是>
        string secondBuffer = "";
        secondBuffer += buffer[cur++];
        while (bra==buffer[cur]) {
            secondBuffer += buffer[cur++];
        }
        if (isOp(buffer[cur])) {
            raiseSyntaxError(curLine,SYNTAX_ERROR::OP_FORMAT_ERROR);
        }
        else {
            for (unsigned int i = 0; i < secondBuffer.size();i++) {
                storeLex(LEX_TYPE::SEPERATOR,string(1, secondBuffer[i]));
            }
        }
    }
    else {
        handleOperator();
    }
}

/***************************************
* 函数名：  analyseToken
* 作用：    启动分块逻辑，对于buffer[cur]开始的词素进行分析
* 参数：    无
****************************************/
void analyseToken() {
    if (buffer[cur] == '\0' || buffer[cur] == '\n') { // 表示程序已经分析到末尾
        return;
    }
    char tmp = buffer[cur];
    switch (tmp)
    {
    case '/':
        if (buffer[cur + 1] == '*') { // 第一类注释
            if (!searchForNoteEndType_1()) {
                raiseSyntaxError(curLine,SYNTAX_ERROR::MULLINE_NOTE_NOT_END_ERROR);
            }
        }
        if (buffer[cur + 1] == '/') {
            if (!searchForNoteEndType_2()) {
                raiseSyntaxError(curLine,SYNTAX_ERROR::UNVALID_CHAR_ERROR);
            }
        }
        else {
            handleOperator();
        }
        break;
    case '#':
        extractMacro(); // 词法分析结果，将宏所在改行完成提取出来
        break;
    case '{':case'}':case'[':case']':case'(':case')':case':':case',':case';':
        handleSeperator();
        break;
    case '<':case'>':
        handleLeftRightAngleBra();
        break;
    case'=':case'?':case'*':case'+':case'-':case'&':case'|':case'%':case'.':case'!':case'~':case'^':
        handleOperator();
        break;
    case'_':case'a':case'b':case'c':case'd':case'e':case'f':case'g':case'h':case'i':
    case'j':case'k':case'l':case'm':case'n':case'o':case'p':case'q':case'r':case's':
    case't':case'u':case'v':case'w':case'x':case'y':case'z':case'A':case'B':case'C':
    case'D':case'E':case'F':case'G':case'H':case'I':case'J':case'K':case'L':case'M':
    case'N':case'O':case'P':case'Q':case'R':case'S':case'T':case'U':case'V':case'W':
    case'X':case'Y':case'Z':
        handlerIdentifier();
        break;
    case'1':case'2':case'3':case'4':case'5':case'6':case'7':case'8':case'9':case'0':
        handleNumber();
        break;
    case'\'':
        handleCharConst();
        break;
    case'\"':
        handleStringConst();
        break;
    default:
        raiseSyntaxError(curLine, SYNTAX_ERROR::UNVALID_CHAR_ERROR);
        break;
    }
}

/***************************************
* 函数名：  lex
* 作用：    从第一个字符开始，启动DFA词法分析周期，制导文件结束
* 参数：    无
****************************************/
void lex() {
    while (!(*fin).eof() || buffer[cur] != '\0') {
        shiftToNextToken();
        analyseToken();
    }
}

/***************************************
* 函数名：  printLexRes
* 作用：    打印词法分析结果
* 参数：    无
****************************************/
void printLexRes() {
    cout << "宏定义：" << endl;
    for (string it : macroBin) {
        cout << it << endl;
    }
    cout << "词法分析词素：" << endl;
    for (pair<int, pair<LEX_TYPE, string>> it : tokenRes) {
        LEX_TYPE type = it.second.first;
        string typeStr = (type == LEX_TYPE::SEPERATOR) ? "LEX_TYPE::SEPERATOR" : ((type == LEX_TYPE::IDENTIFIER) ? "LEX_TYPE::IDENTIFIER" : ((type == LEX_TYPE::OPERATOR) ? "LEX_TYPE::OPERATOR" : (type == LEX_TYPE::CONST) ? "LEX_TYPE::CONST    " : "LEX_TYPE::KEYWORD  "));
        cout << "Line:" << it.first << "  ,Type:" << typeStr << " \t,Content:" << it.second.second << endl;
    }
    cout << "每行开始空格数:" << endl;
    for (pair<int, int> p : spaceRecord) {
        cout << "Line:" << p.first << " , has " << p.second << " Space" << endl;
    }
}

/***************************************
* 函数名：  raiseSyntaxError
* 作用：    进行语法错误的报错处理
* 参数：    line         行号[in]
*           errorId      错误类型[in]
****************************************/
void raiseSyntaxError(int line,SYNTAX_ERROR errorId) {
    switch (errorId)
    {
    case SYNTAX_ERROR::NUM_CONST_FORMAT_ERROR:
        cout << "Line:" << line << ",出现非法数字常量格式错误" << endl;
        abort();
        break;
    case SYNTAX_ERROR::UNVALID_CHAR_ERROR:
        cout << "Line:" << line << ",出现非法字符错误" << endl;
        abort();
        break;
    case SYNTAX_ERROR::STRING_UNCLOSE_ERROR:
        cout << "Line:" << line << ",出现字符串常量格式错误" << endl;
        abort();
        break;
    case SYNTAX_ERROR::CHAR_SINGLE_QUOTE_UNMATCH_ERROR:
        cout << "Line:" << line << ",出现非法字符错误" << endl;
        abort();
        break;
    case SYNTAX_ERROR::OP_FORMAT_ERROR:
        cout << "Line:" << line << "出现非法运算符错误(〃ω〃)" << endl;
        abort();
        break;
    case SYNTAX_ERROR::MULLINE_NOTE_NOT_END_ERROR:
        cout << "Line:" << line << ",发生注释无结尾错误TnT" << endl;
        abort();
        break;
    default:
        break;
    }
}