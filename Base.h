/*********************************************************
*  ģ������    Base
*  �ļ�����    Base.h
*  �����ļ���  ��
*  �� �ã�     ��������һЩ���������ݽṹ
*  �� �ߣ�     �����
*  ʱ �䣺     2020.5.25
*  �� ����     Version 2.2
**********************************************************
*  �޸ģ�������������Ϣ��  
*  [1]  �޸�ʱ�䣺  2020.6.1   
*       �޸�Ŀ�ģ�  ����﷨����������ݽṹ
*       �޸��ˣ�    �����
*  [2]  �޸�ʱ��:   2020.6.12
*       �޸�Ŀ�ģ�  ����������������ݻ���
*       �޸��ˣ�    �����
*********************************************************/
#ifndef _BASE_H_
#define _BASE_H_
#include<vector>
#include<iostream>
#include <map>
#define TAB_SPACE 4 // һ��tab�����������������
#define inputDir  "./data/testCompiler01.cpp"// �ļ�������·��

// �ʷ�������Ŀ��������
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

// �ؼ��ʱ�
static std::vector<std::string> keyword = { "include","define","auto","bool","break","case","catch","char","class",
                                "const","const_cast","continue","default","delete","do","double",
                                "dynamic_cast","else","enum","explicit","extern","false","float","for",
                                "friend","goto","if","inline","int","long","mutable","namespace","new",
                                "operator","private","protected","public","register","reinterpret_cast",
                                "return","short","signed","sizeof","static","static_cast","struct",
                                "switch","template","this","throw","true","try","typedef","typeid",
                                "typename","union","unsigned","using","virtual","void","volatile","while"};
static std::vector<std::string> existTemplate = { "vector","pair","stack" }; // ����������
static std::vector<std::string> existClassName = { "vector" ,"pair","stack","string","ifstream","LEX_TYPE","SYNTAX_ERROR" ,"MPI_Status","MPI_Request"}; // �Ѿ����ֵ�����
static std::vector<std::string> existNameSpace = { "std","cv" }; // �ɹ�����������ռ�
static std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> tokenRes; // ��Ŵʷ������Ľ��
static std::vector<std::string> macroBin; // ��ź�
static std::vector<std::pair<int, int>> spaceRecord; // ��¼ÿһ�е�����
static std::map < std::string, int> OpTable = { {"::M",1},{"[M",2}, {"++L",2},{"--L",2},{".M",2},{"->M",2},{"++R",3},{"--R",3},
{"+R",3},{"-R",3},{"!R",3}, {"~R",3},{"*R",3},{"&R",3},{"*M",5},{"/M",5},{"%M",5},{"+M",6},{"-M",6},{"<<M",7},{">>M",7},{"<M",8},{"<=M",8},
{">M",8},{">=M",8},{"==M",9},{"!=M",9},{"&M",10},{"^M",11},{"|M",12},{"&&M",13},{"||M",14},{"?M",15},{":M",15}, {"=M",16},{"+=M",16},{"-=M",16},
{"*=M",16},{"/=M",16},{"%=M",16},{">>=M",16},{"<<=M",16},{"&=M",16},{"|=M",16},{"^=M",16}};
#endif
