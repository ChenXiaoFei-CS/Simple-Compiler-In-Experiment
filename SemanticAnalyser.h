/*********************************************************
*  ģ������    SemanticAnalyser
*  �ļ�����    SemanticAnalyser.h
*  �����ļ���  parseTree.h,Base.h
*  �� �ã�     �����﷨�����������������
*  �� �ߣ�     �����
*  ʱ �䣺     2020.6.12
*  �� ����     Version 1.1
**********************************************************/
#ifndef _SEMANTICANALYSER_H_
#define _SEMANTICANALYSER_H_
#include "parseTree.h"
#include "Base.h"
#include <vector>

/***************************************
* ��������  convertSemanticType2String
* ���ã�    ����������ת��δ�ַ���
* ������    type       ��������[in]
* ����ֵ��  string
****************************************/
std::string convertSemanticType2String(SemanticType type);

/***************************************
* ������    classInfo
* ���ã�    �����ݵĳ������ڱ�ʾ��������
****************************************/
class classInfo {
public:
	/***************************************
	* ��������  classInfo
	* ���ã�    ������
	* ������    name:           ����[in]
	*           templates       ģ�壨���ͣ�[in]
	****************************************/
	classInfo(std::string name,std::vector<SemanticType> templates) {
		this->className = name;
		this->templates = templates;
	}
	std::string className; // ����
	std::vector<SemanticType> templates; // ģ�壨���ͣ�
};

/***************************************
* ������    funcTableItem
* ���ã�    ������ı���
****************************************/
class funcTableItem {
public:
	/***************************************
	* ��������  funcTableItem
	* ���ã�    ������
	* ������    level          �������ڲ㼶  [in]
	*           name           ������        [in]
	*           type           ��������������[in]
	*           startLine      �����Ŀ�ʼ����[in]
	*           endLine        �����Ľ�������[in]
	****************************************/
	funcTableItem(int level, std::string name, SemanticType type, int startLine, int endLine) {
		this->level = level;
		this->funcName = name;
		this->returnType = type;
		this->startLine = startLine;
		this->endLine = endLine;
	}
	/***************************************
	* ��������  fillInArgTable
	* ���ã�    ��亯�������еĲ�������
	* ������    args       �����б����������[in]
	* ����ֵ��  void
	****************************************/
	void fillInArgTable(std::vector<SemanticType> args);
	/***************************************
	* ��������  fillInReturnClassInfo
	* ���ã�    �������亯�������еķ���ֵ���͹�������Ϣ
	* ������    info       ����ֵ���͹�������Ϣ[in]
	* ����ֵ��  void
	****************************************/
	void fillInReturnClassInfo(classInfo *info);
	/***************************************
	* ��������  fillInReturnClassInfo
	* ���ã�    �������亯�������еĲ������͹�������Ϣ
	* ������    info       �������͹�������Ϣ[in]
	* ����ֵ��  void
	****************************************/
	void fillInArgClassInfo(classInfo* info);
	/***************************************
	* ��������  fillInModfiers
	* ���ã�    ��亯�������е����η��б�
	* ������    modifiers       ���η��б�[in]
	* ����ֵ��  void
	****************************************/
	void fillInModfiers(std::vector<std::string> modifiers);
	std::string getName(); // ��ȡ����
	std::vector<SemanticType> getArgTable(); // ��ȡ���������б�
	std::vector<classInfo*> getArgClassInfo(); // ��ȡ�������͹�������Ϣ�б�
	SemanticType getReturnType() { // ��ȡ����ֵ��������
		return this->returnType;
	}
	classInfo* getReturnInfo() { // ��ȡ����ֵ��������Ϣ�б�
		return this->returnClass;
	}
	/***************************************
	* ��������  printInfo
	* ���ã�    ��ӡ�����������Ϣ
	* ������    ��
	* ����ֵ��  void
	****************************************/
	void printInfo();
private:
	int level; // ���ڲ�μ���
	std::string funcName; // ������
	std::vector<std::string> modifiers; // ���η�
	std::vector<SemanticType> argList; // �����Ĳ����б�
	SemanticType returnType; // ������������
	std::vector<classInfo*> argClassInfoTable; // ��ѡ���������б�����class
	classInfo* returnClass; // ��ѡ������������Ϊclass
	int startLine; // ��ʼ����
	int endLine; // ��������
};

