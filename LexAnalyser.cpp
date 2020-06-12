/*********************************************************
*  ģ������    LexAnalyser
#  �ļ�����    LexAnalyser.cpp
*  �����ļ���  LexAnalyser.h
*  �� �ã�     ����C/C++���Խ��дʷ�����
*  �� �ߣ�     �����
*  ʱ �䣺     2020.5.25
*  �� ����     Version 2.1
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
* ��������  openFile
* ���ã�    �򿪸߼������ļ�
* ������    dir�� �߼������·��[in]
****************************************/
std::ifstream openFile(std::string dir) {
    std::ifstream  inFile(dir);
    if (!inFile.is_open())
    {
        std::cout << "δ�ɹ����ļ�" << std::endl;
    }
    return inFile;
}

/***************************************
* ��������  recordSpace
* ���ã�    ��¼ÿһ�е�������������¼��spaceRecorder��
* ������    ��
****************************************/
void LexAnalyser::recordSpace() {
    int it = 0, count = 0;
    while (buffer[it] == ' ' || buffer[it] == '\t') {
        count += (buffer[it++] == '\t') ? TAB_SPACE : 1;
    }
    spaceRecord.push_back(pair<int, int>(curLine, count));
}

/***************************************
* ��������  loadBuffer
* ���ã�    ��Main Buffer�и���һ�У�����ȫ��ָ��ָ��Main Buffer���׵�ַ
* ������    fin:  �߼������ļ�������ָ��[in/out]
****************************************/
bool LexAnalyser::loadBuffer(ifstream* fin) {
    if ((*fin).is_open() && !(*fin).eof()) {
        (*fin).getline(buffer, MAX_BUFFER_SIZE);
        curLine++;
        recordSpace();
        return true;
    }
    return false;
}

/***************************************
* ��������  init
* ���ã�    ��ʼ��LexAnalyser��������Ӧ�ڴ�
* ������    ��
****************************************/
void LexAnalyser::init() {
    buffer = (char*)malloc(MAX_BUFFER_SIZE * sizeof(char));
    if (buffer!=NULL) {
        memset(buffer, '\0', MAX_BUFFER_SIZE * sizeof(char));
    }
}

