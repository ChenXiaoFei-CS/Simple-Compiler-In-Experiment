/*********************************************************
*  ģ������    parseTree
*  �ļ�����    parseTree.h
*  �����ļ���  Base.h
*  �� �ã�     ����C/C++���Խ����﷨�����Ļ������ݻ���--������
*  �� �ߣ�     �����
*  ʱ �䣺     2020.6.2
*  �� ����     Version 1.1
*********************************************************/
#ifndef _PARSETREE_H_
#define _PARSETREE_H_
#include"Base.h"
#include <vector>
#include <iostream>

/***************************************
* ������    Statement
* ���ã�    ������������㸸��
****************************************/
class Statement
{
public:
	/***************************************
	* ��������  Statement
	* ���ã�    ������
	* ������    ��
	****************************************/
	Statement();
	/***************************************
	* ��������  ~Statement
	* ���ã�    ��������
	****************************************/
	~Statement();
	/***************************************
	* ��������  getParent
	* ���ã�    ��ȡ���׽��
	* ������    ��
	* ����ֵ��  Statement*
	****************************************/
	Statement* getParent();
	/***************************************
	* ��������  addChild
	* ���ã�    ��ӽ�㵽�Լ��ĺ��ӣ�ͬʱ�޸ĺ��ӽ��ĸ�ָ��
	* ������    child       ���ӽ��ָ��[in]
	* ����ֵ��  ��
	****************************************/
	void addChild(Statement* child);
	/***************************************
	* ��������  getInstanceName
	* ���ã�    �麯�����䣬��ȡ����
	* ������    ��
	* ����ֵ��  stirng
	****************************************/
	virtual std::string getInstanceName() {
		return "Statement";
	}
	/***************************************
	* ��������  getInfo
	* ���ã�    �麯������ӡ������ʱ��ӡ�Լ�������Ϣ
	* ������    level        ��ǰ�������[in]
	* ����ֵ��  ��
	****************************************/
	virtual void getInfo(int level);
	/***************************************
	* ��������  getChildList
	* ���ã�    �麯������ȡ�����б�
	* ������    ��
	* ����ֵ��  std::vector<Statement*>
	****************************************/
	virtual std::vector<Statement*> getChildList() {
		return this->childs;
	}
protected:
	std::vector<Statement*> childs; // ���ӽ��ָ���б�
	Statement* parent = NULL; // ���׽��ָ��
};

/***************************************
* ������    RootStatement
* ���ã�    �����������
* ���ࣺ    Statement
****************************************/
class RootStatement :public Statement{
public:
	/***************************************
	* ��������  RootStatement
	* ���ã�    ������
	* ������    ��
	****************************************/
	RootStatement();
	/***************************************
	* ��������  ~RootStatement
	* ���ã�    ��������
	****************************************/
	~RootStatement();
	/***************************************
	* ��������  getInstanceName
	* ���ã�    �麯�����䣬��ȡ����
	* ������    ��
	* ����ֵ��  stirng
	****************************************/
	virtual std::string getInstanceName() {
		return "RootStatement";
	}
	/***************************************
	* ��������  getInfo
	* ���ã�    �麯������ӡ������ʱ��ӡ�Լ�������Ϣ
	* ������    level        ��ǰ�������[in]
	* ����ֵ��  ��
	****************************************/
	void getInfo(int level);
};

