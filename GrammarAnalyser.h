/*********************************************************
*  模块名：    GrammarAnalyser
*  文件名：    GrammarAnalyser.h
*  依赖文件：  Base.h, parseTree.h
*  作 用：     对于C/C++语言进行语法分析
*  作 者：     陈骁飞
*  时 间：     2020.6.1
*  版 本：     Version 1.1
*********************************************************/
#ifndef _GRAMMARANALYSER_H_
#define _GRAMMARANALYSER_H_
#include "Base.h"
#include "parseTree.h"
#include <vector>
#include <string>

/***************************************
* 函数名：  isModifier
* 作用：    判断是否为修饰符类关键词
* 参数：    word：     词语的内容[in]
* 返回值：  bool
****************************************/
bool isModifier(std::string word);
/***************************************
* 函数名：  isType
* 作用：    判断是否能够定义类型
* 参数：    lex：      词语的词法类型[in]
*           word:      词语的内容    [in]
* 返回值：  bool
****************************************/
bool isType(LEX_TYPE lex,std::string word);
/***************************************
* 函数名：  isProtoType
* 作用：    判断是否是基本类型
* 参数：    word:      词语的内容    [in]
* 返回值：  bool
****************************************/
bool isProtoType(std::string word);
/***************************************
* 函数名：  isClassName
* 作用：    判断是否是注册的类名
* 参数：    word:      词语的内容    [in]
* 返回值：  bool
****************************************/
bool isClassName(std::string word);
/***************************************
* 函数名：  hasTemplate
* 作用：    判断类名是否存在泛型（模板）
* 参数：    word:      词语的内容    [in]
* 返回值：  bool
****************************************/
bool hasTemplate(std::string word);
/***************************************
* 函数名：  searchForSpace
* 作用：    查找某一行的缩进
* 参数：    spaceTable:  缩进表  [in]
*           line:        行号    [in]
* 返回值：  int
****************************************/
int searchForSpace(std::vector<std::pair<int, int>> spaceTable, int line);
/***************************************
* 函数名：  searchForLeastPriOp
* 作用：    查找一行优先级最低的运算符索引
* 参数：    bin:       token容器         [in]
*           startPos:  起始索引位置      [in]
*           endPos:    结束索引位置      [in]
*           splitPos:  目标运算符所在索引[in/out]
*           splitDir:  目标运算符方向    [in/out]
* (M代表双目，L代表左边有表达式的单目，R代表右边有表达式的单目)
* 返回值：  无
****************************************/
void searchForLeastPriOp(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> bin, int startPos, int endPos,int &splitPos,std::string &splitDir);
/***************************************
* 函数名：  judgeInvalidExpression
* 作用：    判断表达式是否合法，若不合法，调用中断
* 参数：    bin:       token容器         [in]
*           startPos:  起始索引位置      [in]
*           endPos:    结束索引位置      [in]
* 返回值：  无
****************************************/
void judgeInvalidExpression(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& bin, int startPos, int endPos);