/***************************************
* ��������  shiftToNextToken
* ���ã�    �ӵ�ǰλ�ÿ�ʼѰ����һ��token����λ�ַ����洢��buffer[cur]��
* ������    ��
****************************************/
void LexAnalyser::shiftToNextToken() {
    while (buffer[cur] == ' ' || buffer[cur] == '\0' || buffer[cur] == '\n' || buffer[cur] == '\r' || buffer[cur] == '\t') {
        if (buffer[cur] == '\0' || buffer[cur] == '\n') { // ��������س��������߳���ոտ�ʼ��ˢ�»�����������curָ��
            if ((*this->fin).eof()) { // �߼������ȡ����ʱ��ֱ���ж�
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
* ��������  storeLex
* ���ã�    ���ʷ������Ĵ��ؼ����ʹ洢��tokenRes��
* ������    type:            ���ص�����[in]
*           tokenContent :   ���صľ�������[in]
****************************************/
void LexAnalyser::storeLex(LEX_TYPE type, string tokenContent) {
    // cout << "Store:" << tokenContent << endl;
    tokenRes.push_back(pair<int, pair<LEX_TYPE, string>>(curLine, pair<LEX_TYPE, string>(type, tokenContent)));
}

/***************************************
* ��������  searchForNoteEndType_1
* ���ã�    ���ڶ���ע�ͽ��д���
* ������    ��
****************************************/
bool LexAnalyser::searchForNoteEndType_1() {
    // cout << "����ע��.Line:" << curLine << endl;
    cur += 2;
    while (true) {
        if (buffer[cur] == '*' && buffer[cur + 1] == '/') { // ע�ͽ���
            // cout << "ע�ͽ���.Line:" << curLine << endl;
            cur += 2;
            return true;
        }
        else if ((buffer[cur] == '\0' || buffer[cur] == '\n') && (*this->fin).eof()) { // �������
            break;
        }
        else { // ע���ڲ�
            cur++;
            shiftToNextToken();
        }
    }
    return false;
}

/***************************************
* ��������  searchForNoteEndType_2
* ���ã�    ���ڵ���ע�ͽ��д���
* ������    ��
****************************************/
bool LexAnalyser::searchForNoteEndType_2() {
    // cout << "����ڶ���ע��" << endl;
    while (buffer[cur] != '\n' && buffer[cur] != '\0') {
        cur++;
    }
    if (buffer[cur] == '\0' || (*this->fin).eof()) { // ��������β�����߳������
        shiftToNextToken();
        return true;
    }
    else { // ���ִ���-�Ƿ��ַ���\0��
        return false;
    }
}
/***************************************
* ��������  extractMacro
* ���ã�    ��ȡ�ú�
* ������    ��
****************************************/
void LexAnalyser::extractMacro() {
    macroBin.push_back(string(buffer).substr((size_t)cur + 1, string(buffer).size() - 1));
    loadBuffer(this->fin);
}
/***************************************
* ��������  handleSeperator
* ���ã�    ����buffer[cur]���ڵĽ�����д���
* ������    ��
****************************************/
void LexAnalyser::handleSeperator() {
    if (buffer[cur] == ':' && buffer[cur + 1] == ':') { // ����,::�����������:�������
        storeLex(LEX_TYPE::OPERATOR, "::");
        cur += 2;
    }
    else {
        storeLex(LEX_TYPE::SEPERATOR, string(1, buffer[cur]));
        cur++;
    }
}
/***************************************
* ��������  isOp
* ���ã�    �ж��Ƿ�Ϊ�����
* ������    op:     ���ж������[in]
****************************************/
inline bool isOp(char op) {
    return op == '+' || op == '-' || op == '/' || op == '%' || op == '=' || op == '|' || op == '<' || op == '>' || op == '!' || op == '.' || op == '~' || op == '?' || op == '^';
}
/***************************************
* ��������  handleOperator
* ���ã�    ����buffer[cur]��ʼ����������д���
* ������    ��
****************************************/
void LexAnalyser::handleOperator() {
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
    case'[':
        if (isOp(buffer[cur + 1])) {
            raiseSyntaxError(curLine, SYNTAX_ERROR::OP_FORMAT_ERROR);
        }
        else {
            storeLex(LEX_TYPE::OPERATOR, "[");
            cur++;
        }
        break;
    default:
        break;
    }
}
/***************************************
* ��������  searchKeyword
* ���ã�    ���ҹؼ��ʱ��Ƿ���ڸñ���
* ������    id:    ��ƥ�����[in]
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
* ��������  isValidID
* ���ã�    �ж��Ƿ�Ϊ��ĸ�����֣��»���
* ������    id:    ��ƥ���ַ�[in]
****************************************/
inline bool isValidID(char id) {
    return (id >= 'a' && id <= 'z') || (id >= 'A' && id <= 'Z') || id == '_' || (id >= '0' && id <= '9');
}
/***************************************
* ��������  handlerIdentifier
* ���ã�    ����buffer[cur]��ʼ�ı�ʶ�����д���
* ������    ��
****************************************/
void LexAnalyser::handlerIdentifier() {
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
* ��������  handleNumber
* ���ã�    ����buffer[cur]��ʼ�����ֳ������д���
* ������    ��
****************************************/
void LexAnalyser::handleNumber() {
    string secondBuffer = "";
    if (buffer[cur] == '0' && (buffer[cur + 1] == 'x' || buffer[cur + 1] == 'X' || buffer[cur + 1] == 'O' || buffer[cur + 1] == 'o' || buffer[cur + 1] == 'B' || buffer[cur + 1] == 'b')) {
        switch (buffer[cur + 1])
        {
        case'x':case'X': // ʮ������
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
        case'o':case'O': // �˽���
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
        while (buffer[cur] >= '0' && buffer[cur] <= '9') { // ɨ�赽��һ����Ϊ���ֵ��ַ�
            secondBuffer += buffer[cur++];
        }
        if (buffer[cur] == '.') { // ����
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
                else if (buffer[cur] == 'E' || buffer[cur] == 'e') { // ��ѧ������
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
        else if (buffer[cur] == 'E' || buffer[cur] == 'e') { // ��ѧ������
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
        else if (buffer[cur] == 'L') { // Ϊ������
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
        else { // �ж�Ϊʮ��������
            storeLex(LEX_TYPE::CONST, secondBuffer);
        }
    }
}

/***************************************
* ��������  handleCharConst
* ���ã�    ����buffer[cur]��ʼ���ַ��������д���
* ������    ��
****************************************/
void LexAnalyser::handleCharConst() {
    string secondBuffer = "";
    if (buffer[++cur] == '\\') { // ����ת���ַ�
        if (buffer[cur + 2] == '\'') {
            secondBuffer += buffer[++cur];
            storeLex(LEX_TYPE::CONST, secondBuffer);
            cur += 2;
        }
        else {
            raiseSyntaxError(curLine, SYNTAX_ERROR::CHAR_SINGLE_QUOTE_UNMATCH_ERROR);
        }
    }
    else { // ������ת���ַ�
        if (buffer[cur + 1] == '\'') { // 
            secondBuffer += buffer[cur++];
            storeLex(LEX_TYPE::CONST, '\''+secondBuffer+'\'');
            cur++;
        }
        else {
            raiseSyntaxError(curLine, SYNTAX_ERROR::CHAR_SINGLE_QUOTE_UNMATCH_ERROR);
        }
    }
}

/***************************************
* ��������  handleStringConst
* ���ã�    ����buffer[cur]��ʼ���ַ����������д���
* ������    ��
****************************************/
void LexAnalyser::handleStringConst() {
    string secondBuffer = "";
    cur++;
    while (buffer[cur] != '\"') {
        if (buffer[cur] == '\0') { // һ�н�����C/C++��֧���ַ�������
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
    storeLex(LEX_TYPE::CONST, '\"'+secondBuffer+'\"');
}

/***************************************
* ��������  isTemplateClass
* ���ã�    ���ҷ��ͱ��Ƿ���ڸñ���
* ������    name:    ��ƥ�����[in]
****************************************/
bool isTemplateClass(string name) {
    for (string kw : existTemplate) {
        if (name==kw) {
            return true;
        }
    }
    return false;
}

bool isClassLexName(std::string name) {
    for (auto it:existClassName) {
        if (it==name) {
            return true;
        }
    }
    return false;
}

/***************************************
* ��������  handleLeftRightAngleBra
* ���ã�    ����buffer[cur]Ϊ<>����������ַ��ͺ������
* ������    ��
****************************************/
void LexAnalyser::handleLeftRightAngleBra() {
    if (isTemplateClass(tokenRes[tokenRes.size() - 1].second.second) || searchKeyword(tokenRes[tokenRes.size() - 1].second.second) || isClassLexName(tokenRes[tokenRes.size() - 1].second.second)) { // ���ʹ����߼�:����ǰһ����ʶ�����������Ƿ���ڷ���
        char bra = buffer[cur]; // �洢�Ƿ�Ϊ<����>
        string secondBuffer = "";
        secondBuffer += buffer[cur++];
        while (bra==buffer[cur]) {
            secondBuffer += buffer[cur++];
        }
        if (isOp(buffer[cur])) {
            raiseSyntaxError(curLine,SYNTAX_ERROR::OP_FORMAT_ERROR);
        }
        else {
            for (unsigned i = 0; i < secondBuffer.size();i++) {
                storeLex(LEX_TYPE::SEPERATOR,string(1, secondBuffer[i]));
            }
        }
    }
    else {
        handleOperator();
    }
}

/***************************************
* ��������  analyseToken
* ���ã�    �����ֿ��߼�������buffer[cur]��ʼ�Ĵ��ؽ��з���
* ������    ��
****************************************/
void LexAnalyser::analyseToken() {
    if (buffer[cur] == '\0' || buffer[cur] == '\n') { // ��ʾ�����Ѿ�������ĩβ
        return;
    }
    char tmp = buffer[cur];
    switch (tmp)
    {
    case '/':
        if (buffer[cur + 1] == '*') { // ��һ��ע��
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
        extractMacro(); // �ʷ�����������������ڸ��������ȡ����
        break;
    case '{':case'}':case'(':case')':case':':case',':case';':case']':case'[':
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
* ��������  lex
* ���ã�    �ӵ�һ���ַ���ʼ������DFA�ʷ��������ڣ��Ƶ��ļ�����
* ������    ��
****************************************/
void LexAnalyser::lex() {
    std::cout << "���дʷ���������" << std::endl;
    while (!(*fin).eof() || buffer[cur] != '\0') {
        shiftToNextToken();
        analyseToken();
    }
}

/***************************************
* ��������  printLexRes
* ���ã�    ��ӡ�ʷ��������
* ������    ��
****************************************/
void LexAnalyser::printLexRes() {
    cout << "�궨�壺" << endl;
    for (string it : macroBin) {
        cout << it << endl;
    }
    cout << "�ʷ��������أ�" << endl;
    int c = 1;
    for (pair<int, pair<LEX_TYPE, string>> it : tokenRes) {
        LEX_TYPE type = it.second.first;
        string typeStr = (type == LEX_TYPE::SEPERATOR) ? "SEPERATOR " : ((type == LEX_TYPE::IDENTIFIER) ? "IDENTIFIER" : ((type == LEX_TYPE::OPERATOR) ? "OPERATOR  " : (type == LEX_TYPE::CONST) ? "CONST     " : "KEYWORD   "));
        cout << "Line:" << it.first << " ,(" << typeStr << " , " << it.second.second<<")\n";
    }
    cout << "ÿ�п�ʼ�ո���:" << endl;
    for (pair<int, int> p : spaceRecord) {
        cout << "Line:" << p.first << " , has " << p.second << " Space" << endl;
    }
}

/***************************************
* ��������  getLexRes
* ���ã�    ���ʷ����������������
* ������    Res         ������[in/out]
****************************************/
void LexAnalyser::getLexRes(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& Res)
{
    Res = tokenRes;
}

void LexAnalyser::getSpaceRecord(std::vector<std::pair<int, int>>& Record) {
    Record = spaceRecord;
}

/***************************************
* ��������  raiseSyntaxError
* ���ã�    �����﷨����ı�����
* ������    line         �к�[in]
*           errorId      ��������[in]
****************************************/
void raiseSyntaxError(int line,SYNTAX_ERROR errorId) {
    switch (errorId)
    {
    case SYNTAX_ERROR::NUM_CONST_FORMAT_ERROR:
        cout << "Line:" << line << ",���ַǷ����ֳ�����ʽ����" << endl;
        abort();
        break;
    case SYNTAX_ERROR::UNVALID_CHAR_ERROR:
        cout << "Line:" << line << ",���ַǷ��ַ�����" << endl;
        abort();
        break;
    case SYNTAX_ERROR::STRING_UNCLOSE_ERROR:
        cout << "Line:" << line << ",�����ַ���������ʽ����" << endl;
        abort();
        break;
    case SYNTAX_ERROR::CHAR_SINGLE_QUOTE_UNMATCH_ERROR:
        cout << "Line:" << line << ",���ַǷ��ַ�����" << endl;
        abort();
        break;
    case SYNTAX_ERROR::OP_FORMAT_ERROR:
        cout << "Line:" << line << "���ַǷ����������(���ء�)" << endl;
        abort();
        break;
    case SYNTAX_ERROR::MULLINE_NOTE_NOT_END_ERROR:
        cout << "Line:" << line << ",����ע���޽�β����TnT" << endl;
        abort();
        break;
    default:
        break;
    }
}