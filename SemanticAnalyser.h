/*********************************************************
*  模块名：    SemanticAnalyser
*  文件名：    SemanticAnalyser.h
*  依赖文件：  parseTree.h,Base.h
*  作 用：     对于语法解析树进行语义分析
*  作 者：     陈骁飞
*  时 间：     2020.6.12
*  版 本：     Version 1.1
**********************************************************/
#ifndef _SEMANTICANALYSER_H_
#define _SEMANTICANALYSER_H_
#include "parseTree.h"
#include "Base.h"
#include <vector>

/***************************************
* 函数名：  convertSemanticType2String
* 作用：    将语义类型转换未字符串
* 参数：    type       语义类型[in]
* 返回值：  string
****************************************/
std::string convertSemanticType2String(SemanticType type);

/***************************************
* 类名：    classInfo
* 作用：    类内容的抽象，用于表示语义类型
****************************************/
class classInfo {
public:
	/***************************************
	* 函数名：  classInfo
	* 作用：    构造器
	* 参数：    name:           类名[in]
	*           templates       模板（泛型）[in]
	****************************************/
	classInfo(std::string name,std::vector<SemanticType> templates) {
		this->className = name;
		this->templates = templates;
	}
	std::string className; // 类名
	std::vector<SemanticType> templates; // 模板（泛型）
};

/***************************************
* 类名：    funcTableItem
* 作用：    函数表的表项
****************************************/
class funcTableItem {
public:
	/***************************************
	* 函数名：  funcTableItem
	* 作用：    构造器
	* 参数：    level          函数所在层级  [in]
	*           name           函数名        [in]
	*           type           函数的语义类型[in]
	*           startLine      函数的开始行数[in]
	*           endLine        函数的结束行数[in]
	****************************************/
	funcTableItem(int level, std::string name, SemanticType type, int startLine, int endLine) {
		this->level = level;
		this->funcName = name;
		this->returnType = type;
		this->startLine = startLine;
		this->endLine = endLine;
	}
	/***************************************
	* 函数名：  fillInArgTable
	* 作用：    填充函数表项中的参数类型
	* 参数：    args       参数列表的语义类型[in]
	* 返回值：  void
	****************************************/
	void fillInArgTable(std::vector<SemanticType> args);
	/***************************************
	* 函数名：  fillInReturnClassInfo
	* 作用：    备用项，填充函数表项中的返回值类型关联类信息
	* 参数：    info       返回值类型关联类信息[in]
	* 返回值：  void
	****************************************/
	void fillInReturnClassInfo(classInfo *info);
	/***************************************
	* 函数名：  fillInReturnClassInfo
	* 作用：    备用项，填充函数表项中的参数类型关联类信息
	* 参数：    info       参数类型关联类信息[in]
	* 返回值：  void
	****************************************/
	void fillInArgClassInfo(classInfo* info);
	/***************************************
	* 函数名：  fillInModfiers
	* 作用：    填充函数表项中的修饰符列表
	* 参数：    modifiers       修饰符列表[in]
	* 返回值：  void
	****************************************/
	void fillInModfiers(std::vector<std::string> modifiers);
	std::string getName(); // 获取类名
	std::vector<SemanticType> getArgTable(); // 获取参数类型列表
	std::vector<classInfo*> getArgClassInfo(); // 获取参数类型关联类信息列表
	SemanticType getReturnType() { // 获取返回值语义类型
		return this->returnType;
	}
	classInfo* getReturnInfo() { // 获取返回值关联类信息列表
		return this->returnClass;
	}
	/***************************************
	* 函数名：  printInfo
	* 作用：    打印函数表表项信息
	* 参数：    无
	* 返回值：  void
	****************************************/
	void printInfo();
private:
	int level; // 所在层次级别
	std::string funcName; // 函数名
	std::vector<std::string> modifiers; // 修饰符
	std::vector<SemanticType> argList; // 函数的参数列表
	SemanticType returnType; // 函数返回类型
	std::vector<classInfo*> argClassInfoTable; // 备选项，如果参数列表中有class
	classInfo* returnClass; // 备选项，如果返回类型为class
	int startLine; // 起始行数
	int endLine; // 结束行数
};

