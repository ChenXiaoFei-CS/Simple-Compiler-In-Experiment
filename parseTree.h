/*********************************************************
*  模块名：    parseTree
*  文件名：    parseTree.h
*  依赖文件：  Base.h
*  作 用：     对于C/C++语言进行语法分析的基础数据机构--解析树
*  作 者：     陈骁飞
*  时 间：     2020.6.2
*  版 本：     Version 1.1
*********************************************************/
#ifndef _PARSETREE_H_
#define _PARSETREE_H_
#include"Base.h"
#include <vector>
#include <iostream>

/***************************************
* 类名：    Statement
* 作用：    解析树基础结点父类
****************************************/
class Statement
{
public:
	/***************************************
	* 函数名：  Statement
	* 作用：    构造器
	* 参数：    无
	****************************************/
	Statement();
	/***************************************
	* 函数名：  ~Statement
	* 作用：    析构函数
	****************************************/
	~Statement();
	/***************************************
	* 函数名：  getParent
	* 作用：    获取父亲结点
	* 参数：    无
	* 返回值：  Statement*
	****************************************/
	Statement* getParent();
	/***************************************
	* 函数名：  addChild
	* 作用：    添加结点到自己的孩子，同时修改孩子结点的父指针
	* 参数：    child       孩子结点指针[in]
	* 返回值：  无
	****************************************/
	void addChild(Statement* child);
	/***************************************
	* 函数名：  getInstanceName
	* 作用：    虚函数反射，获取类名
	* 参数：    无
	* 返回值：  stirng
	****************************************/
	virtual std::string getInstanceName() {
		return "Statement";
	}
	/***************************************
	* 函数名：  getInfo
	* 作用：    虚函数，打印解析树时打印自己结点的信息
	* 参数：    level        当前所处层次[in]
	* 返回值：  无
	****************************************/
	virtual void getInfo(int level);
	/***************************************
	* 函数名：  getChildList
	* 作用：    虚函数，获取孩子列表
	* 参数：    无
	* 返回值：  std::vector<Statement*>
	****************************************/
	virtual std::vector<Statement*> getChildList() {
		return this->childs;
	}
protected:
	std::vector<Statement*> childs; // 孩子结点指针列表
	Statement* parent = NULL; // 父亲结点指针
};

/***************************************
* 类名：    RootStatement
* 作用：    解析树根结点
* 父类：    Statement
****************************************/
class RootStatement :public Statement{
public:
	/***************************************
	* 函数名：  RootStatement
	* 作用：    构造器
	* 参数：    无
	****************************************/
	RootStatement();
	/***************************************
	* 函数名：  ~RootStatement
	* 作用：    析构函数
	****************************************/
	~RootStatement();
	/***************************************
	* 函数名：  getInstanceName
	* 作用：    虚函数反射，获取类名
	* 参数：    无
	* 返回值：  stirng
	****************************************/
	virtual std::string getInstanceName() {
		return "RootStatement";
	}
	/***************************************
	* 函数名：  getInfo
	* 作用：    虚函数，打印解析树时打印自己结点的信息
	* 参数：    level        当前所处层次[in]
	* 返回值：  无
	****************************************/
	void getInfo(int level);
};

