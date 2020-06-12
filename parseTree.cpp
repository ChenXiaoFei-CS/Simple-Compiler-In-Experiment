/*********************************************************
*  ģ������    parseTree
*  �ļ�����    parseTree.cpp
*  �����ļ���  parseTree.h
*  �� �ã�     ����C/C++���Խ����﷨�����Ļ������ݻ���--������
*  �� �ߣ�     �����
*  ʱ �䣺     2020.6.2
*  �� ����     Version 1.1
*********************************************************/
#include "parseTree.h"

Statement::Statement(){}
Statement::~Statement(){}
/***************************************
* ��������  getParent
* ���ã�    ��ȡ���׽��
* ������    ��
* ����ֵ��  Statement*
****************************************/
Statement* Statement::getParent() {
	return parent;
}
/***************************************
* ��������  addChild
* ���ã�    ��ӽ�㵽�Լ��ĺ��ӣ�ͬʱ�޸ĺ��ӽ��ĸ�ָ��
* ������    child       ���ӽ��ָ��[in]
* ����ֵ��  ��
****************************************/
void Statement::addChild(Statement *child) {
	childs.push_back(child);
	child->parent = this;
}
/***************************************
* ��������  getInfo
* ���ã�    �麯������ӡ������ʱ��ӡ�Լ�������Ϣ
* ������    level        ��ǰ�������[in]
* ����ֵ��  ��
****************************************/
void Statement::getInfo(int level) {
	std::cout << "Unimplement Node" << std::endl;
}

RootStatement::RootStatement() {}
RootStatement::~RootStatement() {}
/***************************************
* ��������  getInfo
* ���ã�    �麯������ӡ������ʱ��ӡ�Լ�������Ϣ
* ������    level        ��ǰ�������[in]
* ����ֵ��  ��
****************************************/
void RootStatement::getInfo(int level) {
	auto space = [](int spaceNum)->void {
		for (int i = 0; i < spaceNum; i++) {
			std::cout << " ";
		}
	};
	space(level);
	std::cout << "[Root]" << std::endl;
	for (int i = 0; i < childs.size(); i++) {
		childs[i]->getInfo(level + 3);
		std::cout << std::endl;
	}
}

FuncDefineStatement::FuncDefineStatement(std::string name, std::string returnType, std::vector<std::string> modifier,int startLine) {
	this->name = name;
	this->returnType = returnType;
	this->modifier = modifier;
	this->startLine = startLine;
}
FuncDefineStatement::~FuncDefineStatement() {}
/***************************************
* ��������  setArgTable
* ���ã�    ��Ӻ��������б�
* ������    argTable      �����б�[in]
* ����ֵ��  ��
****************************************/
void FuncDefineStatement::setArgTable(std::vector<std::pair<std::string, std::string>> argTable) {
	this->argTable = argTable;
}
/***************************************
* ��������  setEndLine
* ���ã�    ��Ӻ�����β����
* ������    endLine       ������β����[in]
* ����ֵ��  ��
****************************************/
void FuncDefineStatement::setEndLine(int endLine) {
	this->endLine = endLine;
}
/***************************************
* ��������  getInfo
* ���ã�    �麯������ӡ������ʱ��ӡ�Լ�������Ϣ
* ������    level        ��ǰ�������[in]
* ����ֵ��  ��
****************************************/
void FuncDefineStatement::getInfo(int level) {
	auto space = [](int spaceNum)->void {
		for (int i = 0; i < spaceNum; i++) {
			std::cout << " ";
		}
	};
	auto hline = [](int spaceNum)->void {
		for (int i = 0; i < spaceNum; i++) {
			std::cout << "-";
		}
	};
	hline(level);
	std::cout << "[FuncDefine]: {Name: " << name << " ,Line:" << startLine << " }" << std::endl;
	space(level + 14);
	std::cout << "{ReturnType:" << returnType << " ,Modifier: ";
	if (modifier.size() > 0) {
		for (int i = 0; i < modifier.size(); i++) {
			std::cout << modifier[i] << " ";
		}
	}
	else {
		std::cout << "None ";
	}
	std::cout << ",ArgTable:";
	for (int i = 0; i < argTable.size(); i++) {
		std::cout << "{" << argTable[i].first << ":" << argTable[i].second << "}";
	}
	std::cout << "}" << std::endl;
	for (int i = 0; i < childs.size(); i++) {
		childs[i]->getInfo(level + 6);
		std::cout << std::endl;
	}
}