/***************************************
* ������    FuncDefineStatement
* ���ã�    �������������������
* ���ࣺ    Statement
****************************************/
class FuncDefineStatement :public Statement {
public:
	/***************************************
	* ��������  FuncDefineStatement
	* ���ã�    ������
	* ������    name       ������        [in]
	*           returnType ��������ֵ����[in]
	*           modifier   �������η�    [in]
	*           startLine  ������������  [in]
	****************************************/
	FuncDefineStatement(std::string name,std::string returnType, std::vector<std::string> modifier,int startLine);
	/***************************************
	* ��������  ~FuncDefineStatement
	* ���ã�    ��������
	****************************************/
	~FuncDefineStatement();
	/***************************************
	* ��������  setArgTable
	* ���ã�    ��Ӻ��������б�
	* ������    argTable      �����б�[in]
	* ����ֵ��  ��
	****************************************/
	void setArgTable(std::vector<std::pair<std::string, std::string>> argTable);
	/***************************************
	* ��������  setEndLine
	* ���ã�    ��Ӻ�����β����
	* ������    endLine       ������β����[in]
	* ����ֵ��  ��
	****************************************/
	void setEndLine(int endLine);
	/***************************************
	* ��������  getInstanceName
	* ���ã�    �麯�����䣬��ȡ����
	* ������    ��
	* ����ֵ��  stirng
	****************************************/
	virtual std::string getInstanceName() {
		return "FuncDefineStatement";
	}
	/***************************************
	* ��������  getInfo
	* ���ã�    �麯������ӡ������ʱ��ӡ�Լ�������Ϣ
	* ������    level        ��ǰ�������[in]
	* ����ֵ��  ��
	****************************************/
	void getInfo(int level);
	std::string getName();
	std::vector<std::string> getModifiers();
	std::vector<std::pair<std::string, std::string>> getArgTable();
	int getStartLine();
	int getEndLine();
	std::string getReturnType();
private:
	std::string returnType; // �����ķ���ֵ����
	std::vector<std::string> modifier; // ���������η�
	std::vector<std::pair<std::string, std::string>> argTable; // �����Ĳ����б�
	std::string name; // ������
	int startLine; // �����������ʼ��
	int endLine; // ��������Ľ�β��
};

/***************************************
* ������    VarDeclarationStatement
* ���ã�    �������������������
* ���ࣺ    Statement
****************************************/
class VarDeclarationStatement :public Statement{
private:
	std::string name; // ������
	std::string type; // ��������
	std::vector<std::string> modifiers; // �������η�
	int startLine; // ������������
public:
	/***************************************
	* ��������  VarDeclarationStatement
	* ���ã�    ������
	* ������    name       ������        [in]
	*           type       ��������      [in]
	*           modifier   �������η�    [in]
	*           startLine  ������������  [in]
	****************************************/
	VarDeclarationStatement(std::string name, std::string type, std::vector<std::string> modifier, int startLine);
	/***************************************
	* ��������  ~VarDeclarationStatement
	* ���ã�    ��������
	****************************************/
	~VarDeclarationStatement();
	/***************************************
	* ��������  getInstanceName
	* ���ã�    �麯�����䣬��ȡ����
	* ������    ��
	* ����ֵ��  stirng
	****************************************/
	virtual std::string getInstanceName() {
		return "VarDeclarationStatement";
	}
	/***************************************
	* ��������  getInfo
	* ���ã�    �麯������ӡ������ʱ��ӡ�Լ�������Ϣ
	* ������    level        ��ǰ�������[in]
	* ����ֵ��  ��
	****************************************/
	void getInfo(int level);

	std::string getName();
	std::string getType();
	std::vector<std::string> getModifiers();
	int getLine();
};

/***************************************
* ������    branchStatement
* ���ã�    ��������֧�����
* ���ࣺ    Statement
****************************************/
class branchStatement :public Statement {
private:
	std::string branchType; // ��֧������
	int startLine; // ��֧����������
	int endLine; // ��֧����������
	bool hasBrack = false; // ��֧����Ƿ�ʹ�ô�������Ϊ���
public:
	/***************************************
	* ��������  branchStatement
	* ���ã�    ������
	* ������    type       ��֧����      [in]
	*           startLine  ������������  [in]
	****************************************/
	branchStatement(std::string type,int startLine);
	/***************************************
	* ��������  ~branchStatement
	* ���ã�    ��������
	****************************************/
	~branchStatement();
	/***************************************
	* ��������  setHasBrack
	* ���ã�    �趨�Ƿ�ʹ�ô����Ž������
	* ������    hasBrack       �Ƿ�ʹ�ô����Ž������[in]
	* ����ֵ��  ��
	****************************************/
	void setHasBrack(bool hasBrack);
	/***************************************
	* ��������  setEndLine
	* ���ã�    �趨��֧����������
	* ������    endLine        ��������[in]
	* ����ֵ��  ��
	****************************************/
	void setEndLine(int endLine);
	/***************************************
	* ��������  getHasBrack
	* ���ã�    ��֧�����Ƿ�ʹ�ô����Ž������
	* ������    ��
	* ����ֵ��  bool
	****************************************/
	bool getHasBrack();
	/***************************************
	* ��������  getStartLine
	* ���ã�    ��ȡ��֧�����ʼ����
	* ������    ��
	* ����ֵ��  int
	****************************************/
	int getStartLine();
	/***************************************
	* ��������  getInstanceName
	* ���ã�    �麯�����䣬��ȡ����
	* ������    ��
	* ����ֵ��  stirng
	****************************************/
	virtual std::string getInstanceName() {
		return "branchStatement";
	}
	/***************************************
	* ��������  getInfo
	* ���ã�    �麯������ӡ������ʱ��ӡ�Լ�������Ϣ
	* ������    level        ��ǰ�������[in]
	* ����ֵ��  ��
	****************************************/
	void getInfo(int level);
	std::string getbranchType() {
		return this->branchType;
	}
	Statement* judgement; // ����if,while,switchѭ�����ж����
	std::vector<Statement*> init; // ����forѭ����ʼ���
	Statement* end; // ����forѭ����β�ж����
	std::vector<Statement*> iterate; // ����forѭ���������
	std::vector<Statement*> forEachDeclare; // ������ǿfor�ж����
};