/***************************************
* 类名：    FuncDefineStatement
* 作用：    解析树函数定义语句结点
* 父类：    Statement
****************************************/
class FuncDefineStatement :public Statement {
public:
	/***************************************
	* 函数名：  FuncDefineStatement
	* 作用：    构造器
	* 参数：    name       函数名        [in]
	*           returnType 函数返回值类型[in]
	*           modifier   函数修饰符    [in]
	*           startLine  函数定义行数  [in]
	****************************************/
	FuncDefineStatement(std::string name,std::string returnType, std::vector<std::string> modifier,int startLine);
	/***************************************
	* 函数名：  ~FuncDefineStatement
	* 作用：    析构函数
	****************************************/
	~FuncDefineStatement();
	/***************************************
	* 函数名：  setArgTable
	* 作用：    添加函数参数列表
	* 参数：    argTable      参数列表[in]
	* 返回值：  无
	****************************************/
	void setArgTable(std::vector<std::pair<std::string, std::string>> argTable);
	/***************************************
	* 函数名：  setEndLine
	* 作用：    添加函数结尾行数
	* 参数：    endLine       函数结尾行数[in]
	* 返回值：  无
	****************************************/
	void setEndLine(int endLine);
	/***************************************
	* 函数名：  getInstanceName
	* 作用：    虚函数反射，获取类名
	* 参数：    无
	* 返回值：  stirng
	****************************************/
	virtual std::string getInstanceName() {
		return "FuncDefineStatement";
	}
	/***************************************
	* 函数名：  getInfo
	* 作用：    虚函数，打印解析树时打印自己结点的信息
	* 参数：    level        当前所处层次[in]
	* 返回值：  无
	****************************************/
	void getInfo(int level);
	std::string getName();
	std::vector<std::string> getModifiers();
	std::vector<std::pair<std::string, std::string>> getArgTable();
	int getStartLine();
	int getEndLine();
	std::string getReturnType();
private:
	std::string returnType; // 函数的返回值类型
	std::vector<std::string> modifier; // 函数的修饰符
	std::vector<std::pair<std::string, std::string>> argTable; // 函数的参数列表
	std::string name; // 函数名
	int startLine; // 函数定义的起始行
	int endLine; // 函数定义的结尾行
};

/***************************************
* 类名：    VarDeclarationStatement
* 作用：    解析树变量声明语句结点
* 父类：    Statement
****************************************/
class VarDeclarationStatement :public Statement{
private:
	std::string name; // 变量名
	std::string type; // 变量类型
	std::vector<std::string> modifiers; // 变量修饰符
	int startLine; // 变量声明行数
public:
	/***************************************
	* 函数名：  VarDeclarationStatement
	* 作用：    构造器
	* 参数：    name       变量名        [in]
	*           type       变量类型      [in]
	*           modifier   变量修饰符    [in]
	*           startLine  变量声明行数  [in]
	****************************************/
	VarDeclarationStatement(std::string name, std::string type, std::vector<std::string> modifier, int startLine);
	/***************************************
	* 函数名：  ~VarDeclarationStatement
	* 作用：    析构函数
	****************************************/
	~VarDeclarationStatement();
	/***************************************
	* 函数名：  getInstanceName
	* 作用：    虚函数反射，获取类名
	* 参数：    无
	* 返回值：  stirng
	****************************************/
	virtual std::string getInstanceName() {
		return "VarDeclarationStatement";
	}
	/***************************************
	* 函数名：  getInfo
	* 作用：    虚函数，打印解析树时打印自己结点的信息
	* 参数：    level        当前所处层次[in]
	* 返回值：  无
	****************************************/
	void getInfo(int level);

	std::string getName();
	std::string getType();
	std::vector<std::string> getModifiers();
	int getLine();
};