/***************************************
* ������    varTableItem
* ���ã�    ������ı���
****************************************/
class varTableItem {
public:
	/***************************************
	* ��������  varTableItem
	* ���ã�    ������
	* ������    level           �������ڲ㼶          [in]
	*           type            ������������          [in]
	*           info            �����������͹�������Ϣ[in]
	*           modifiers       ���η��б�            [in]
	*           line            ������������          [in]
	****************************************/
	varTableItem(int level,std::string name,SemanticType type,classInfo *info,std::vector<std::string> modifiers,int line) {
		this->level = level;
		this->type = type;
		this->name = name;
		this->info = info;
		this->modifiers = modifiers;
		this->line = line;
	}
	std::string getName() {  // ��ñ�����
		return this->name;
	}
	int getLevel() { // ��ñ������ڲ㼶
		return this->level;
	}
	bool getDeprecated() {  // ** ��ñ����Ƿ��Ѿ�����
		return this->deprecated;
	}
	void setDeprecated(bool flag) { // ���ñ����Ƿ����
		this->deprecated = flag;
	}
	SemanticType getSemanType() { // ��ñ�����������
		return this->type;
	}
	classInfo* getClassInfo() { // ��ñ����������͹���������Ϣ
		return info;
	}
	/***************************************
	* ��������  printInfo
	* ���ã�    ��ӡ�����������Ϣ
	* ������    ��
	* ����ֵ��  void
	****************************************/
	void printInfo();
private:
	std::string name; // ��������
	SemanticType type; // ��������
	classInfo* info; // ��ѡ����
	std::vector<std::string> modifiers; // ���η�
	int level; // �������ڲ㼶
	int line; // ������������
	bool deprecated = false; // �����Ƿ����
};