/***************************************
* ������    breakStatement
* ���ã�    ������break���������
* ���ࣺ    Statement
****************************************/
class breakStatement :public Statement {
public:
	/***************************************
	* ��������  breakStatement
	* ���ã�    ������
	* ������    ��
	****************************************/
	breakStatement(int line);
	/***************************************
	* ��������  ~breakStatement
	* ���ã�    ��������
	****************************************/
	~breakStatement();
	/***************************************
	* ��������  getInstanceName
	* ���ã�    �麯�����䣬��ȡ����
	* ������    ��
	* ����ֵ��  stirng
	****************************************/
	virtual std::string getInstanceName() {
		return "breakStatement";
	}
	/***************************************
	* ��������  getInfo
	* ���ã�    �麯������ӡ������ʱ��ӡ�Լ�������Ϣ
	* ������    level        ��ǰ�������[in]
	* ����ֵ��  ��
	****************************************/
	void getInfo(int level);
private:
	int line; // break�����������
};

/***************************************
* ������    returnStatement
* ���ã�    ������return���������
* ���ࣺ    Statement
****************************************/
class returnStatement :public Statement {
public:
	/***************************************
	* ��������  returnStatement
	* ���ã�    ������
	* ������    line        return���ִ������[in]
	****************************************/
	returnStatement(int line);
	/***************************************
	* ��������  ~returnStatement
	* ���ã�    ��������
	****************************************/
	~returnStatement();
	/***************************************
	* ��������  getInstanceName
	* ���ã�    �麯�����䣬��ȡ����
	* ������    ��
	* ����ֵ��  stirng
	****************************************/
	virtual std::string getInstanceName() {
		return "returnStatement";
	}
	/***************************************
	* ��������  getInfo
	* ���ã�    �麯������ӡ������ʱ��ӡ�Լ�������Ϣ
	* ������    level        ��ǰ�������[in]
	* ����ֵ��  ��
	****************************************/
	void getInfo(int level);
	int getLine() {
		return this->line;
	}
private:
	int line; // reutrn�����������
};

/***************************************
* ������    OpNodeStatement
* ���ã�    ��������������
* ���ࣺ    Statement
****************************************/
class OpNodeStatement :public Statement {
private:
	std::string OpType; // ���������
	std::string OpDir; // ��������� M˫Ŀ��L��Ŀ������ұ��ʽ��R˫Ŀ���ұ��б��ʽ
	int startLine; // �����ִ������ 
	std::string castType; //���������ԣ�ǿ��ת������
public:
	/***************************************
	* ��������  OpNodeStatement
	* ���ã�    ������
	* ������    OpType        ���������[in]
	*           opDir         ���������[in]
	*           startLine     ��ʼ����[in]
	****************************************/
	OpNodeStatement(std::string OpType, std::string opDir,int startLine);
	/***************************************
	* ��������  OpNodeStatement
	* ���ã�    ������
	* ������    OpType        ���������[in]
	*           opDir         ���������[in]
	*           startLine     ��ʼ����[in]
	*           castType      ǿ��ת������[in]
	****************************************/
	OpNodeStatement(std::string OpType, std::string opDir, int startLine,std::string castType);
	/***************************************
	* ��������  ~OpNodeStatement
	* ���ã�    ��������
	****************************************/
	~OpNodeStatement();
	/***************************************
	* ��������  getInstanceName
	* ���ã�    �麯�����䣬��ȡ����
	* ������    ��
	* ����ֵ��  stirng
	****************************************/
	virtual std::string getInstanceName() {
		return "OpNodeStatement";
	}
	/***************************************
	* ��������  getInfo
	* ���ã�    �麯������ӡ������ʱ��ӡ�Լ�������Ϣ
	* ������    level        ��ǰ�������[in]
	* ����ֵ��  ��
	****************************************/
	void getInfo(int level);