std::string FuncDefineStatement::getName() {
	return this->name;
}
std::vector<std::pair<std::string,std::string>> FuncDefineStatement::getArgTable() {
	return this->argTable;
}

std::vector<std::string> FuncDefineStatement::getModifiers() {
	return this->modifier;
}

int FuncDefineStatement::getStartLine() {
	return this->startLine;
}
int FuncDefineStatement::getEndLine() {
	return this->endLine;
}
std::string FuncDefineStatement::getReturnType() {
	return this->returnType;
}

VarDeclarationStatement::VarDeclarationStatement(std::string name, std::string type, std::vector<std::string> modifier, int startLine) {
	this->name = name;
	this->type = type;
	this->modifiers = modifier;
	this->startLine = startLine;
}
VarDeclarationStatement::~VarDeclarationStatement() {}
/***************************************
* ��������  getInfo
* ���ã�    �麯������ӡ������ʱ��ӡ�Լ�������Ϣ
* ������    level        ��ǰ�������[in]
* ����ֵ��  ��
****************************************/
void VarDeclarationStatement::getInfo(int level) {
	auto space = [](int spaceNum)->void {
		for (int i = 0; i < spaceNum; i++) {
			std::cout << " ";
		}
	};
	auto hline = [](int spaceNum)->void {
		for (int i = 0; i < spaceNum; i++) {
			std::cout << "-";
		}
	};
	hline(level);
	std::cout << "[VarDeclare]: {Name: " << name << " ,Line:" << startLine << " }" << std::endl;
	space(level + 14);
	std::cout << "{Type:" << type << ",Modifier: ";
	if (modifiers.size() > 0) {
		for (int i = 0; i < modifiers.size(); i++) {
			std::cout << modifiers[i] << " ";
		}
	}
	else {
		std::cout << "None ";
	}
	std::cout << "}" << std::endl;
}

std::string VarDeclarationStatement::getName() {
	return this->name;
}
std::string VarDeclarationStatement::getType() {
	return this->type;
}
std::vector<std::string> VarDeclarationStatement::getModifiers() {
	return this->modifiers;
}
int VarDeclarationStatement::getLine() {
	return this->startLine;
}


