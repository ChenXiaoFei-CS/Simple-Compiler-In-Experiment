/*********************************************************
*  ģ������    GrammarAnalyser
*  �ļ�����    GrammarAnalyser.h
*  �����ļ���  Base.h, parseTree.h
*  �� �ã�     ����C/C++���Խ����﷨����
*  �� �ߣ�     �����
*  ʱ �䣺     2020.6.1
*  �� ����     Version 1.1
*********************************************************/
#ifndef _GRAMMARANALYSER_H_
#define _GRAMMARANALYSER_H_
#include "Base.h"
#include "parseTree.h"
#include <vector>
#include <string>

/***************************************
* ��������  isModifier
* ���ã�    �ж��Ƿ�Ϊ���η���ؼ���
* ������    word��     ���������[in]
* ����ֵ��  bool
****************************************/
bool isModifier(std::string word);
/***************************************
* ��������  isType
* ���ã�    �ж��Ƿ��ܹ���������
* ������    lex��      ����Ĵʷ�����[in]
*           word:      ���������    [in]
* ����ֵ��  bool
****************************************/
bool isType(LEX_TYPE lex,std::string word);
/***************************************
* ��������  isProtoType
* ���ã�    �ж��Ƿ��ǻ�������
* ������    word:      ���������    [in]
* ����ֵ��  bool
****************************************/
bool isProtoType(std::string word);
/***************************************
* ��������  isClassName
* ���ã�    �ж��Ƿ���ע�������
* ������    word:      ���������    [in]
* ����ֵ��  bool
****************************************/
bool isClassName(std::string word);
/***************************************
* ��������  hasTemplate
* ���ã�    �ж������Ƿ���ڷ��ͣ�ģ�壩
* ������    word:      ���������    [in]
* ����ֵ��  bool
****************************************/
bool hasTemplate(std::string word);
/***************************************
* ��������  searchForSpace
* ���ã�    ����ĳһ�е�����
* ������    spaceTable:  ������  [in]
*           line:        �к�    [in]
* ����ֵ��  int
****************************************/
int searchForSpace(std::vector<std::pair<int, int>> spaceTable, int line);
/***************************************
* ��������  searchForLeastPriOp
* ���ã�    ����һ�����ȼ���͵����������
* ������    bin:       token����         [in]
*           startPos:  ��ʼ����λ��      [in]
*           endPos:    ��������λ��      [in]
*           splitPos:  Ŀ���������������[in/out]
*           splitDir:  Ŀ�����������    [in/out]
* (M����˫Ŀ��L��������б��ʽ�ĵ�Ŀ��R�����ұ��б��ʽ�ĵ�Ŀ)
* ����ֵ��  ��
****************************************/
void searchForLeastPriOp(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> bin, int startPos, int endPos,int &splitPos,std::string &splitDir);
/***************************************
* ��������  judgeInvalidExpression
* ���ã�    �жϱ��ʽ�Ƿ�Ϸ��������Ϸ��������ж�
* ������    bin:       token����         [in]
*           startPos:  ��ʼ����λ��      [in]
*           endPos:    ��������λ��      [in]
* ����ֵ��  ��
****************************************/
void judgeInvalidExpression(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& bin, int startPos, int endPos);

