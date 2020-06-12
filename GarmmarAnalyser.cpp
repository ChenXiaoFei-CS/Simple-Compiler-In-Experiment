/*********************************************************
*  模块名：    GrammarAnalyser
*  文件名：    GrammarAnalyser.cpp
*  依赖文件：  Base.h, parseTree.h
*  作 用：     对于C/C++语言进行语法分析
*  作 者：     陈骁飞
*  时 间：     2020.6.2
*  版 本：     Version 1.1
*********************************************************/
#include "GrammarAnalyser.h"
#include "Base.h"
#include "parseTree.h"
#include<iostream>
using namespace std;
GrammarAnalyser::GrammarAnalyser(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> lexRes, std::vector<std::pair<int, int>> spaceRecord) {
	this->lexRes = lexRes;
	this->spaceRecord = spaceRecord;
	init();
}
GrammarAnalyser::~GrammarAnalyser() {}
/***************************************
* 函数名：  init
* 作用：    进行语法分析时，分批额初始数据结构
* 参数：    无
* 返回值：  无
****************************************/
void GrammarAnalyser::init() {
	root = new RootStatement();
	curStatement = root;
}
/***************************************
* 函数名：  grammar
* 作用：    开始进行语法分析
* 参数：    无
* 返回值：  无
****************************************/
void GrammarAnalyser::grammar() {
	cout << "进行语法分析 ！！" << endl;
	while (cur < lexRes.size()) {
		parseComponent();
	}
}
/***************************************
* 函数名：  handleUsing
* 作用：    处理using关键字开始语句
* 参数：    无
* 返回值：  无
****************************************/
void GrammarAnalyser::handleUsing() {
	string type1 = lexRes[++cur].second.second;
	string type2 = lexRes[++cur].second.second;
	cur++;
}
/***************************************
* 函数名：  handleFunc
* 作用：    处理函数定义语句
* 参数：    无
* 返回值：  无
****************************************/
void GrammarAnalyser::handleFunc(std::vector<std::string> modifiers,std::string type,std::string name,int startLine) {
	std::vector<std::pair<std::string, std::string>> argTable;
	extractArgTable(argTable);
	if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second=="{") {
		Statement* func = new FuncDefineStatement(name, type, modifiers, startLine);
		static_cast<FuncDefineStatement*>(func)->setArgTable(argTable);
		curStatement->addChild(func);
		curStatement = func;
		cur++;
	}
	else if(lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == ";"){
		cout << "函数声明" << endl;
		cur++;
	}
	else {
		cout << "函数无结束标记" << endl;
		abort();
	}
}
/***************************************
* 函数名：  handleVarDeclaration
* 作用：    处理变量声明语句
* 参数：    无
* 返回值：  无
****************************************/
void GrammarAnalyser::handleVarDeclaration(std::vector<std::string> modifiers, std::string type, std::string name, int startLine) {
	if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == ";") {
		Statement* func = new VarDeclarationStatement(name, type, modifiers, startLine);
		curStatement->addChild(func);
		cur++;
	}
	else {
		Statement* func = new VarDeclarationStatement(name, type, modifiers, startLine);
		curStatement->addChild(func);
		cur--;
		std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> resTable;
		std::vector<int> commaedPos;
		split(resTable,commaedPos,";",",");
		for (int i = 0; i < commaedPos.size();i++) {
			int cursor = commaedPos[i];
			int ends = (i == commaedPos.size() - 1) ? resTable.size() : commaedPos[i + 1];
			if (i > 0) {
				if (isModifier(resTable[commaedPos[i]].second.second)) {
					modifiers.clear();
					while (isModifier(resTable[cursor].second.second)) {
						cursor++;
					}
				}
				if (isType(resTable[commaedPos[i]].second.first, resTable[commaedPos[i]].second.second)) {
					type = "";
					extractType(type, resTable, cursor);
				}
				name = resTable[cursor].second.second;
				Statement* func = new VarDeclarationStatement(name, type, modifiers, startLine);
				curStatement->addChild(func);
			}
			Statement* sentence;
			handleNode(resTable,cursor,ends, sentence);
			curStatement->addChild(sentence);
		}
	}
}
/***************************************
* 函数名：  handleNode
* 作用：    自下而上处理表达式与函数调用混合语句
* 参数：    无
* 返回值：  无
****************************************/
void GrammarAnalyser::handleNode(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& bin, int startPos, int endPos, Statement*& handle) {
	judgeInvalidExpression(bin,startPos,endPos); // 进行一系列检查
	// 消除多余括号（最外层）
	if (bin[startPos].second.first == LEX_TYPE::SEPERATOR && bin[startPos].second.second == "(") {
		int leftBra = 1, cursor = startPos + 1;
		for (;cursor<endPos;cursor++) {
			if (bin[cursor].second.first == LEX_TYPE::SEPERATOR && bin[cursor].second.second == "(") {
				leftBra++;
			}
			if (bin[cursor].second.first == LEX_TYPE::SEPERATOR && bin[cursor].second.second == ")") {
				leftBra--;
			}
			if (leftBra==0) {
				break;
			}
		}
		if (cursor==endPos-1) {
			startPos++;
			endPos--;
		}
	}
	// 搜索最惰性运算符
	int splitPos = -1, castPosStart = -1, castPosEnd = -1;
	std::string splitDir = "", CastType="";
	searchForLeastPriOp(bin,startPos,endPos, splitPos, splitDir); // 搜索分裂运算符
	searchForCast(bin, startPos, endPos, castPosStart, castPosEnd, CastType);
	if (splitPos!=-1) {
		Statement* op = new OpNodeStatement(bin[splitPos].second.second, splitDir, bin[splitPos].first);
		handle = op;
		if (OpTable[bin[splitPos].second.second+splitDir]==0) {
			cout << "解析结点中运算符出现错误" << endl;
			abort();
		}
		else if (OpTable[bin[splitPos].second.second + splitDir] == 1 && castPosEnd!=-1) { // 优先级低于强制转型
			Statement* st;
			handleNode(bin,castPosEnd+1,endPos,st);
			op->addChild(st);
		}
		else {
			if (splitDir=="M") {
				if (bin[splitPos].second.second == "[") { // 处理数组
					Statement* stL, * stR;
					handleNode(bin, startPos, splitPos,stL);
					handleNode(bin, splitPos + 1, endPos-1, stR);
					op->addChild(stL);
					op->addChild(stR);
				}
				else { // 非数组的运算符
					Statement* stL, * stR;
					handleNode(bin, startPos, splitPos,stL);
					handleNode(bin, splitPos + 1, endPos,stR);
					op->addChild(stL);
					op->addChild(stR);
				}
			}
			else if (splitDir == "L") {
				Statement* stL;
				handleNode(bin, startPos, splitPos,stL);
				op->addChild(stL);
			}
			else if (splitDir == "R") {
				Statement* stR;
				handleNode(bin, splitPos+1, endPos,stR);
				op->addChild(stR);
			}
			else {
				cout << "结点解析错误" << endl;
				abort();
			}
		}
	}
	else if (castPosEnd!=-1) {
		Statement* cast = new OpNodeStatement("Cast","R",bin[castPosStart].first,CastType);
		handle = cast;
		Statement* stR;
		handleNode(bin,castPosEnd+1,endPos,stR);
		cast->addChild(stR);
	}
	else { // 处理单个结点
		if (endPos-startPos==1) { // 常量，标识符结点
			string name = bin[startPos].second.second;
			LEX_TYPE lex = bin[startPos].second.first;
			Statement* id = new IdNodeStatement(name,lex,bin[startPos].first);
			handle = id;
		}
		else { // 函数结点
			string funcName = bin[startPos].second.second;
			int cursor = startPos + 1;
			Statement* func = new FunctionStatement(funcName,bin[startPos].first);
			handle = func;
			Statement* arg;
			if (bin[cursor].second.first == LEX_TYPE::SEPERATOR && bin[cursor].second.second == "(") {
				cursor++;
				std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> tmp;
				std::vector<int> comma;
				split(bin,tmp,comma,")",",",cursor);
				for (int i = 0; i < comma.size();i++) {
					int end = (i == comma.size() - 1) ? tmp.size() : comma[i+1];
					handleNode(tmp,comma[i],end,arg);
					((FunctionStatement*)func)->addArg(arg);
				}
			}
			else {
				cout << "解析结点出错，误入函数调用处理逻辑" << endl;
				abort();
			}
		}
	}
}
/***************************************
* 函数名：  handleEndsWithBracket
* 作用：    处理右大括号界符
* 参数：    无
* 返回值：  无
****************************************/
void GrammarAnalyser::handleEndsWithBracket() {
	int endLine = lexRes[cur].first;
	if (curStatement->getInstanceName()=="FuncDefineStatement") {
		static_cast<FuncDefineStatement*>(curStatement)->setEndLine(endLine);
	}
	else if (curStatement->getInstanceName() == "branchStatement") {
		static_cast<branchStatement*>(curStatement)->setEndLine(endLine);
	}
	
	if (this->curStatement->getParent()!=NULL) {
		this->curStatement = this->curStatement->getParent();
	}
	else {
		cout << "过多的大括号" << endl;
		abort();
	}
	cur++;
}
/***************************************
* 函数名：  handleIf
* 作用：    处理if分支
* 参数：    无
* 返回值：  无
****************************************/
void GrammarAnalyser::handleIf() {
	int line = lexRes[cur].first;
	Statement* branch = new branchStatement("if", line);
	cur++;
	if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == "(") {
		std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> resTable;
		extractJudgement(resTable);
		Statement* judgement;
		handleNode(resTable,0,resTable.size(),judgement);
		static_cast<branchStatement*>(branch)->judgement = judgement;
		curStatement->addChild(branch);
		curStatement = branch;
	}
	else {
		cout << "If 循环没有结尾" << endl;
		abort();
	}
	if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == "{") {
		static_cast<branchStatement*>(branch)->setHasBrack(true);
		cur++;
	}
	else {
		static_cast<branchStatement*>(branch)->setHasBrack(false);
	}
}
/***************************************
* 函数名：  handleElse
* 作用：    处理else 以及 else if分支
* 参数：    无
* 返回值：  无
****************************************/
void GrammarAnalyser::handleElse() {
	int line = lexRes[cur].first;
	cur++;
	if (lexRes[cur].second.first == LEX_TYPE::KEYWORD && lexRes[cur].second.second == "if") {
		Statement* branch = new branchStatement("else if", line);
		cur++;
		if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == "(") {
			std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> resTable;
			extractJudgement(resTable);
			Statement* judgement;
			handleNode(resTable, 0, resTable.size(),judgement);
			static_cast<branchStatement*>(branch)->judgement = judgement;
			curStatement->addChild(branch);
			curStatement = branch;
		}
		else {
			cout << "If 循环没有结尾" << endl;
			abort();
		}
		if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == "{") {
			static_cast<branchStatement*>(branch)->setHasBrack(true);
			cur++;
		}
		else {
			static_cast<branchStatement*>(branch)->setHasBrack(false);
		}
	}
	else if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == "{") {
		int line = lexRes[cur].first;
		Statement* branch = new branchStatement("else", line);
		curStatement->addChild(branch);
		curStatement = branch;
		static_cast<branchStatement*>(branch)->setHasBrack(true);
		cur++;
	}
	else {
		int line = lexRes[cur].first;
		Statement* branch = new branchStatement("else", line);
		curStatement->addChild(branch);
		curStatement = branch;
		static_cast<branchStatement*>(branch)->setHasBrack(false);
	}
}
/***************************************
* 函数名：  handleWhile
* 作用：    处理while循环
* 参数：    无
* 返回值：  无
****************************************/
void GrammarAnalyser::handleWhile() {
	int line = lexRes[cur].first;
	Statement* branch = new branchStatement("while", line);
	cur++;
	if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == "(") {
		std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> resTable;
		extractJudgement(resTable);
		Statement* judgement;
		handleNode(resTable, 0, resTable.size(),judgement);
		static_cast<branchStatement*>(branch)->judgement = judgement;
		curStatement->addChild(branch);
		curStatement = branch;
	}
	else {
		cout << "While 循环没有结尾" << endl;
		abort();
	}
	if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == "{") {
		static_cast<branchStatement*>(branch)->setHasBrack(true);
		cur++;
	}
	else {
		static_cast<branchStatement*>(branch)->setHasBrack(false);
	}
}
/***************************************
* 函数名：  handleFor
* 作用：    处理for循环
* 参数：    无
* 返回值：  无
****************************************/
void GrammarAnalyser::handleFor() {
	int line = lexRes[cur].first;
	Statement* branch = new branchStatement("for", line);
	cur++;
	if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == "(") {
		curStatement->addChild(branch);
		curStatement = branch;
		std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> resTable;
		extractJudgement(resTable);
		int timesL1 = 0, timesL2 = 0;
		for (int i = 0; i < resTable.size();i++) {
			if (resTable[i].second.first == LEX_TYPE::SEPERATOR && resTable[i].second.second == ";") {
				timesL1++;
			}
			else if (resTable[i].second.first == LEX_TYPE::SEPERATOR && resTable[i].second.second == ":") {
				timesL2++;
			}
		}
		// 处理普通For循环 for(int i=0,j=1;i<50;i++,j++)
		if (timesL1 == 2 && timesL2 == 0) {
			int start1 = 0, end1 = 0, start2 = 0, end2 = 0, start3 = 3, end3 = 0;
			int cursor = start1;
			while ((resTable[cursor].second.first != LEX_TYPE::SEPERATOR || resTable[cursor].second.second != ";") && cursor <= resTable.size()) {
				cursor++;
			}
			end1 = cursor++;
			start2 = cursor;
			while ((resTable[cursor].second.first != LEX_TYPE::SEPERATOR || resTable[cursor].second.second != ";") && cursor <= resTable.size()) {
				cursor++;
			}
			end2 = cursor++;
			start3 = cursor;
			end3 = resTable.size();
			if (end1 > start1&& isType(resTable[start1].second.first, resTable[start1].second.second)) {
				string type = "";
				cursor = start1;
				vector<string> modifiers;
				while (resTable[cursor].second.first == LEX_TYPE::KEYWORD && isModifier(resTable[cursor].second.second)) {
					modifiers.push_back(resTable[cursor++].second.second);
				}
				extractType(type, resTable, cursor);
				string name = "";
				int line = -1;
				if (resTable[cursor].second.first == LEX_TYPE::IDENTIFIER) {
					line = resTable[cursor].first;
					name = resTable[cursor++].second.second;
				}
				else {
					abort();
					cout << "声明类型之后的一次词不是标识符" << endl;
				}
				if (resTable[cursor].second.first == LEX_TYPE::SEPERATOR && resTable[cursor].second.second == ";") {
					Statement* func = new VarDeclarationStatement(name, type, modifiers, line);
					static_cast<branchStatement*>(branch)->init.push_back(func);
					cursor++;
				}
				else {
					Statement* func = new VarDeclarationStatement(name, type, modifiers, line);
					static_cast<branchStatement*>(branch)->init.push_back(func);
					cursor--;
					std::vector<int> commaedPos;
					int startt = cursor;
					for (int j = cursor; j < end1; j++) {
						if (resTable[j].second.first == LEX_TYPE::SEPERATOR && resTable[j].second.second == ",") {
							Statement* initSent;
							handleNode(resTable, startt, j, initSent);
							static_cast<branchStatement*>(branch)->init.push_back(initSent);
							startt = j + 1;
						}
					}
					Statement* initSent;
					handleNode(resTable, startt, end1,initSent);
					static_cast<branchStatement*>(branch)->init.push_back(initSent);
				}
			}
			else if (end1 > start1) {
				Statement* initSent;
				handleNode(resTable, start1, end1,initSent);
				static_cast<branchStatement*>(branch)->init.push_back(initSent);
			}
			if (end2 > start2&& isType(resTable[start2].second.first, resTable[start2].second.second)) {
				cout << "for循环的继续条件不能为声明语句" << endl;
				abort();
			}
			else if (end2 > start2) {
				Statement* endSent;
				handleNode(resTable, start2, end2, endSent);
				static_cast<branchStatement*>(branch)->end = endSent;
			}
			if (end3 > start3&& isType(resTable[start3].second.first, resTable[start3].second.second)) {
				string type = "";
				cursor = start3;
				vector<string> modifiers;
				while (resTable[cursor].second.first == LEX_TYPE::KEYWORD && isModifier(resTable[cursor].second.second)) {
					modifiers.push_back(resTable[cursor++].second.second);
				}
				extractType(type, resTable, cursor);
				string name = "";
				int line = -1;
				if (resTable[cursor].second.first == LEX_TYPE::IDENTIFIER) {
					line = resTable[cursor].first;
					name = resTable[cursor++].second.second;
				}
				else {
					abort();
					cout << "声明类型之后的一次词不是标识符" << endl;
				}
				if (resTable[cursor].second.first == LEX_TYPE::SEPERATOR && resTable[cursor].second.second == ";") {
					Statement* func = new VarDeclarationStatement(name, type, modifiers, line);
					static_cast<branchStatement*>(branch)->init.push_back(func);
					cursor++;
				}
				else {
					Statement* func = new VarDeclarationStatement(name, type, modifiers, line);
					static_cast<branchStatement*>(branch)->init.push_back(func);
					cursor--;
					std::vector<int> commaedPos;
					int startt = cursor;
					for (int j = cursor; j < end3; j++) {
						if (resTable[j].second.first == LEX_TYPE::SEPERATOR && resTable[j].second.second == ",") {
							Statement* iterSent;
							handleNode(resTable, startt, j, iterSent);
							static_cast<branchStatement*>(branch)->iterate.push_back(iterSent);
							startt = j + 1;
						}
					}
					Statement* iterSent;
					handleNode(resTable, startt, end3, iterSent);
					static_cast<branchStatement*>(branch)->iterate.push_back(iterSent);
				}
			}
			else if (end3 > start3) {
				Statement* iterSent;
				handleNode(resTable, start3, end3,iterSent);
				static_cast<branchStatement*>(branch)->iterate.push_back(iterSent);
			}
		}
		// 处理增强for循环 for(auto i : list)
		else if (timesL1 == 0 && timesL2 == 1) {
		    int cursor = -1;
			for (int i = 0; i < resTable.size(); i++) {
				if (resTable[i].second.first == LEX_TYPE::SEPERATOR && resTable[i].second.second == ":") {
					cursor = i;
				}
			}
			if (isType(resTable[0].second.first,resTable[0].second.second)) {
				string type = "";
				int cursor1 = 0;
				vector<string> modifiers;
				while (resTable[cursor1].second.first == LEX_TYPE::KEYWORD && isModifier(resTable[cursor1].second.second)) {
					modifiers.push_back(resTable[cursor1++].second.second);
				}
				extractType(type, resTable, cursor1);
				string name = "";
				int line = -1;
				if (resTable[cursor1].second.first == LEX_TYPE::IDENTIFIER) {
					line = resTable[cursor1].first;
					name = resTable[cursor1].second.second;
				}
				else {
					abort();
					cout << "声明类型之后的一次词不是标识符" << endl;
				}
				Statement* func = new VarDeclarationStatement(name, type, modifiers, line);
				static_cast<branchStatement*>(branch)->forEachDeclare.push_back(func);
			}
			else {
				cout << "ForEach循环格式错误" << endl;
			}
			Statement* forEachSent;
			handleNode(resTable, cursor + 1, resTable.size(),forEachSent);
			static_cast<branchStatement*>(branch)->forEachDeclare.push_back(forEachSent);
		}
		else {
			cout << "For循环格式错误" << endl;
			abort();
		}
	}
	else {
		cout << "For 循环没有结尾" << endl;
		abort();
	}
	if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == "{") {
		static_cast<branchStatement*>(branch)->setHasBrack(true);
		cur++;
	}
	else {
		static_cast<branchStatement*>(branch)->setHasBrack(false);
	}
}
/***************************************
* 函数名：  handleAssign
* 作用：    处理普通表达式入口
*           （内部还有一次对于逗号的切割）
* 参数：    无
* 返回值：  无
****************************************/
void GrammarAnalyser::handleAssign() {
	std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> resTable;
	std::vector<int> commaedPos;
	split(resTable, commaedPos, ";", ",");
	for (int i = 0; i < commaedPos.size(); i++) {
		int ends = (i == commaedPos.size() - 1) ? resTable.size() : commaedPos[i + 1];
		Statement* sent;
		handleNode(resTable, commaedPos[i], ends,sent);
		curStatement->addChild(sent);
	}
}
/***************************************
* 函数名：  handleSwitch
* 作用：    处理switch分支
* 参数：    无
* 返回值：  无
****************************************/
void GrammarAnalyser::handleSwitch() {
	int line = lexRes[cur].first;
	Statement* branch = new branchStatement("switch", line);
	cur++;
	if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == "(") {
		std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> resTable;
		extractJudgement(resTable);
		Statement* judgement;
		handleNode(resTable, 0, resTable.size(),judgement);
		static_cast<branchStatement*>(branch)->judgement = judgement;
		curStatement->addChild(branch);
		curStatement = branch;
	}
	else {
		cout << "Switch 循环没有结尾" << endl;
		abort();
	}
	if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == "{") {
		static_cast<branchStatement*>(branch)->setHasBrack(true);
		cur++;
	}
	else {
		static_cast<branchStatement*>(branch)->setHasBrack(false);
	}
}
/***************************************
* 函数名：  handleCase
* 作用：    处理case分支
* 参数：    无
* 返回值：  无
****************************************/
void GrammarAnalyser::handleCase() {
	int line = lexRes[cur].first;
	Statement* branch = new branchStatement("case", line);
	static_cast<branchStatement*>(branch)->setHasBrack(false);
	cur++;
	std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> m;
	m.push_back(lexRes[cur++]);
	if (lexRes[cur].second.first == LEX_TYPE::OPERATOR && lexRes[cur].second.second == "::") {
		m.push_back(lexRes[cur++]);
		m.push_back(lexRes[cur++]);
	}
	Statement* judgement;
	handleNode(m, 0, m.size(),judgement);
	static_cast<branchStatement*>(branch)->judgement = judgement;
	if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == ":") {
		curStatement->addChild(branch);
		curStatement = branch;
		cur++;
	}
	else {
		cout << "Case的格式错误:后面只能跟随一个常量" << endl;
		abort();
	}
}
/***************************************
* 函数名：  handleBreak
* 作用：    处理break语句
*           （内部还有一次对于逗号的切割）
* 参数：    无
* 返回值：  无
****************************************/
void GrammarAnalyser::handleBreak() {
	int line = lexRes[cur].first;
	Statement* breakst = new breakStatement(line);
	curStatement->addChild(breakst);
	cur++;
}
/***************************************
* 函数名：  handleDefault
* 作用：    处理default分支
* 参数：    无
* 返回值：  无
****************************************/
void GrammarAnalyser::handleDefault() {
	int line = lexRes[cur].first;
	Statement* branch = new branchStatement("default", line);
	static_cast<branchStatement*>(branch)->setHasBrack(false);
	cur++;
	if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == ":") {
		curStatement->addChild(branch);
		curStatement = branch;
		cur++;
	}
	else {
		cout << "Default的格式错误:无冒号" << endl;
		abort();
	}
}
/***************************************
* 函数名：  handleReturn
* 作用：    处理return语句
* 参数：    无
* 返回值：  无
****************************************/
void GrammarAnalyser::handleReturn() {
	int line = lexRes[cur++].first;
	Statement* st = new returnStatement(line);
	curStatement->addChild(st);
	std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> resTable;
	split(resTable,";");
	if (resTable.size()>0) {
		Statement* sent;
		handleNode(resTable, 0, resTable.size(),sent);
		st->addChild(sent);
	}
}
/***************************************
* 函数名：  handleIndentation
* 作用：    对缩进进行检查
* 参数：    无
* 返回值：  无
****************************************/
void GrammarAnalyser::handleIndentation() {
	if (curStatement->getInstanceName() == "branchStatement") {
		if (static_cast<branchStatement*>(curStatement)->getHasBrack()==false) {
			int branchLine =  static_cast<branchStatement*>(curStatement)->getStartLine();
			int branchIndentation = searchForSpace(spaceRecord, branchLine);
			int curLine = lexRes[cur].first;
			int curIndentation = searchForSpace(spaceRecord, curLine);
			if (curIndentation <= branchIndentation) {
				curStatement = curStatement->getParent();
			}
		}
	}
}