/***************************************
* 类名：    varTableItem
* 作用：    变量表的表项
****************************************/
class varTableItem {
public:
	/***************************************
	* 函数名：  varTableItem
	* 作用：    构造器
	* 参数：    level           变量所在层级          [in]
	*           type            变量语义类型          [in]
	*           info            变量语义类型关联类信息[in]
	*           modifiers       修饰符列表            [in]
	*           line            变量声明行数          [in]
	****************************************/
	varTableItem(int level,std::string name,SemanticType type,classInfo *info,std::vector<std::string> modifiers,int line) {
		this->level = level;
		this->type = type;
		this->name = name;
		this->info = info;
		this->modifiers = modifiers;
		this->line = line;
	}
	std::string getName() {  // 获得变量名
		return this->name;
	}
	int getLevel() { // 获得变量所在层级
		return this->level;
	}
	bool getDeprecated() {  // ** 获得变量是否已经过期
		return this->deprecated;
	}
	void setDeprecated(bool flag) { // 设置变量是否过期
		this->deprecated = flag;
	}
	SemanticType getSemanType() { // 获得变量语义类型
		return this->type;
	}
	classInfo* getClassInfo() { // 获得变量语义类型关联的类信息
		return info;
	}
	/***************************************
	* 函数名：  printInfo
	* 作用：    打印变量表表项信息
	* 参数：    无
	* 返回值：  void
	****************************************/
	void printInfo();
private:
	std::string name; // 变量名称
	SemanticType type; // 变量类型
	classInfo* info; // 备选项结果
	std::vector<std::string> modifiers; // 修饰符
	int level; // 变量所在层级
	int line; // 变量声明行数
	bool deprecated = false; // 变量是否过期
};

