/*********************************************************
*  模块名：    parseTree
*  文件名：    parseTree.cpp
*  依赖文件：  parseTree.h
*  作 用：     对于C/C++语言进行语法分析的基础数据机构--解析树
*  作 者：     陈骁飞
*  时 间：     2020.6.2
*  版 本：     Version 1.1
*********************************************************/
#include "parseTree.h"

Statement::Statement(){}
Statement::~Statement(){}
/***************************************
* 函数名：  getParent
* 作用：    获取父亲结点
* 参数：    无
* 返回值：  Statement*
****************************************/
Statement* Statement::getParent() {
	return parent;
}
/***************************************
* 函数名：  addChild
* 作用：    添加结点到自己的孩子，同时修改孩子结点的父指针
* 参数：    child       孩子结点指针[in]
* 返回值：  无
****************************************/
void Statement::addChild(Statement *child) {
	childs.push_back(child);
	child->parent = this;
}
/***************************************
* 函数名：  getInfo
* 作用：    虚函数，打印解析树时打印自己结点的信息
* 参数：    level        当前所处层次[in]
* 返回值：  无
****************************************/
void Statement::getInfo(int level) {
	std::cout << "Unimplement Node" << std::endl;
}

RootStatement::RootStatement() {}
RootStatement::~RootStatement() {}
/***************************************
* 函数名：  getInfo
* 作用：    虚函数，打印解析树时打印自己结点的信息
* 参数：    level        当前所处层次[in]
* 返回值：  无
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
* 函数名：  setArgTable
* 作用：    添加函数参数列表
* 参数：    argTable      参数列表[in]
* 返回值：  无
****************************************/
void FuncDefineStatement::setArgTable(std::vector<std::pair<std::string, std::string>> argTable) {
	this->argTable = argTable;
}
/***************************************
* 函数名：  setEndLine
* 作用：    添加函数结尾行数
* 参数：    endLine       函数结尾行数[in]
* 返回值：  无
****************************************/
void FuncDefineStatement::setEndLine(int endLine) {
	this->endLine = endLine;
}
/***************************************
* 函数名：  getInfo
* 作用：    虚函数，打印解析树时打印自己结点的信息
* 参数：    level        当前所处层次[in]
* 返回值：  无
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
* 函数名：  getInfo
* 作用：    虚函数，打印解析树时打印自己结点的信息
* 参数：    level        当前所处层次[in]
* 返回值：  无
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
* 函数名：  setEndLine
* 作用：    设定分支语句结束行数
* 参数：    endLine        结束行数[in]
* 返回值：  无
****************************************/
void branchStatement::setEndLine(int endLine) {
	this->endLine = endLine;
}
/***************************************
* 函数名：  setHasBrack
* 作用：    设定是否使用大括号界符属性
* 参数：    hasBrack       是否使用大括号界符属性[in]
* 返回值：  无
****************************************/
void branchStatement::setHasBrack(bool hasBrank) {
	this->hasBrack = hasBrank;
}
/***************************************
* 函数名：  getHasBrack
* 作用：    分支局与是否使用大括号界符属性
* 参数：    无
* 返回值：  bool
****************************************/
bool branchStatement::getHasBrack() {
	return this->hasBrack;
}
/***************************************
* 函数名：  getStartLine
* 作用：    获取分支语句起始行数
* 参数：    无
* 返回值：  int
****************************************/
int branchStatement::getStartLine() {
	return this->startLine;
}
/***************************************
* 函数名：  getInfo
* 作用：    虚函数，打印解析树时打印自己结点的信息
* 参数：    level        当前所处层次[in]
* 返回值：  无
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
* 函数名：  getInfo
* 作用：    虚函数，打印解析树时打印自己结点的信息
* 参数：    level        当前所处层次[in]
* 返回值：  无
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
* 函数名：  getInfo
* 作用：    虚函数，打印解析树时打印自己结点的信息
* 参数：    level        当前所处层次[in]
* 返回值：  无
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
* 函数名：  getInfo
* 作用：    虚函数，打印解析树时打印自己结点的信息
* 参数：    level        当前所处层次[in]
* 返回值：  无
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
* 函数名：  getInfo
* 作用：    虚函数，打印解析树时打印自己结点的信息
* 参数：    level        当前所处层次[in]
* 返回值：  无
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
* 函数名：  addArg
* 作用：    在参数列表中添加表达式
* 参数：    arg        参数表达式[in]
* 返回值：  无
****************************************/
void FunctionStatement::addArg(Statement* st) {
	this->argList.push_back(st);
}
/***************************************
* 函数名：  getInfo
* 作用：    虚函数，打印解析树时打印自己结点的信息
* 参数：    level        当前所处层次[in]
* 返回值：  无
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