/***************************************
* 函数名：  parseComponent
* 作用：    解析一个语法组件，自顶向下的思想
* 参数：    无
* 返回值：  无
****************************************/
void GrammarAnalyser::parseComponent() {
	vector<string> modifiers;
	handleIndentation();
	while (lexRes[cur].second.first == LEX_TYPE::KEYWORD && isModifier(lexRes[cur].second.second)) {
		modifiers.push_back(lexRes[cur++].second.second);
	}
	if (lexRes[cur].second.first == LEX_TYPE::KEYWORD && lexRes[cur].second.second == "using") {
		handleUsing();
	}
	else if (isType(lexRes[cur].second.first,lexRes[cur].second.second)) {
		string type = "";
		extractType(type);
		string name = "";
		int line = -1;
		if (lexRes[cur].second.first==LEX_TYPE::IDENTIFIER) {
			line = lexRes[cur].first;
			name = lexRes[cur++].second.second;
		}
		else {
			cout << "声明类型之后的一次词不是标识符" << endl;
			abort();
		}
		if (lexRes[cur].second.first==LEX_TYPE::SEPERATOR && lexRes[cur].second.second == "(") {
			handleFunc(modifiers,type,name,line);
		}
		else {
			handleVarDeclaration(modifiers, type, name, line);
		}
	}
	else if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == "}") {
		handleEndsWithBracket();
	}
	else if (lexRes[cur].second.first == LEX_TYPE::KEYWORD && lexRes[cur].second.second == "if") {
		handleIf();
	}
	else if (lexRes[cur].second.first == LEX_TYPE::KEYWORD && lexRes[cur].second.second == "else") {
		handleElse();
	}
	else if (lexRes[cur].second.first == LEX_TYPE::KEYWORD && lexRes[cur].second.second == "while") {
		handleWhile();
	}
	else if (lexRes[cur].second.first == LEX_TYPE::KEYWORD && lexRes[cur].second.second == "for") {
		handleFor();
	}
	else if (lexRes[cur].second.first == LEX_TYPE::KEYWORD && lexRes[cur].second.second == "switch") {
		handleSwitch();
	}
	else if (lexRes[cur].second.first == LEX_TYPE::KEYWORD && lexRes[cur].second.second == "case") {
		handleCase();
	}
	else if (lexRes[cur].second.first == LEX_TYPE::KEYWORD && lexRes[cur].second.second == "break") {
		handleBreak();
	}
	else if (lexRes[cur].second.first == LEX_TYPE::KEYWORD && lexRes[cur].second.second == "default") {
		handleDefault();
	}
	else if (lexRes[cur].second.first == LEX_TYPE::IDENTIFIER || lexRes[cur].second.first == LEX_TYPE::CONST || lexRes[cur].second.first == LEX_TYPE::OPERATOR || (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == "(")) {
		handleAssign();
	}
	else if (lexRes[cur].second.first == LEX_TYPE::KEYWORD && lexRes[cur].second.second == "return") {
		handleReturn();
	}
	else {
		cout << "Others "  << lexRes[cur].second.second << endl;
	}
	if (cur<lexRes.size() && lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == ";") {
		cur++;
	}
}
/***************************************
* 函数名：  printParseTree
* 作用：    打印语法解析树
* 参数：    无
* 返回值：  无
****************************************/
void GrammarAnalyser::printParseTree() {
	cout << "以下打印语法解析树：" << endl;
	root->getInfo(0);
}
/***************************************
* 函数名：  getParseTree
* 作用：    获取解析树根节点语法解析树
* 参数：    root        [in/out]
* 返回值：  无
****************************************/
void GrammarAnalyser::getParseTree(Statement *&root) {
	root = this->root;
}
/***************************************
* 函数名：  extractType
* 作用：    从lexRes中解析当前指针指向的类型
* 参数：    type       类型输出句柄[in/out]
* 返回值：  无
****************************************/
void GrammarAnalyser::extractType(string& type) {
	type += lexRes[cur++].second.second;
	if (hasTemplate(type) && lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == "<") {
		type += lexRes[cur++].second.second;
		int leftBra = 1;
		while (leftBra > 0 && lexRes[cur].second.second != ";") {
			if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == "<") {
				leftBra++;
			}
			if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == ">") {
				leftBra--;
			}
			type += lexRes[cur++].second.second;
		}
	}
	while (lexRes[cur].second.first==LEX_TYPE::OPERATOR && lexRes[cur].second.second == "*") {
		type += lexRes[cur++].second.second;
	}
}
/***************************************
* 函数名：  extractType
* 作用：    从lexRes中解析当前指针指向的类型
* 参数：    type       类型输出句柄[in/out]
*           tmpResBin  token容器   [in]
*           cursor     token指针   [in/out]
* 返回值：  无
****************************************/
void GrammarAnalyser::extractType(std::string& type, std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& tmpResBin, int &cursor) {
	type += tmpResBin[cursor++].second.second;
	if (hasTemplate(type) && tmpResBin[cursor].second.first == LEX_TYPE::SEPERATOR && tmpResBin[cursor].second.second == "<") {
		type += tmpResBin[cursor++].second.second;
		int leftBra = 1;
		while (leftBra > 0 && tmpResBin[cursor].second.second != ";") {
			if (tmpResBin[cursor].second.first == LEX_TYPE::SEPERATOR && tmpResBin[cursor].second.second == "<") {
				leftBra++;
			}
			if (tmpResBin[cursor].second.first == LEX_TYPE::SEPERATOR && tmpResBin[cursor].second.second == ">") {
				leftBra--;
			}
			type += tmpResBin[cursor++].second.second;
		}
	}
	while (tmpResBin[cursor].second.first == LEX_TYPE::OPERATOR && tmpResBin[cursor].second.second == "*") {
		type += tmpResBin[cursor++].second.second;
	}
}
/***************************************
* 函数名：  extractArgTable
* 作用：    从lexRes中解析函数定义时的参数列表
* 参数：    argTable   参数列表输出句柄[in/out]
* 返回值：  无
****************************************/
void GrammarAnalyser::extractArgTable(std::vector<std::pair<std::string, std::string>>& argTable) {
	cur++;
	while (isType(lexRes[cur].second.first,lexRes[cur].second.second)) {
		string type = "";
		extractType(type);
		string name = lexRes[cur++].second.second;
		argTable.push_back(pair<std::string,std::string>(type,name));
		if (lexRes[cur].second.first==LEX_TYPE::SEPERATOR && lexRes[cur].second.second == ",") {
			cur++;
		}
	}
	cur++;
}
/***************************************
* 函数名：  extractJudgement
* 作用：    从lexRes中分离出分支语句判断表达式
* 参数：    tmpResBin  判断表达式输出句柄[in/out]
* 返回值：  无
****************************************/
void GrammarAnalyser::extractJudgement(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& tmpResBin) {
	int leftBra = 1;
	cur++;
	while (leftBra>0 && cur<lexRes.size()) {
		if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second=="(") {
			leftBra++;
		}
		if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == ")") {
			leftBra--;
		}
		tmpResBin.push_back(lexRes[cur++]);
	}
	if (tmpResBin.size()>0) {
		tmpResBin.pop_back();
	}
	else {
		cout << "程序结尾有一个小括号无配对" << endl;
		abort();
	}
}
/***************************************
* 函数名：  split
* 作用：    从lexRes中按照目标token进行二重语句切割
* 参数：    tmpResBin     语句输出句柄[in/out]
*           commaedPos    内层切割的首元素索引输出句柄[in/out]
*           end           外层切割标记[in]
*           mid           内层切割标记[in]
* 返回值：  无
****************************************/
void GrammarAnalyser::split(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& tmpResBin, std::vector<int>& commaedPos, std::string end, std::string mid) {
	commaedPos.push_back(0);
	int leftbra = 0;
	while (lexRes[cur].second.first!=LEX_TYPE::SEPERATOR || lexRes[cur].second.second != end) {
		if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == "(") {
			leftbra++;
		}
		if (lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == ")") {
			leftbra--;
		}
		if (leftbra==0 &&  lexRes[cur].second.first == LEX_TYPE::SEPERATOR && lexRes[cur].second.second == mid) {
			cur++;
			commaedPos.push_back(tmpResBin.size());
		}
		else {
			tmpResBin.push_back(lexRes[cur++]);
		}
	}
	if (tmpResBin.size()==0) {
		commaedPos.pop_back();
	}
}
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
void GrammarAnalyser::split(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> src, std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& dst, std::vector<int>& commaedPos, std::string end, std::string mid,int &cursor) {
	commaedPos.push_back(0);
	int leftbra = 0;
	while (leftbra !=0 || src[cursor].second.first != LEX_TYPE::SEPERATOR || src[cursor].second.second != end) {
		if (src[cursor].second.first == LEX_TYPE::SEPERATOR && src[cursor].second.second == "(") {
			leftbra++;
		}
		if (src[cursor].second.first == LEX_TYPE::SEPERATOR && src[cursor].second.second == ")") {
			leftbra--;
		}
		if (leftbra == 0 && src[cursor].second.first == LEX_TYPE::SEPERATOR && src[cursor].second.second == mid) {
			cursor++;
			commaedPos.push_back(dst.size());
		}
		else {
			dst.push_back(src[cursor++]);
		}
	}
	if (dst.size() == 0) {
		commaedPos.pop_back();
	}
}
/***************************************
* 函数名：  split
* 作用：    从lexRes中按照目标token进行单重语句切割
* 参数：    tmpResBin     语句输出句柄[in/out]
*           end           切割标记[in]
* 返回值：  无
****************************************/
void GrammarAnalyser::split(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& tmpResBin,std::string end) {
	while (lexRes[cur].second.first != LEX_TYPE::SEPERATOR || lexRes[cur].second.second != end) {
		tmpResBin.push_back(lexRes[cur++]);
	}
}
/***************************************
* 函数名：  judgeInvalidExpression
* 作用：    判断表达式是否合法，若不合法，调用中断
* 参数：    bin:       token容器         [in]
*           startPos:  起始索引位置      [in]
*           endPos:    结束索引位置      [in]
* 返回值：  无
****************************************/
void judgeInvalidExpression(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& bin, int startPos, int endPos) {
	// [1] 检查是否为空解析
	if (startPos>=endPos) {
		cout << "存在空子结点" << endl;
		abort();
	}
	// [2] 检查是否存在非法字符
	bool flag = false;
	for (int cursor = startPos; cursor < endPos;cursor++) {
		if (bin[cursor].second.first==LEX_TYPE::SEPERATOR && bin[cursor].second.second == ";") {
			flag = true;
		}
	}
	if (flag) {
		cout << "存在非法字符" << endl;
		abort();
	}
	// 检查是否存在名词性常量冗余（运算符缺省）
	if (endPos-startPos>=2) {
		for (int cursor = startPos; cursor < endPos; cursor++) {
			if (bin[cursor].second.first == LEX_TYPE::SEPERATOR || bin[cursor].second.first == LEX_TYPE::OPERATOR) {
				flag = true;
			}
		}
		if (flag==false) {
			cout << "存在名词性常量冗余" << endl;
			abort();
		}
	}
	// 检查括号是否匹配
	int leftBra = 0;
	for (int cursor = startPos; cursor < endPos; cursor++) {
		if (bin[cursor].second.first == LEX_TYPE::SEPERATOR && bin[cursor].second.second == "(") {
			leftBra++;
		}
		if (bin[cursor].second.first == LEX_TYPE::SEPERATOR && bin[cursor].second.second == ")") {
			leftBra--;
		}
	}
	if (leftBra!=0) {
		cout << "括号不匹配" << endl;
		abort();
	}
}
/***************************************
* 函数名：  searchForSpace
* 作用：    查找某一行的缩进
* 参数：    spaceTable:  缩进表  [in]
*           line:        行号    [in]
* 返回值：  int
****************************************/
int searchForSpace(std::vector<std::pair<int,int>> spaceTable,int line) {
	for (auto it:spaceTable) {
		if (it.first==line) {
			return it.second;
		}
	}
	return -1;
}
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
void searchForLeastPriOp(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> bin, int startPos, int endPos,int &splitPos,std::string &splitDir) {
	auto isNoun = [](int pos, std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> bin)->bool {
		return (bin[pos].second.first == LEX_TYPE::CONST) || (bin[pos].second.first == LEX_TYPE::IDENTIFIER) || (bin[pos].second.first == LEX_TYPE::KEYWORD);
	};
	auto isLeftBranck = [](int pos, std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> bin)->bool {
		return (bin[pos].second.first == LEX_TYPE::SEPERATOR) && (bin[pos].second.second == "(");
	};
	auto isRightBranck = [](int pos, std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> bin)->bool {
		return (bin[pos].second.first == LEX_TYPE::SEPERATOR) && (bin[pos].second.second == ")");
	};
	auto isRightSquareBranck= [](int pos, std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> bin)->bool {
		return (bin[pos].second.first == LEX_TYPE::SEPERATOR) && (bin[pos].second.second == "]");
	};
	auto isLeftSquareBranck = [](int pos, std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> bin)->bool {
		return (bin[pos].second.first == LEX_TYPE::SEPERATOR) && (bin[pos].second.second == "[");
	};
	auto isColon = [](int pos, std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> bin)->bool {
		return (bin[pos].second.first == LEX_TYPE::SEPERATOR) && (bin[pos].second.second == ":");
	};
	int pos = -1;
	int maxPri = -1;
	std::string dir = "";
	int leftBra = 0;
	for (int cursor = startPos; cursor < endPos;cursor++) {
		if (leftBra==0 && (bin[cursor].second.first == LEX_TYPE::OPERATOR || isLeftSquareBranck(cursor,bin)||isColon(cursor,bin))) {
			if ((cursor >startPos&&(isNoun(cursor-1,bin)||isRightBranck(cursor-1,bin)||isRightSquareBranck(cursor-1,bin)))&& (cursor < endPos-1 && (isNoun(cursor + 1, bin) || isLeftBranck(cursor + 1, bin)))) {
				if (OpTable[bin[cursor].second.second + "M"] > maxPri) {
					maxPri = OpTable[bin[cursor].second.second + "M"];
					pos = cursor;
					dir = "M";
				}
			}
			else if (cursor > startPos && (isNoun(cursor - 1, bin) || isRightBranck(cursor - 1, bin))) {
				if (OpTable[bin[cursor].second.second + "L"] > maxPri) {
					maxPri = OpTable[bin[cursor].second.second + "L"];
					pos = cursor;
					dir = "L";
				}
			}
			else {
				if (OpTable[bin[cursor].second.second + "R"] > maxPri) {
					maxPri = OpTable[bin[cursor].second.second + "R"];
					pos = cursor;
					dir = "R";
				}
			}
		}
		if (isLeftBranck(cursor, bin)|| isLeftSquareBranck(cursor,bin)) {
			leftBra++;
		}
		if (isRightBranck(cursor, bin)||isRightSquareBranck(cursor,bin)) {
			leftBra--;
		}
	}
	splitPos = pos;
	splitDir = dir;
}
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
void GrammarAnalyser::searchForCast(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>bin, int startPos, int endPos, int& castPosStart, int& CastPosEnd, std::string& CastType) {
	auto isLeftBranck = [](int pos, std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> bin)->bool {
		return (bin[pos].second.first == LEX_TYPE::SEPERATOR) && (bin[pos].second.second == "(");
	};
	auto isRightBranck = [](int pos, std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> bin)->bool {
		return (bin[pos].second.first == LEX_TYPE::SEPERATOR) && (bin[pos].second.second == ")");
	};
	auto isRightSquareBranck = [](int pos, std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> bin)->bool {
		return (bin[pos].second.first == LEX_TYPE::SEPERATOR) && (bin[pos].second.second == "]");
	};
	auto isLeftSquareBranck = [](int pos, std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> bin)->bool {
		return (bin[pos].second.first == LEX_TYPE::SEPERATOR) && (bin[pos].second.second == "[");
	};
	int leftBra = 0;
	for (int cursor = startPos; cursor < endPos; cursor++) {
		if (leftBra==0 && isLeftBranck(cursor, bin)) {
			castPosStart = cursor;
			leftBra++;
			cursor++;
			string type = "";
			while (bin[cursor].second.first ==LEX_TYPE::KEYWORD &&isModifier(bin[cursor].second.second)) {
				type += bin[cursor++].second.second;
				type += " ";
			}
			if (isType(bin[cursor].second.first, bin[cursor].second.second)) {
				extractType(type, bin, cursor);
				if (isRightBranck(cursor,bin)) {
					CastPosEnd = (cursor++);
					CastType = type;
					return;
				}
			}
		}
		if (isLeftSquareBranck(cursor, bin)) {
			leftBra++;
		}
		if (isRightBranck(cursor, bin)||isRightSquareBranck(cursor,bin)) {
			leftBra--;
		}
	}
}
/***************************************
* 函数名：  isModifier
* 作用：    判断是否为修饰符类关键词
* 参数：    word：     词语的内容[in]
* 返回值：  bool
****************************************/
bool isModifier(string word) {
	return (word == "inline") || (word == "public") || (word == "static") || (word == "protected") || (word == "private") || (word=="unsigned")||(word=="const");
}
/***************************************
* 函数名：  isProtoType
* 作用：    判断是否是基本类型
* 参数：    word:      词语的内容    [in]
* 返回值：  bool
****************************************/
bool isProtoType(string word) {
	return (word == "void") || (word == "int") || (word == "char") || (word == "bool") || (word == "double") || (word == "float")||(word=="long")||(word=="auto")||(word=="unsigned");
}
/***************************************
* 函数名：  isClassName
* 作用：    判断是否是注册的类名
* 参数：    word:      词语的内容    [in]
* 返回值：  bool
****************************************/
bool isClassName(string word) {
	for (auto it : existClassName) {
		if (it == word) {
			return true;
		}
	}
	return false;
}
/***************************************
* 函数名：  isType
* 作用：    判断是否能够定义类型
* 参数：    lex：      词语的词法类型[in]
*           word:      词语的内容    [in]
* 返回值：  bool
****************************************/
bool isType(LEX_TYPE lex, string word) {
	return ((lex == LEX_TYPE::KEYWORD) && isProtoType(word)) || ((lex == LEX_TYPE::IDENTIFIER) && (isClassName(word)));
}
/***************************************
* 函数名：  hasTemplate
* 作用：    判断类名是否存在泛型（模板）
* 参数：    word:      词语的内容    [in]
* 返回值：  bool
****************************************/
bool hasTemplate(string word) {
	for (auto it : existTemplate) {
		if (it == word) {
			return true;
		}
	}
	return false;
}