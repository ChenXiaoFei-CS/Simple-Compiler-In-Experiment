/*********************************************************
*  模块名：    LexAnalyser
*  文件名：    LexAnalyser.h
*  依赖文件：  Base.h
*  作 用：     对于C/C++语言进行词法分析
*  作 者：     陈骁飞
*  时 间：     2020.5.25
*  版 本：     Version 2.2
*********************************************************/
#ifndef _LEXANALYSER_H_
#define _LEXANALYSER_H_
#include "Base.h"
#include <iostream>
#include <vector>
#define MAX_BUFFER_SIZE 1000 // Main Buffer的最大容量

/***************************************
* 函数名：  openFile
* 作用：    打开高级程序文件
* 参数：    dir： 高级程序的路径[in]
****************************************/
std::ifstream openFile(std::string dir);
/***************************************
* 函数名：  searchKeyword
* 作用：    查找关键词表是否存在该表项
* 参数：    id:    待匹配表项[in]
****************************************/
bool searchKeyword(std::string id);
/***************************************
* 函数名：  isTemplateClass
* 作用：    查找泛型表是否存在该表项
* 参数：    name:    待匹配表项[in]
****************************************/
bool isTemplateClass(std::string name);
/***************************************
* 函数名：  isValidID
* 作用：    判定是否为字母，数字，下划线
* 参数：    id:    待匹配字符[in]
****************************************/
inline bool isValidID(char id);
/***************************************
* 函数名：  isOp
* 作用：    判断是否为运算符
* 参数：    op:     待判定运算符[in]
****************************************/
inline bool isOp(char op);
bool isClassLexName(std::string name);
/***************************************
* 函数名：  raiseSyntaxError
* 作用：    报错，中断程序
* 参数：    line:     语法错误行数[in]
*           errorId： 语法错误类型[in]
****************************************/
void raiseSyntaxError(int line,SYNTAX_ERROR errorId);

/***************************************
* 类名：    LexAnalyser
* 作用：    进行词法分析
****************************************/
class LexAnalyser
{
private:
    std::ifstream* fin; // 文件指针设计在词法分析器内部
    char* buffer; // 主缓冲区
    int curLine = 0; // 当前行号
    int cur = 0; // LexAnalyser在Main Buffer中的指针index形式
    /***************************************
    * 函数名：  init
    * 作用：    初始化LexAnalyser，分配相应内存
    * 参数：    无
    ****************************************/
    void init();
    /***************************************
    * 函数名：  recordSpace
    * 作用：    记录每一行的缩进个数，记录在spaceRecorder中
    * 参数：    无
    ****************************************/
    void recordSpace();
    /***************************************
    * 函数名：  loadBuffer
    * 作用：    在Main Buffer中更新一行，并将全局指针指向Main Buffer的首地址
    * 参数：    fin:  高级程序文件输入流指针[in/out]
    ****************************************/
    bool loadBuffer(std::ifstream* fin);
    /***************************************
    * 函数名：  shiftToNextToken
    * 作用：    从当前位置开始寻找下一个token的首位字符，存储在buffer[cur]中
    * 参数：    无
    ****************************************/
    void shiftToNextToken();
    /***************************************
    * 函数名：  searchForNoteEndType_1
    * 作用：    对于多行注释进行处理
    * 参数：    无
    ****************************************/
    bool searchForNoteEndType_1();
    /***************************************
    * 函数名：  searchForNoteEndType_2
    * 作用：    对于单行注释进行处理
    * 参数：    无
    ****************************************/
    bool searchForNoteEndType_2();
    /***************************************
    * 函数名：  extractMacro
    * 作用：    提取该宏
    * 参数：    无
    ****************************************/
    void extractMacro();
    /***************************************
    * 函数名：  handleSeperator
    * 作用：    对于buffer[cur]所在的界符进行处理
    * 参数：    无
    ****************************************/
    void handleSeperator();
    /***************************************
    * 函数名：  handleOperator
    * 作用：    对于buffer[cur]开始的运算符进行处理
    * 参数：    无
    ****************************************/
    void handleOperator();
    /***************************************
    * 函数名：  handlerIdentifier
    * 作用：    对于buffer[cur]开始的标识符进行处理
    * 参数：    无
    ****************************************/
    void handlerIdentifier();
    /***************************************
    * 函数名：  handleNumber
    * 作用：    对于buffer[cur]开始的数字常量进行处理
    * 参数：    无
    ****************************************/
    void handleNumber();
    /***************************************
    * 函数名：  handleCharConst
    * 作用：    对于buffer[cur]开始的字符常量进行处理
    * 参数：    无
    ****************************************/
    void handleCharConst();
    /***************************************
    * 函数名：  handleStringConst
    * 作用：    对于buffer[cur]开始的字符串常量进行处理
    * 参数：    无
    ****************************************/
    void handleStringConst();
    /***************************************
    * 函数名：  handleLeftRightAngleBra
    * 作用：    对于buffer[cur]为<>的情况，区分泛型和运算符
    * 参数：    无
    ****************************************/
    void handleLeftRightAngleBra();
    /***************************************
    * 函数名：  storeLex
    * 作用：    将词法分析的词素及类型存储到tokenRes中
    * 参数：    type:            词素的类型     [in]
    *           tokenContent :   词素的具体内容 [in]
    ****************************************/
    void storeLex(LEX_TYPE type, std::string tokenContent);
    /***************************************
    * 函数名：  analyseToken
    * 作用：    启动分块逻辑，对于buffer[cur]开始的词素进行分析
    * 参数：    无
    ****************************************/
    void analyseToken();
public:
    /***************************************
    * 函数名：  LexAnalyser
    * 作用：    构造器
    * 参数：    inFile:  私有文件指针[in]
    ****************************************/
    LexAnalyser(std::ifstream& inFile) {
        this->fin = &inFile;
        init();
    }
    /***************************************
    * 函数名：  lex
    * 作用：    从第一个字符开始，启动DFA词法分析周期，制导文件结束
    * 参数：    无
    ****************************************/
    void lex();
    /****************************************
    * 函数名：  printLexRes
    * 作用：    打印词法分析结果
    * 参数：    无
    ****************************************/
    void printLexRes();
    /***************************************
    * 函数名：  getLexRes
    * 作用：    将词法分析结果输出给外界
    * 参数：    Res         输出句柄[in/out]
    ****************************************/
    void getLexRes(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> &Res);
    /***************************************
    * 函数名：  getSpaceRecord
    * 作用：    将缩进表输出给外界
    * 参数：    Record         输出句柄[in/out]
    ****************************************/
    void getSpaceRecord(std::vector<std::pair<int, int>> &Record);
};
#endif