/***************************************
* 类名：    SemanticAnalyser
* 作用：    语义分析器
****************************************/
class SemanticAnalyser {
public:
	/***************************************
	* 函数名：  SemanticAnalyser
	* 作用：    构造器
	* 参数：    root   解析树头节点[in]
	****************************************/
	SemanticAnalyser(Statement* root);
	/***************************************
	* 函数名：  ~SemanticAnalyser
	* 作用：    析构函数
	****************************************/
	~SemanticAnalyser();
	/***************************************
	* 函数名：  semantics
	* 作用：    开始进行语义分析
	* 参数：    无
	* 返回值：  void
	****************************************/
	void semantics();
	/***************************************
	* 函数名：  printFuncInfo
	* 作用：    打印函数表
	* 参数：    无
	* 返回值：  void
	****************************************/
	void printFuncInfo();
	/***************************************
	* 函数名：  printVarInfo
	* 作用：    打印变量表
	* 参数：    无
	* 返回值：  void
	****************************************/
	void printVarInfo();
private:
	Statement* parseTreeRoot;  // 解析树根节点
	int level;   // 当前分析层级
	/***************************************
	* 函数名：  analyserNode
	* 作用：    自下而上对某一个结点进行语义检查
	* 参数：    root         结点的指针[in]
	* 返回值：  void
	****************************************/
	void analyserNode(Statement *root);
	std::vector<funcTableItem*> functionTable; // 函数表
	std::vector<varTableItem*> variableTable; // 符号表
	/***************************************
	* 函数名：  registerFunc
	* 作用：    对于函数表中的表项进行注册
	* 参数：    funcNode  解析树函数定义结点的指针[in]
	* 返回值：  void
	****************************************/
	void registerFunc(FuncDefineStatement* funcNode);
	/***************************************
	* 函数名：  registerVar
	* 作用：    对于变量表中的表项进行注册
	* 参数：    varDeclNode  解析树变量声明结点的指针[in]
	* 返回值：  void
	****************************************/
	void registerVar(VarDeclarationStatement* varDeclNode);
	/***************************************
	* 函数名：  registerVar
	* 作用：    对于变量表中的表项进行注册
	* 参数：    name        变量名        [in]
	*           type        变量的语法类型[in]
	*           line        变量声明的函数[in]
	* 返回值：  void
	****************************************/
	void registerVar(std::string name,std::string type,int line);
	/***************************************
	* 函数名：  parseType
	* 作用：    将语法类型解析为语义类型
	* 参数：    proto       原始语法类型          [in]
	*           type        语义类型输出句柄      [in/out]
	*           info        语义类型关联类信息句柄[in/out]
	* 返回值：  void
	****************************************/
	void parseType(std::string proto,SemanticType &type,classInfo* &info);
	/***************************************
	* 函数名：  judgeIDType
	* 作用：    判断IDNode所对应的语义类型
	* 参数：    idNode     IDNode结点的指针[in]
	*           type       语义类型输出句柄[in/out]
	*           classInfo  语义类型关联类信息句柄[in/out]
	* 返回值：  void
	****************************************/
	void judgeIDType(IdNodeStatement* idNode,SemanticType &type,classInfo *&info);
	/***************************************
	* 函数名：  judgeFunctionType
	* 作用：    解析FunctionNode所对应的语义类型
	* 参数：    root       FunctionNode结点的指针[in]
	*           type       语义类型输出句柄[in/out]
	*           classInfo  语义类型关联类信息句柄[in/out]
	* 返回值：  void
	****************************************/
	void judgeFunctionType(FunctionStatement* root, SemanticType& type, classInfo*& info);
	/***************************************
	* 函数名：  operate
	* 作用：    解析OpNodeStatement所对应的语义类型
	* 参数：    opNode       FunctionNode结点的指针[in]
	*           type         语义类型输出句柄[in/out]
	*           classInfo    语义类型关联类信息句柄[in/out]
	* 返回值：  void
	****************************************/
	void operate(OpNodeStatement* opNode, SemanticType &type,classInfo *&info);
	/***************************************
	* 函数名：  checkOperation
	* 作用：    解析运算符类型计算的结果类型
	* 参数：    OpType        运算符类型                      [in]
	*           OpDir         运算符方向                      [in]
	*           typeOp1       第一操作数的语义类型            [in]
	*           classInfo1    第一操作数的类型关联的类信息    [in]
	*           typeOp2       第二操作数的语义类型            [in]
	*           classInfo2    第二操作数的类型关联的类信息    [in]
	*           resType       结果语义类型                    [in/out]
	*           resInfo       结果语义类型关联的类信息        [in/out]
	*           line          起始行数                        [in]
	*           castSemanType 强制转型结果语义类型            [in]
	*           castInfo      强制转型结果语义类型关联的类信息[in]
	* 返回值：  void
	****************************************/
	void checkOperation(std::string OpType, std::string OpDir,SemanticType &typeOp1,classInfo* &classInfo1,SemanticType &typeOp2, classInfo *&classInfo2, SemanticType &resType,classInfo* &resInfo,int line, SemanticType& castSemanType, classInfo*& castInfo);
	/***************************************
	* 函数名：  checkJudge
	* 作用：    检查分支是否合法
	* 参数：    judge        分支结点指针[in]
	* 返回值：  void
	****************************************/
	void checkJudge(branchStatement* judge);
	/***************************************
	* 函数名：  checkReturn
	* 作用：    检return语句是否与函数定义匹配
	* 参数：    judge        分支结点指针[in]
	* 返回值：  void
	****************************************/
	void checkReturn(returnStatement* root);
	/***************************************
	* 函数名：  checkReturnComplement
	* 作用：    函数定义结点是否存在return
	* 参数：    root        函数定义结点指针[in]
	* 返回值：  void
	****************************************/
	void checkReturnComplement(FuncDefineStatement* root);
};
#endif