/***************************************
* 类名：    branchStatement
* 作用：    解析树分支语句结点
* 父类：    Statement
****************************************/
class branchStatement :public Statement {
private:
	std::string branchType; // 分支的类型
	int startLine; // 分支语句进入行数
	int endLine; // 分支语句结束行数
	bool hasBrack = false; // 分支语句是否使用大括号作为界符
public:
	/***************************************
	* 函数名：  branchStatement
	* 作用：    构造器
	* 参数：    type       分支类型      [in]
	*           startLine  变量声明行数  [in]
	****************************************/
	branchStatement(std::string type,int startLine);
	/***************************************
	* 函数名：  ~branchStatement
	* 作用：    析构函数
	****************************************/
	~branchStatement();
	/***************************************
	* 函数名：  setHasBrack
	* 作用：    设定是否使用大括号界符属性
	* 参数：    hasBrack       是否使用大括号界符属性[in]
	* 返回值：  无
	****************************************/
	void setHasBrack(bool hasBrack);
	/***************************************
	* 函数名：  setEndLine
	* 作用：    设定分支语句结束行数
	* 参数：    endLine        结束行数[in]
	* 返回值：  无
	****************************************/
	void setEndLine(int endLine);
	/***************************************
	* 函数名：  getHasBrack
	* 作用：    分支局与是否使用大括号界符属性
	* 参数：    无
	* 返回值：  bool
	****************************************/
	bool getHasBrack();
	/***************************************
	* 函数名：  getStartLine
	* 作用：    获取分支语句起始行数
	* 参数：    无
	* 返回值：  int
	****************************************/
	int getStartLine();
	/***************************************
	* 函数名：  getInstanceName
	* 作用：    虚函数反射，获取类名
	* 参数：    无
	* 返回值：  stirng
	****************************************/
	virtual std::string getInstanceName() {
		return "branchStatement";
	}
	/***************************************
	* 函数名：  getInfo
	* 作用：    虚函数，打印解析树时打印自己结点的信息
	* 参数：    level        当前所处层次[in]
	* 返回值：  无
	****************************************/
	void getInfo(int level);
	std::string getbranchType() {
		return this->branchType;
	}
	Statement* judgement; // 用于if,while,switch循环的判断语句
	std::vector<Statement*> init; // 用于for循环起始语句
	Statement* end; // 用于for循环结尾判断语句
	std::vector<Statement*> iterate; // 用于for循环迭代语句
	std::vector<Statement*> forEachDeclare; // 用于增强for判断语句
};

/***************************************
* 类名：    breakStatement
* 作用：    解析树break特殊语句结点
* 父类：    Statement
****************************************/
class breakStatement :public Statement {
public:
	/***************************************
	* 函数名：  breakStatement
	* 作用：    构造器
	* 参数：    无
	****************************************/
	breakStatement(int line);
	/***************************************
	* 函数名：  ~breakStatement
	* 作用：    析构函数
	****************************************/
	~breakStatement();
	/***************************************
	* 函数名：  getInstanceName
	* 作用：    虚函数反射，获取类名
	* 参数：    无
	* 返回值：  stirng
	****************************************/
	virtual std::string getInstanceName() {
		return "breakStatement";
	}
	/***************************************
	* 函数名：  getInfo
	* 作用：    虚函数，打印解析树时打印自己结点的信息
	* 参数：    level        当前所处层次[in]
	* 返回值：  无
	****************************************/
	void getInfo(int level);
private:
	int line; // break语句所属行数
};

/***************************************
* 类名：    returnStatement
* 作用：    解析树return特殊语句结点
* 父类：    Statement
****************************************/
class returnStatement :public Statement {
public:
	/***************************************
	* 函数名：  returnStatement
	* 作用：    构造器
	* 参数：    line        return语句执行行数[in]
	****************************************/
	returnStatement(int line);
	/***************************************
	* 函数名：  ~returnStatement
	* 作用：    析构函数
	****************************************/
	~returnStatement();
	/***************************************
	* 函数名：  getInstanceName
	* 作用：    虚函数反射，获取类名
	* 参数：    无
	* 返回值：  stirng
	****************************************/
	virtual std::string getInstanceName() {
		return "returnStatement";
	}
	/***************************************
	* 函数名：  getInfo
	* 作用：    虚函数，打印解析树时打印自己结点的信息
	* 参数：    level        当前所处层次[in]
	* 返回值：  无
	****************************************/
	void getInfo(int level);
	int getLine() {
		return this->line;
	}
private:
	int line; // reutrn语句所属行数
};