branchStatement::branchStatement(std::string type,int startLine) {
	this->branchType = type;
	this->startLine = startLine;
}
branchStatement::~branchStatement() {}
/***************************************
* ��������  setEndLine
* ���ã�    �趨��֧����������
* ������    endLine        ��������[in]
* ����ֵ��  ��
****************************************/
void branchStatement::setEndLine(int endLine) {
	this->endLine = endLine;
}
/***************************************
* ��������  setHasBrack
* ���ã�    �趨�Ƿ�ʹ�ô����Ž������
* ������    hasBrack       �Ƿ�ʹ�ô����Ž������[in]
* ����ֵ��  ��
****************************************/
void branchStatement::setHasBrack(bool hasBrank) {
	this->hasBrack = hasBrank;
}
/***************************************
* ��������  getHasBrack
* ���ã�    ��֧�����Ƿ�ʹ�ô����Ž������
* ������    ��
* ����ֵ��  bool
****************************************/
bool branchStatement::getHasBrack() {
	return this->hasBrack;
}
/***************************************
* ��������  getStartLine
* ���ã�    ��ȡ��֧�����ʼ����
* ������    ��
* ����ֵ��  int
****************************************/
int branchStatement::getStartLine() {
	return this->startLine;
}
/***************************************
* ��������  getInfo
* ���ã�    �麯������ӡ������ʱ��ӡ�Լ�������Ϣ
* ������    level        ��ǰ�������[in]
* ����ֵ��  ��
****************************************/
void branchStatement::getInfo(int level) {
	auto space = [](int spaceNum)->void {
		for (int i = 0; i < spaceNum; i++) {
			std::cout << " ";
		}
	};
	auto hline = [](int spaceNum)->void {
		for (int i = 0; i < spaceNum; i++) {
			std::cout << "-";
		}
	};
	hline(level);
	std::cout << "[Branch]: {Type: " << branchType << " ,Line:" << startLine << " }" << std::endl;
	if (branchType == "if" || branchType == "else if" || branchType == "switch" || branchType == "case" || branchType == "while") {
		space(level + 10);
		std::cout << "Judgement:" << std::endl;
		this->judgement->getInfo(level + 10);
	}
	if (branchType == "for") {
		if (this->forEachDeclare.size() != 0) {
			std::cout << "Foreach:" << std::endl;
			for (int i = 0; i < this->forEachDeclare.size(); i++) {
				this->forEachDeclare[i]->getInfo(level + 10);
			}
		}
		else {
			space(level + 10);
			std::cout << "Init:" << std::endl;
			for (int i = 0; i < this->init.size(); i++) {
				this->init[i]->getInfo(level + 10);
			}
			std::cout << std::endl;
			space(level + 10);
			std::cout << "End:" << std::endl;
			this->end->getInfo(level + 10);
			std::cout << std::endl;
			space(level + 10);
			std::cout << "Iterate:" << std::endl;
			for (int i = 0; i < this->iterate.size(); i++) {
				this->iterate[i]->getInfo(level + 10);
			}
		}
	}
	for (int i = 0; i < childs.size(); i++) {
		childs[i]->getInfo(level + 6);
		std::cout << std::endl;
	}
}
breakStatement::breakStatement(int startLine) {
	this->line = startLine;
}
breakStatement::~breakStatement() {}
/***************************************
* ��������  getInfo
* ���ã�    �麯������ӡ������ʱ��ӡ�Լ�������Ϣ
* ������    level        ��ǰ�������[in]
* ����ֵ��  ��
****************************************/
void breakStatement::getInfo(int level) {
	auto space = [](int spaceNum)->void {
		for (int i = 0; i < spaceNum; i++) {
			std::cout << " ";
		}
	};
	auto hline = [](int spaceNum)->void {
		for (int i = 0; i < spaceNum; i++) {
			std::cout << "-";
		}
	};
	hline(level);
	std::cout << "[Break]: {Line:" << line << " }" << std::endl;
}

returnStatement::returnStatement(int startLine) {
	this->line = startLine;
}
returnStatement::~returnStatement() {}
/***************************************
* ��������  getInfo
* ���ã�    �麯������ӡ������ʱ��ӡ�Լ�������Ϣ
* ������    level        ��ǰ�������[in]
* ����ֵ��  ��
****************************************/
void returnStatement::getInfo(int level) {
	auto space = [](int spaceNum)->void {
		for (int i = 0; i < spaceNum; i++) {
			std::cout << " ";
		}
	};
	auto hline = [](int spaceNum)->void {
		for (int i = 0; i < spaceNum; i++) {
			std::cout << "-";
		}
	};
	hline(level);
	std::cout << "[Return]: {Line:" << line << " }" << std::endl;
	for (int i = 0; i < childs.size(); i++) {
		childs[i]->getInfo(level + 4);
	}
}