	std::string getOpType() {
		return this->OpType;
	}
	std::string getOpDir() {
		return this->OpDir;
	}
	int getLine() {
		return this->startLine;
	}
	std::string getCastType() {
		return this->castType;
	}
};

/***************************************
* ������    IdNodeStatement
* ���ã�    ��������ʶ�������������ֱ����ֽ��
* ���ࣺ    Statement
****************************************/
class IdNodeStatement :public Statement {
private:
	std::string name; // ����
	LEX_TYPE lex; // �ʷ�����
	int startLine; // ��ʼ����
public:
	/***************************************
	* ��������  IdNodeStatement
	* ���ã�    ������
	* ������    name          ����[in]
	*           lex           �ʷ�����[in]
	*           startLine     ��ʼ����[in]
	****************************************/
	IdNodeStatement(std::string name, LEX_TYPE lex, int startLine);
	/***************************************
	* ��������  ~IdNodeStatement
	* ���ã�    ��������
	****************************************/
	~IdNodeStatement();
	/***************************************
	* ��������  getInstanceName
	* ���ã�    �麯�����䣬��ȡ����
	* ������    ��
	* ����ֵ��  stirng
	****************************************/
	virtual std::string getInstanceName() {
		return "IdNodeStatement";
	}
	/***************************************
	* ��������  getInfo
	* ���ã�    �麯������ӡ������ʱ��ӡ�Լ�������Ϣ
	* ������    level        ��ǰ�������[in]
	* ����ֵ��  ��
	****************************************/
	void getInfo(int level);
	std::string getName() {
		return this->name;
	}
	LEX_TYPE getLex() {
		return this->lex;
	}
	int getLine() {
		return this->startLine;
	}
};

/***************************************
* ������    FunctionStatement
* ���ã�    �������������ý��
* ���ࣺ    Statement
****************************************/
class FunctionStatement :public Statement {
private:
	std::string name; // ������
	std::vector<Statement*> argList; // ���������б�
	int startLine; // ��ʼ����
public:
	/***************************************
	* ��������  FunctionStatement
	* ���ã�    ������
	* ������    name          ����[in]
	*           startLine     ��ʼ����[in]
	****************************************/
	FunctionStatement(std::string name, int startLine);
	/***************************************
	* ��������  ~FunctionStatement
	* ���ã�    ��������
	****************************************/
	~FunctionStatement();
	/***************************************
	* ��������  addArg
	* ���ã�    �ڲ����б�����ӱ��ʽ
	* ������    arg        �������ʽ[in]
	* ����ֵ��  ��
	****************************************/
	void addArg(Statement *arg);
	/***************************************
	* ��������  getInstanceName
	* ���ã�    �麯�����䣬��ȡ����
	* ������    ��
	* ����ֵ��  stirng
	****************************************/
	virtual std::string getInstanceName() {
		return "FunctionStatement";
	}
	/***************************************
	* ��������  getInfo
	* ���ã�    �麯������ӡ������ʱ��ӡ�Լ�������Ϣ
	* ������    level        ��ǰ�������[in]
	* ����ֵ��  ��
	****************************************/
	void getInfo(int level);
	std::string getName() {
		return this->name;
	}
	std::vector<Statement*> getArgList() {
		return this->argList;
	}
	int getLine() {
		return this->startLine;
	}
};
#endif