/***************************************
* 类名：    OpNodeStatement
* 作用：    解析树运算符结点
* 父类：    Statement
****************************************/
class OpNodeStatement :public Statement {
private:
	std::string OpType; // 运算符内容
	std::string OpDir; // 运算符方向 M双目，L单目，左边右表达式，R双目，右边有表达式
	int startLine; // 运算符执行行数 
	std::string castType; //（附加属性）强制转型类型
public:
	/***************************************
	* 函数名：  OpNodeStatement
	* 作用：    构造器
	* 参数：    OpType        运算符内容[in]
	*           opDir         运算符方向[in]
	*           startLine     起始行数[in]
	****************************************/
	OpNodeStatement(std::string OpType, std::string opDir,int startLine);
	/***************************************
	* 函数名：  OpNodeStatement
	* 作用：    构造器
	* 参数：    OpType        运算符内容[in]
	*           opDir         运算符方向[in]
	*           startLine     起始行数[in]
	*           castType      强制转型类型[in]
	****************************************/
	OpNodeStatement(std::string OpType, std::string opDir, int startLine,std::string castType);
	/***************************************
	* 函数名：  ~OpNodeStatement
	* 作用：    析构函数
	****************************************/
	~OpNodeStatement();
	/***************************************
	* 函数名：  getInstanceName
	* 作用：    虚函数反射，获取类名
	* 参数：    无
	* 返回值：  stirng
	****************************************/
	virtual std::string getInstanceName() {
		return "OpNodeStatement";
	}
	/***************************************
	* 函数名：  getInfo
	* 作用：    虚函数，打印解析树时打印自己结点的信息
	* 参数：    level        当前所处层次[in]
	* 返回值：  无
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
* 类名：    IdNodeStatement
* 作用：    解析树标识符，常量，部分保留字结点
* 父类：    Statement
****************************************/
class IdNodeStatement :public Statement {
private:
	std::string name; // 名称
	LEX_TYPE lex; // 词法类型
	int startLine; // 起始行数
public:
	/***************************************
	* 函数名：  IdNodeStatement
	* 作用：    构造器
	* 参数：    name          名称[in]
	*           lex           词法类型[in]
	*           startLine     起始行数[in]
	****************************************/
	IdNodeStatement(std::string name, LEX_TYPE lex, int startLine);
	/***************************************
	* 函数名：  ~IdNodeStatement
	* 作用：    析构函数
	****************************************/
	~IdNodeStatement();
	/***************************************
	* 函数名：  getInstanceName
	* 作用：    虚函数反射，获取类名
	* 参数：    无
	* 返回值：  stirng
	****************************************/
	virtual std::string getInstanceName() {
		return "IdNodeStatement";
	}
	/***************************************
	* 函数名：  getInfo
	* 作用：    虚函数，打印解析树时打印自己结点的信息
	* 参数：    level        当前所处层次[in]
	* 返回值：  无
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
* 类名：    FunctionStatement
* 作用：    解析树函数调用结点
* 父类：    Statement
****************************************/
class FunctionStatement :public Statement {
private:
	std::string name; // 函数名
	std::vector<Statement*> argList; // 函数参数列表
	int startLine; // 起始行数
public:
	/***************************************
	* 函数名：  FunctionStatement
	* 作用：    构造器
	* 参数：    name          名称[in]
	*           startLine     起始行数[in]
	****************************************/
	FunctionStatement(std::string name, int startLine);
	/***************************************
	* 函数名：  ~FunctionStatement
	* 作用：    析构函数
	****************************************/
	~FunctionStatement();
	/***************************************
	* 函数名：  addArg
	* 作用：    在参数列表中添加表达式
	* 参数：    arg        参数表达式[in]
	* 返回值：  无
	****************************************/
	void addArg(Statement *arg);
	/***************************************
	* 函数名：  getInstanceName
	* 作用：    虚函数反射，获取类名
	* 参数：    无
	* 返回值：  stirng
	****************************************/
	virtual std::string getInstanceName() {
		return "FunctionStatement";
	}
	/***************************************
	* 函数名：  getInfo
	* 作用：    虚函数，打印解析树时打印自己结点的信息
	* 参数：    level        当前所处层次[in]
	* 返回值：  无
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