OpNodeStatement::OpNodeStatement(std::string opType,std::string opDir,int startLine) {
	this->OpType = opType;
	this->OpDir = opDir;
	this->startLine = startLine;
}
OpNodeStatement::OpNodeStatement(std::string opType, std::string opDir, int startLine, std::string castType) {
	this->OpType = opType;
	this->OpDir = opDir;
	this->startLine = startLine;
	this->castType = castType;
}
OpNodeStatement::~OpNodeStatement() {}
/***************************************
* ��������  getInfo
* ���ã�    �麯������ӡ������ʱ��ӡ�Լ�������Ϣ
* ������    level        ��ǰ�������[in]
* ����ֵ��  ��
****************************************/
void OpNodeStatement::getInfo(int level) {
	auto space = [](int spaceNum)->void {
		for (int i = 0; i < spaceNum; i++) {
			std::cout << " ";
		}
	};
	auto hline = [](int spaceNum)->void {
		for (int i = 0; i < spaceNum; i++) {
			std::cout << "-";
		}
	};
	if (OpDir == "M" || OpDir == "L") {
		childs[0]->getInfo(level + 4);
	}
	hline(level);
	std::cout << "[Operator]: {OpType: " << OpType << " ,Line:" << startLine << " }" << std::endl;
	space(level + 12);
	std::cout << "{OpDir: " << OpDir;
	if (OpType == "Cast") {
		std::cout << " ,CaseType: " << castType;
	}
	std::cout << " }" << std::endl;
	if (OpDir == "M") {
		childs[1]->getInfo(level + 4);
	}
	if (OpDir == "R") {
		childs[0]->getInfo(level + 4);
	}
}

IdNodeStatement::IdNodeStatement(std::string name, LEX_TYPE lex, int startLine) {
	this->name = name;
	this->lex = lex;
	this->startLine = startLine;
}
IdNodeStatement::~IdNodeStatement(){}
/***************************************
* ��������  getInfo
* ���ã�    �麯������ӡ������ʱ��ӡ�Լ�������Ϣ
* ������    level        ��ǰ�������[in]
* ����ֵ��  ��
****************************************/
void IdNodeStatement::getInfo(int level) {
	auto space = [](int spaceNum)->void {
		for (int i = 0; i < spaceNum; i++) {
			std::cout << " ";
		}
	};
	auto hline = [](int spaceNum)->void {
		for (int i = 0; i < spaceNum; i++) {
			std::cout << "-";
		}
	};
	LEX_TYPE type = lex;
	std::string typeStr = (type == LEX_TYPE::SEPERATOR) ? "SEPERATOR" : ((type == LEX_TYPE::IDENTIFIER) ? "IDENTIFIER" : ((type == LEX_TYPE::OPERATOR) ? "OPERATOR" : (type == LEX_TYPE::CONST) ? "CONST" : "KEYWORD"));
	hline(level);
	std::cout << "[IdNode]: { Name: " << name << " ,Line: " << startLine << " }" << std::endl;
	space(level + 10);
	std::cout << "{LexType:" << typeStr << " }" << std::endl;
}

FunctionStatement::FunctionStatement(std::string name, int startLine) {
	this->name = name;
	this->startLine = startLine;
}
FunctionStatement::~FunctionStatement() {}
/***************************************
* ��������  addArg
* ���ã�    �ڲ����б�����ӱ��ʽ
* ������    arg        �������ʽ[in]
* ����ֵ��  ��
****************************************/
void FunctionStatement::addArg(Statement* st) {
	this->argList.push_back(st);
}
/***************************************
* ��������  getInfo
* ���ã�    �麯������ӡ������ʱ��ӡ�Լ�������Ϣ
* ������    level        ��ǰ�������[in]
* ����ֵ��  ��
****************************************/
void FunctionStatement::getInfo(int level) {
	auto space = [](int spaceNum)->void {
		for (int i = 0; i < spaceNum; i++) {
			std::cout << " ";
		}
	};
	auto hline = [](int spaceNum)->void {
		for (int i = 0; i < spaceNum; i++) {
			std::cout << "-";
		}
	};
	hline(level);
	std::cout << "[Function]: {Name: " << name << " ,Line:" << startLine << " }" << std::endl;
	space(level + 12);
	std::cout << "ArgList: " << argList.size() << " Members" << std::endl;
	for (int i = 0; i < argList.size(); i++) {
		argList[i]->getInfo(level + 12);
	}
}