/***************************************
* ������    GrammarAnalyser
* ���ã�    �����﷨����
****************************************/
class GrammarAnalyser
{
public:
	/***************************************
	* ��������  GrammarAnalyser
	* ���ã�    ������
	* ������    lexRes:     �ʷ��������[in]
	*           spaceRecord ÿһ�е�����[in]
	****************************************/
	GrammarAnalyser(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> lexRes, std::vector<std::pair<int, int>> spaceRecord);
	/***************************************
	* ��������  ~GrammarAnalyser
	* ���ã�    ��������
	****************************************/
	~GrammarAnalyser();
	/***************************************
	* ��������  grammar
	* ���ã�    ��ʼ�����﷨����
	* ������    ��
	* ����ֵ��  ��
	****************************************/
	void grammar(); 
	/***************************************
	* ��������  printParseTree
	* ���ã�    ��ӡ�﷨������
	* ������    ��
	* ����ֵ��  ��
	****************************************/
	void printParseTree();
	/***************************************
	* ��������  getParseTree
	* ���ã�    ��ȡ���������ڵ��﷨������
	* ������    root        [in/out]
	* ����ֵ��  ��
	****************************************/
	void getParseTree(Statement* &root);
	Statement* root = NULL; // �������ĸ��ڵ�
private:
	std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> lexRes; // �ʷ������Ľ��
	std::vector<std::pair<int, int>> spaceRecord;  // ���������
	/***************************************
	* ��������  init
	* ���ã�    �����﷨����ʱ���������ʼ���ݽṹ
	* ������    ��
	* ����ֵ��  ��
	****************************************/
	void init();
	int cur = 0; // token��ָ������
	Statement* curStatement = NULL; // �ڹ��������ʱ����ǰ���
	/***************************************
	* ��������  parseComponent
	* ���ã�    ����һ���﷨������Զ����µ�˼��
	* ������    ��
	* ����ֵ��  ��
	****************************************/
	void parseComponent();
	/***************************************
	* ��������  extractType
	* ���ã�    ��lexRes�н�����ǰָ��ָ�������
	* ������    type       ����������[in/out]
	* ����ֵ��  ��
	****************************************/
	void extractType(std::string& type);
	/***************************************
	* ��������  extractType
	* ���ã�    ��lexRes�н�����ǰָ��ָ�������
	* ������    type       ����������[in/out]
	*           tmpResBin  token����   [in]
	*           cursor     tokenָ��   [in/out]
	* ����ֵ��  ��
	****************************************/
	void extractType(std::string& type, std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& tmpResBin, int& cursor);
	/***************************************
	* ��������  extractArgTable
	* ���ã�    ��lexRes�н�����������ʱ�Ĳ����б�
	* ������    argTable   �����б�������[in/out]
	* ����ֵ��  ��
	****************************************/
	void extractArgTable(std::vector<std::pair<std::string, std::string>> &argTable);
	/***************************************
	* ��������  extractJudgement
	* ���ã�    ��lexRes�з������֧����жϱ��ʽ
	* ������    tmpResBin  �жϱ��ʽ������[in/out]
	* ����ֵ��  ��
	****************************************/
	void extractJudgement(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& tmpResBin);
	/***************************************
	* ��������  searchForCast
	* ���ã�    ���ұ��ʽ�д��ڵ�ǿ��ת�����
	* ������    bin          token����[in/out]
	*           startPos     ��ʼλ������[in]
	*           endPos       ����λ������[in]
	*           castPosStart ǿ��ת����ʼλ��������[in/out]
	*           castPosEnd   ǿ��ת�ͽ���λ��������[in/out]
	*           CastType     ǿ��ת������������[in/out]
	* ����ֵ��  ��
	****************************************/
	void searchForCast(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>bin, int startPos, int endPos, int& castPosStart, int& CastPosEnd, std::string& CastType);
	/***************************************
	* ��������  split
	* ���ã�    ��lexRes�а���Ŀ��token���ж�������и�
	* ������    tmpResBin     ���������[in/out]
	*           commaedPos    �ڲ��и����Ԫ������������[in/out]
	*           end           ����и���[in]
	*           mid           �ڲ��и���[in]
	* ����ֵ��  ��
	****************************************/
	void split(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& tmpResBin, std::vector<int>& commaedPos,std::string end,std::string mid);
	/***************************************
	* ��������  split
	* ���ã�    ��lexRes�а���Ŀ��token���е�������и�
	* ������    tmpResBin     ���������[in/out]
	*           end           �и���[in]
	* ����ֵ��  ��
	****************************************/
	void split(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& tmpResBin, std::string end);
	/***************************************
	* ��������  split
	* ���ã�    ��Ŀ�������а���Ŀ��token���ж�������и�
	* ������    src             Ŀ��token����[in]
	*           dst             ���������[in/out]
	*           commaedPos      �ڲ��и����Ԫ������������[in/out]
	*           end             ����и���[in]
	*           mid             �ڲ��и���[in]
	*           cursor          Ŀ��������ʼָ��[in/out]
	* ����ֵ��  ��
	****************************************/
	void split(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> src, std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> &dst, std::vector<int>& commaedPos, std::string end, std::string mid,int &cursor);
	/***************************************
	* ��������  handleUsing
	* ���ã�    ����using�ؼ��ֿ�ʼ���
	* ������    ��
	* ����ֵ��  ��
	****************************************/
	void handleUsing();
	/***************************************
	* ��������  handleFunc
	* ���ã�    �������������
	* ������    ��
	* ����ֵ��  ��
	****************************************/
	void handleFunc(std::vector<std::string> modifiers, std::string type, std::string name,int startLine);
	/***************************************
	* ��������  handleVarDeclaration
	* ���ã�    ��������������
	* ������    ��
	* ����ֵ��  ��
	****************************************/
	void handleVarDeclaration(std::vector<std::string> modifiers, std::string type, std::string name, int startLine);
	/***************************************
	* ��������  handleNode
	* ���ã�    ���¶��ϴ�����ʽ�뺯�����û�����
	* ������    ��
	* ����ֵ��  ��
	****************************************/
	void handleNode(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& bin,int startPos,int endPos,Statement *&handle);
	/***************************************
	* ��������  handleEndsWithBracket
	* ���ã�    �����Ҵ����Ž��
	* ������    ��
	* ����ֵ��  ��
	****************************************/
	void handleEndsWithBracket();
	/***************************************
	* ��������  handleIf
	* ���ã�    ����if��֧
	* ������    ��
	* ����ֵ��  ��
	****************************************/
	void handleIf();
	/***************************************
	* ��������  handleElse
	* ���ã�    ����else �Լ� else if��֧
	* ������    ��
	* ����ֵ��  ��
	****************************************/
	void handleElse();
	/***************************************
	* ��������  handleWhile
	* ���ã�    ����whileѭ��
	* ������    ��
	* ����ֵ��  ��
	****************************************/
	void handleWhile();
	/***************************************
	* ��������  handleFor
	* ���ã�    ����forѭ��
	* ������    ��
	* ����ֵ��  ��
	****************************************/
	void handleFor();
	/***************************************
	* ��������  handleAssign
	* ���ã�    ������ͨ���ʽ���
	*           ���ڲ�����һ�ζ��ڶ��ŵ��и
	* ������    ��
	* ����ֵ��  ��
	****************************************/
	void handleAssign();
	/***************************************
	* ��������  handleSwitch
	* ���ã�    ����switch��֧
	* ������    ��
	* ����ֵ��  ��
	****************************************/
	void handleSwitch();
	/***************************************
	* ��������  handleCase
	* ���ã�    ����case��֧
	* ������    ��
	* ����ֵ��  ��
	****************************************/
	void handleCase();
	/***************************************
	* ��������  handleBreak
	* ���ã�    ����break���
	*           ���ڲ�����һ�ζ��ڶ��ŵ��и
	* ������    ��
	* ����ֵ��  ��
	****************************************/
	void handleBreak();
	/***************************************
	* ��������  handleDefault
	* ���ã�    ����default��֧
	* ������    ��
	* ����ֵ��  ��
	****************************************/
	void handleDefault();
	/***************************************
	* ��������  handleIndentation
	* ���ã�    ���������м��
	* ������    ��
	* ����ֵ��  ��
	****************************************/
	void handleIndentation();
	/***************************************
	* ��������  handleReturn
	* ���ã�    ����return���
	* ������    ��
	* ����ֵ��  ��
	****************************************/
	void handleReturn();
};
#endif