/***************************************
* ������    SemanticAnalyser
* ���ã�    ���������
****************************************/
class SemanticAnalyser {
public:
	/***************************************
	* ��������  SemanticAnalyser
	* ���ã�    ������
	* ������    root   ������ͷ�ڵ�[in]
	****************************************/
	SemanticAnalyser(Statement* root);
	/***************************************
	* ��������  ~SemanticAnalyser
	* ���ã�    ��������
	****************************************/
	~SemanticAnalyser();
	/***************************************
	* ��������  semantics
	* ���ã�    ��ʼ�����������
	* ������    ��
	* ����ֵ��  void
	****************************************/
	void semantics();
	/***************************************
	* ��������  printFuncInfo
	* ���ã�    ��ӡ������
	* ������    ��
	* ����ֵ��  void
	****************************************/
	void printFuncInfo();
	/***************************************
	* ��������  printVarInfo
	* ���ã�    ��ӡ������
	* ������    ��
	* ����ֵ��  void
	****************************************/
	void printVarInfo();
private:
	Statement* parseTreeRoot;  // ���������ڵ�
	int level;   // ��ǰ�����㼶
	/***************************************
	* ��������  analyserNode
	* ���ã�    ���¶��϶�ĳһ��������������
	* ������    root         ����ָ��[in]
	* ����ֵ��  void
	****************************************/
	void analyserNode(Statement *root);
	std::vector<funcTableItem*> functionTable; // ������
	std::vector<varTableItem*> variableTable; // ���ű�
	/***************************************
	* ��������  registerFunc
	* ���ã�    ���ں������еı������ע��
	* ������    funcNode  �����������������ָ��[in]
	* ����ֵ��  void
	****************************************/
	void registerFunc(FuncDefineStatement* funcNode);
	/***************************************
	* ��������  registerVar
	* ���ã�    ���ڱ������еı������ע��
	* ������    varDeclNode  ������������������ָ��[in]
	* ����ֵ��  void
	****************************************/
	void registerVar(VarDeclarationStatement* varDeclNode);
	/***************************************
	* ��������  registerVar
	* ���ã�    ���ڱ������еı������ע��
	* ������    name        ������        [in]
	*           type        �������﷨����[in]
	*           line        ���������ĺ���[in]
	* ����ֵ��  void
	****************************************/
	void registerVar(std::string name,std::string type,int line);
	/***************************************
	* ��������  parseType
	* ���ã�    ���﷨���ͽ���Ϊ��������
	* ������    proto       ԭʼ�﷨����          [in]
	*           type        ��������������      [in/out]
	*           info        �������͹�������Ϣ���[in/out]
	* ����ֵ��  void
	****************************************/
	void parseType(std::string proto,SemanticType &type,classInfo* &info);
	/***************************************
	* ��������  judgeIDType
	* ���ã�    �ж�IDNode����Ӧ����������
	* ������    idNode     IDNode����ָ��[in]
	*           type       ��������������[in/out]
	*           classInfo  �������͹�������Ϣ���[in/out]
	* ����ֵ��  void
	****************************************/
	void judgeIDType(IdNodeStatement* idNode,SemanticType &type,classInfo *&info);
	/***************************************
	* ��������  judgeFunctionType
	* ���ã�    ����FunctionNode����Ӧ����������
	* ������    root       FunctionNode����ָ��[in]
	*           type       ��������������[in/out]
	*           classInfo  �������͹�������Ϣ���[in/out]
	* ����ֵ��  void
	****************************************/
	void judgeFunctionType(FunctionStatement* root, SemanticType& type, classInfo*& info);
	/***************************************
	* ��������  operate
	* ���ã�    ����OpNodeStatement����Ӧ����������
	* ������    opNode       FunctionNode����ָ��[in]
	*           type         ��������������[in/out]
	*           classInfo    �������͹�������Ϣ���[in/out]
	* ����ֵ��  void
	****************************************/
	void operate(OpNodeStatement* opNode, SemanticType &type,classInfo *&info);
	/***************************************
	* ��������  checkOperation
	* ���ã�    ������������ͼ���Ľ������
	* ������    OpType        ���������                      [in]
	*           OpDir         ���������                      [in]
	*           typeOp1       ��һ����������������            [in]
	*           classInfo1    ��һ�����������͹���������Ϣ    [in]
	*           typeOp2       �ڶ�����������������            [in]
	*           classInfo2    �ڶ������������͹���������Ϣ    [in]
	*           resType       �����������                    [in/out]
	*           resInfo       ����������͹���������Ϣ        [in/out]
	*           line          ��ʼ����                        [in]
	*           castSemanType ǿ��ת�ͽ����������            [in]
	*           castInfo      ǿ��ת�ͽ���������͹���������Ϣ[in]
	* ����ֵ��  void
	****************************************/
	void checkOperation(std::string OpType, std::string OpDir,SemanticType &typeOp1,classInfo* &classInfo1,SemanticType &typeOp2, classInfo *&classInfo2, SemanticType &resType,classInfo* &resInfo,int line, SemanticType& castSemanType, classInfo*& castInfo);
	/***************************************
	* ��������  checkJudge
	* ���ã�    ����֧�Ƿ�Ϸ�
	* ������    judge        ��֧���ָ��[in]
	* ����ֵ��  void
	****************************************/
	void checkJudge(branchStatement* judge);
	/***************************************
	* ��������  checkReturn
	* ���ã�    ��return����Ƿ��뺯������ƥ��
	* ������    judge        ��֧���ָ��[in]
	* ����ֵ��  void
	****************************************/
	void checkReturn(returnStatement* root);
	/***************************************
	* ��������  checkReturnComplement
	* ���ã�    �����������Ƿ����return
	* ������    root        ����������ָ��[in]
	* ����ֵ��  void
	****************************************/
	void checkReturnComplement(FuncDefineStatement* root);
};
#endif