/***************************************
* 类名：    GrammarAnalyser
* 作用：    进行语法分析
****************************************/
class GrammarAnalyser
{
public:
	/***************************************
	* 函数名：  GrammarAnalyser
	* 作用：    构造器
	* 参数：    lexRes:     词法分析结果[in]
	*           spaceRecord 每一行的缩进[in]
	****************************************/
	GrammarAnalyser(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> lexRes, std::vector<std::pair<int, int>> spaceRecord);
	/***************************************
	* 函数名：  ~GrammarAnalyser
	* 作用：    析构函数
	****************************************/
	~GrammarAnalyser();
	/***************************************
	* 函数名：  grammar
	* 作用：    开始进行语法分析
	* 参数：    无
	* 返回值：  无
	****************************************/
	void grammar(); 
	/***************************************
	* 函数名：  printParseTree
	* 作用：    打印语法解析树
	* 参数：    无
	* 返回值：  无
	****************************************/
	void printParseTree();
	/***************************************
	* 函数名：  getParseTree
	* 作用：    获取解析树根节点语法解析树
	* 参数：    root        [in/out]
	* 返回值：  无
	****************************************/
	void getParseTree(Statement* &root);
	Statement* root = NULL; // 解析树的根节点
private:
	std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> lexRes; // 词法分析的结果
	std::vector<std::pair<int, int>> spaceRecord;  // 存放缩进表
	/***************************************
	* 函数名：  init
	* 作用：    进行语法分析时，分批额初始数据结构
	* 参数：    无
	* 返回值：  无
	****************************************/
	void init();
	int cur = 0; // token的指针索引
	Statement* curStatement = NULL; // 在构造解析树时，当前结点
	/***************************************
	* 函数名：  parseComponent
	* 作用：    解析一个语法组件，自顶向下的思想
	* 参数：    无
	* 返回值：  无
	****************************************/
	void parseComponent();
	/***************************************
	* 函数名：  extractType
	* 作用：    从lexRes中解析当前指针指向的类型
	* 参数：    type       类型输出句柄[in/out]
	* 返回值：  无
	****************************************/
	void extractType(std::string& type);
	/***************************************
	* 函数名：  extractType
	* 作用：    从lexRes中解析当前指针指向的类型
	* 参数：    type       类型输出句柄[in/out]
	*           tmpResBin  token容器   [in]
	*           cursor     token指针   [in/out]
	* 返回值：  无
	****************************************/
	void extractType(std::string& type, std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& tmpResBin, int& cursor);
	/***************************************
	* 函数名：  extractArgTable
	* 作用：    从lexRes中解析函数定义时的参数列表
	* 参数：    argTable   参数列表输出句柄[in/out]
	* 返回值：  无
	****************************************/
	void extractArgTable(std::vector<std::pair<std::string, std::string>> &argTable);
	/***************************************
	* 函数名：  extractJudgement
	* 作用：    从lexRes中分离出分支语句判断表达式
	* 参数：    tmpResBin  判断表达式输出句柄[in/out]
	* 返回值：  无
	****************************************/
	void extractJudgement(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& tmpResBin);
	/***************************************
	* 函数名：  searchForCast
	* 作用：    查找表达式中存在的强制转型情况
	* 参数：    bin          token容器[in/out]
	*           startPos     起始位置索引[in]
	*           endPos       结束位置索引[in]
	*           castPosStart 强制转型起始位置输出句柄[in/out]
	*           castPosEnd   强制转型结束位置输出句柄[in/out]
	*           CastType     强制转型类型输出句柄[in/out]
	* 返回值：  无
	****************************************/
	void searchForCast(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>bin, int startPos, int endPos, int& castPosStart, int& CastPosEnd, std::string& CastType);
	/***************************************
	* 函数名：  split
	* 作用：    从lexRes中按照目标token进行二重语句切割
	* 参数：    tmpResBin     语句输出句柄[in/out]
	*           commaedPos    内层切割的首元素索引输出句柄[in/out]
	*           end           外层切割标记[in]
	*           mid           内层切割标记[in]
	* 返回值：  无
	****************************************/
	void split(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& tmpResBin, std::vector<int>& commaedPos,std::string end,std::string mid);
	/***************************************
	* 函数名：  split
	* 作用：    从lexRes中按照目标token进行单重语句切割
	* 参数：    tmpResBin     语句输出句柄[in/out]
	*           end           切割标记[in]
	* 返回值：  无
	****************************************/
	void split(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& tmpResBin, std::string end);
	/***************************************
	* 函数名：  split
	* 作用：    在目标容器中按照目标token进行二重语句切割
	* 参数：    src             目标token容器[in]
	*           dst             语句输出句柄[in/out]
	*           commaedPos      内层切割的首元素索引输出句柄[in/out]
	*           end             外层切割标记[in]
	*           mid             内层切割标记[in]
	*           cursor          目标容器起始指针[in/out]
	* 返回值：  无
	****************************************/
	void split(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> src, std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> &dst, std::vector<int>& commaedPos, std::string end, std::string mid,int &cursor);
	/***************************************
	* 函数名：  handleUsing
	* 作用：    处理using关键字开始语句
	* 参数：    无
	* 返回值：  无
	****************************************/
	void handleUsing();
	/***************************************
	* 函数名：  handleFunc
	* 作用：    处理函数定义语句
	* 参数：    无
	* 返回值：  无
	****************************************/
	void handleFunc(std::vector<std::string> modifiers, std::string type, std::string name,int startLine);
	/***************************************
	* 函数名：  handleVarDeclaration
	* 作用：    处理变量声明语句
	* 参数：    无
	* 返回值：  无
	****************************************/
	void handleVarDeclaration(std::vector<std::string> modifiers, std::string type, std::string name, int startLine);
	/***************************************
	* 函数名：  handleNode
	* 作用：    自下而上处理表达式与函数调用混合语句
	* 参数：    无
	* 返回值：  无
	****************************************/
	void handleNode(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& bin,int startPos,int endPos,Statement *&handle);
	/***************************************
	* 函数名：  handleEndsWithBracket
	* 作用：    处理右大括号界符
	* 参数：    无
	* 返回值：  无
	****************************************/
	void handleEndsWithBracket();
	/***************************************
	* 函数名：  handleIf
	* 作用：    处理if分支
	* 参数：    无
	* 返回值：  无
	****************************************/
	void handleIf();
	/***************************************
	* 函数名：  handleElse
	* 作用：    处理else 以及 else if分支
	* 参数：    无
	* 返回值：  无
	****************************************/
	void handleElse();
	/***************************************
	* 函数名：  handleWhile
	* 作用：    处理while循环
	* 参数：    无
	* 返回值：  无
	****************************************/
	void handleWhile();
	/***************************************
	* 函数名：  handleFor
	* 作用：    处理for循环
	* 参数：    无
	* 返回值：  无
	****************************************/
	void handleFor();
	/***************************************
	* 函数名：  handleAssign
	* 作用：    处理普通表达式入口
	*           （内部还有一次对于逗号的切割）
	* 参数：    无
	* 返回值：  无
	****************************************/
	void handleAssign();
	/***************************************
	* 函数名：  handleSwitch
	* 作用：    处理switch分支
	* 参数：    无
	* 返回值：  无
	****************************************/
	void handleSwitch();
	/***************************************
	* 函数名：  handleCase
	* 作用：    处理case分支
	* 参数：    无
	* 返回值：  无
	****************************************/
	void handleCase();
	/***************************************
	* 函数名：  handleBreak
	* 作用：    处理break语句
	*           （内部还有一次对于逗号的切割）
	* 参数：    无
	* 返回值：  无
	****************************************/
	void handleBreak();
	/***************************************
	* 函数名：  handleDefault
	* 作用：    处理default分支
	* 参数：    无
	* 返回值：  无
	****************************************/
	void handleDefault();
	/***************************************
	* 函数名：  handleIndentation
	* 作用：    对缩进进行检查
	* 参数：    无
	* 返回值：  无
	****************************************/
	void handleIndentation();
	/***************************************
	* 函数名：  handleReturn
	* 作用：    处理return语句
	* 参数：    无
	* 返回值：  无
	****************************************/
	void handleReturn();
};
#endif