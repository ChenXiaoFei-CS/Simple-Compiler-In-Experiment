/*********************************************************
*  模块名：    Base
*  文件名：    Base.h
*  依赖文件：  无
*  作 用：     编译器中一些基本的数据结构
*  作 者：     陈骁飞
*  时 间：     2020.5.25
*  版 本：     Version 2.2
**********************************************************
*  修改（补丁升级）信息：  
*  [1]  修改时间：  2020.6.1   
*       修改目的：  添加语法分析相关数据结构
*       修改人：    陈骁飞
*  [2]  修改时间:   2020.6.12
*       修改目的：  添加语义分析相关数据机构
*       修改人：    陈骁飞
*********************************************************/
#ifndef _BASE_H_
#define _BASE_H_
#include<vector>
#include<iostream>
#include <map>
#define TAB_SPACE 4 // 一个tab键所代表的缩进个数
#define inputDir  "./data/testCompiler01.cpp"// 文件的输入路径

// 词法分析的目标结果类型
enum class LEX_TYPE {
    IDENTIFIER = 0,
    OPERATOR = 1,
    CONST = 2,
    SEPERATOR = 3,
    KEYWORD = 4
};

enum class SYNTAX_ERROR {
    NUM_CONST_FORMAT_ERROR=0,
    UNVALID_CHAR_ERROR=1,
    STRING_UNCLOSE_ERROR=2,
    CHAR_SINGLE_QUOTE_UNMATCH_ERROR=3,
    OP_FORMAT_ERROR=4,
    MULLINE_NOTE_NOT_END_ERROR=5,
    NUM_CONST_UNCOMPELET_ERROR = 6
};

enum class SemanticType {
    INT = 0,
    DOUBLE = 1,
    CHAR = 2,
    FLOAT = 3,
    LONG = 4,
    SHORT = 5,
    INT_PTR = 6,
    DOUBLE_PTR = 7,
    CHAR_PTR = 8,
    FLOAT_PTR = 9,
    LONG_PTR = 10,
    SHORT_PTR = 11,
    CLASS = 12,
    CLASS_PTR = 13,
    NONE = 14,
    CHAR_PTR_PTR = 15,
    BOOL = 16,
    BOOL_PTR = 17,
    VOID = 18,
    VOID_PTR = 19
};

// 关键词表
static std::vector<std::string> keyword = { "include","define","auto","bool","break","case","catch","char","class",
                                "const","const_cast","continue","default","delete","do","double",
                                "dynamic_cast","else","enum","explicit","extern","false","float","for",
                                "friend","goto","if","inline","int","long","mutable","namespace","new",
                                "operator","private","protected","public","register","reinterpret_cast",
                                "return","short","signed","sizeof","static","static_cast","struct",
                                "switch","template","this","throw","true","try","typedef","typeid",
                                "typename","union","unsigned","using","virtual","void","volatile","while"};
static std::vector<std::string> existTemplate = { "vector","pair","stack" }; // 泛型类名表
static std::vector<std::string> existClassName = { "vector" ,"pair","stack","string","ifstream","LEX_TYPE","SYNTAX_ERROR" ,"MPI_Status","MPI_Request"}; // 已经出现的类名
static std::vector<std::string> existNameSpace = { "std","cv" }; // 可供导入的命名空间
static std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> tokenRes; // 存放词法分析的结果
static std::vector<std::string> macroBin; // 存放宏
static std::vector<std::pair<int, int>> spaceRecord; // 记录每一行的缩进
static std::map < std::string, int> OpTable = { {"::M",1},{"[M",2}, {"++L",2},{"--L",2},{".M",2},{"->M",2},{"++R",3},{"--R",3},
{"+R",3},{"-R",3},{"!R",3}, {"~R",3},{"*R",3},{"&R",3},{"*M",5},{"/M",5},{"%M",5},{"+M",6},{"-M",6},{"<<M",7},{">>M",7},{"<M",8},{"<=M",8},
{">M",8},{">=M",8},{"==M",9},{"!=M",9},{"&M",10},{"^M",11},{"|M",12},{"&&M",13},{"||M",14},{"?M",15},{":M",15}, {"=M",16},{"+=M",16},{"-=M",16},
{"*=M",16},{"/=M",16},{"%=M",16},{">>=M",16},{"<<=M",16},{"&=M",16},{"|=M",16},{"^=M",16}};
#endif
