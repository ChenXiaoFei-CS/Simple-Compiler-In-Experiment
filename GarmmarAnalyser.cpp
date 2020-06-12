/*********************************************************
*  ģ������    GrammarAnalyser
*  �ļ�����    GrammarAnalyser.cpp
*  �����ļ���  Base.h, parseTree.h
*  �� �ã�     ����C/C++���Խ����﷨����
*  �� �ߣ�     �����
*  ʱ �䣺     2020.6.2
*  �� ����     Version 1.1
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
* ��������  init
* ���ã�    �����﷨����ʱ���������ʼ���ݽṹ
* ������    ��
* ����ֵ��  ��
****************************************/
void GrammarAnalyser::init() {
	root = new RootStatement();
	curStatement = root;
}
/***************************************
* ��������  grammar
* ���ã�    ��ʼ�����﷨����
* ������    ��
* ����ֵ��  ��
****************************************/
void GrammarAnalyser::grammar() {
	cout << "�����﷨���� ����" << endl;
	while (cur < lexRes.size()) {
		parseComponent();
	}
}
/***************************************
* ��������  handleUsing
* ���ã�    ����using�ؼ��ֿ�ʼ���
* ������    ��
* ����ֵ��  ��
****************************************/
void GrammarAnalyser::handleUsing() {
	string type1 = lexRes[++cur].second.second;
	string type2 = lexRes[++cur].second.second;
	cur++;
}
/***************************************
* ��������  handleFunc
* ���ã�    �������������
* ������    ��
* ����ֵ��  ��
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
		cout << "��������" << endl;
		cur++;
	}
	else {
		cout << "�����޽������" << endl;
		abort();
	}
}
/***************************************
* ��������  handleVarDeclaration
* ���ã�    ��������������
* ������    ��
* ����ֵ��  ��
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
* ��������  handleNode
* ���ã�    ���¶��ϴ�����ʽ�뺯�����û�����
* ������    ��
* ����ֵ��  ��
****************************************/
void GrammarAnalyser::handleNode(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& bin, int startPos, int endPos, Statement*& handle) {
	judgeInvalidExpression(bin,startPos,endPos); // ����һϵ�м��
	// �����������ţ�����㣩
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
	// ��������������
	int splitPos = -1, castPosStart = -1, castPosEnd = -1;
	std::string splitDir = "", CastType="";
	searchForLeastPriOp(bin,startPos,endPos, splitPos, splitDir); // �������������
	searchForCast(bin, startPos, endPos, castPosStart, castPosEnd, CastType);
	if (splitPos!=-1) {
		Statement* op = new OpNodeStatement(bin[splitPos].second.second, splitDir, bin[splitPos].first);
		handle = op;
		if (OpTable[bin[splitPos].second.second+splitDir]==0) {
			cout << "�����������������ִ���" << endl;
			abort();
		}
		else if (OpTable[bin[splitPos].second.second + splitDir] == 1 && castPosEnd!=-1) { // ���ȼ�����ǿ��ת��
			Statement* st;
			handleNode(bin,castPosEnd+1,endPos,st);
			op->addChild(st);
		}
		else {
			if (splitDir=="M") {
				if (bin[splitPos].second.second == "[") { // ��������
					Statement* stL, * stR;
					handleNode(bin, startPos, splitPos,stL);
					handleNode(bin, splitPos + 1, endPos-1, stR);
					op->addChild(stL);
					op->addChild(stR);
				}
				else { // ������������
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
				cout << "����������" << endl;
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
	else { // ���������
		if (endPos-startPos==1) { // ��������ʶ�����
			string name = bin[startPos].second.second;
			LEX_TYPE lex = bin[startPos].second.first;
			Statement* id = new IdNodeStatement(name,lex,bin[startPos].first);
			handle = id;
		}
		else { // �������
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
				cout << "�������������뺯�����ô����߼�" << endl;
				abort();
			}
		}
	}
}
/***************************************
* ��������  handleEndsWithBracket
* ���ã�    �����Ҵ����Ž��
* ������    ��
* ����ֵ��  ��
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
		cout << "����Ĵ�����" << endl;
		abort();
	}
	cur++;
}
/***************************************
* ��������  handleIf
* ���ã�    ����if��֧
* ������    ��
* ����ֵ��  ��
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
		cout << "If ѭ��û�н�β" << endl;
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
* ��������  handleElse
* ���ã�    ����else �Լ� else if��֧
* ������    ��
* ����ֵ��  ��
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
			cout << "If ѭ��û�н�β" << endl;
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
* ��������  handleWhile
* ���ã�    ����whileѭ��
* ������    ��
* ����ֵ��  ��
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
		cout << "While ѭ��û�н�β" << endl;
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
* ��������  handleFor
* ���ã�    ����forѭ��
* ������    ��
* ����ֵ��  ��
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
		// ������ͨForѭ�� for(int i=0,j=1;i<50;i++,j++)
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
					cout << "��������֮���һ�δʲ��Ǳ�ʶ��" << endl;
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
				cout << "forѭ���ļ�����������Ϊ�������" << endl;
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
					cout << "��������֮���һ�δʲ��Ǳ�ʶ��" << endl;
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
		// ������ǿforѭ�� for(auto i : list)
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
					cout << "��������֮���һ�δʲ��Ǳ�ʶ��" << endl;
				}
				Statement* func = new VarDeclarationStatement(name, type, modifiers, line);
				static_cast<branchStatement*>(branch)->forEachDeclare.push_back(func);
			}
			else {
				cout << "ForEachѭ����ʽ����" << endl;
			}
			Statement* forEachSent;
			handleNode(resTable, cursor + 1, resTable.size(),forEachSent);
			static_cast<branchStatement*>(branch)->forEachDeclare.push_back(forEachSent);
		}
		else {
			cout << "Forѭ����ʽ����" << endl;
			abort();
		}
	}
	else {
		cout << "For ѭ��û�н�β" << endl;
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
* ��������  handleAssign
* ���ã�    ������ͨ���ʽ���
*           ���ڲ�����һ�ζ��ڶ��ŵ��и
* ������    ��
* ����ֵ��  ��
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
* ��������  handleSwitch
* ���ã�    ����switch��֧
* ������    ��
* ����ֵ��  ��
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
		cout << "Switch ѭ��û�н�β" << endl;
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
* ��������  handleCase
* ���ã�    ����case��֧
* ������    ��
* ����ֵ��  ��
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
		cout << "Case�ĸ�ʽ����:����ֻ�ܸ���һ������" << endl;
		abort();
	}
}
/***************************************
* ��������  handleBreak
* ���ã�    ����break���
*           ���ڲ�����һ�ζ��ڶ��ŵ��и
* ������    ��
* ����ֵ��  ��
****************************************/
void GrammarAnalyser::handleBreak() {
	int line = lexRes[cur].first;
	Statement* breakst = new breakStatement(line);
	curStatement->addChild(breakst);
	cur++;
}
/***************************************
* ��������  handleDefault
* ���ã�    ����default��֧
* ������    ��
* ����ֵ��  ��
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
		cout << "Default�ĸ�ʽ����:��ð��" << endl;
		abort();
	}
}
/***************************************
* ��������  handleReturn
* ���ã�    ����return���
* ������    ��
* ����ֵ��  ��
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
* ��������  handleIndentation
* ���ã�    ���������м��
* ������    ��
* ����ֵ��  ��
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
* ��������  parseComponent
* ���ã�    ����һ���﷨������Զ����µ�˼��
* ������    ��
* ����ֵ��  ��
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
			cout << "��������֮���һ�δʲ��Ǳ�ʶ��" << endl;
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
* ��������  printParseTree
* ���ã�    ��ӡ�﷨������
* ������    ��
* ����ֵ��  ��
****************************************/
void GrammarAnalyser::printParseTree() {
	cout << "���´�ӡ�﷨��������" << endl;
	root->getInfo(0);
}
/***************************************
* ��������  getParseTree
* ���ã�    ��ȡ���������ڵ��﷨������
* ������    root        [in/out]
* ����ֵ��  ��
****************************************/
void GrammarAnalyser::getParseTree(Statement *&root) {
	root = this->root;
}
/***************************************
* ��������  extractType
* ���ã�    ��lexRes�н�����ǰָ��ָ�������
* ������    type       ����������[in/out]
* ����ֵ��  ��
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
* ��������  extractType
* ���ã�    ��lexRes�н�����ǰָ��ָ�������
* ������    type       ����������[in/out]
*           tmpResBin  token����   [in]
*           cursor     tokenָ��   [in/out]
* ����ֵ��  ��
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
* ��������  extractArgTable
* ���ã�    ��lexRes�н�����������ʱ�Ĳ����б�
* ������    argTable   �����б�������[in/out]
* ����ֵ��  ��
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
* ��������  extractJudgement
* ���ã�    ��lexRes�з������֧����жϱ��ʽ
* ������    tmpResBin  �жϱ��ʽ������[in/out]
* ����ֵ��  ��
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
		cout << "�����β��һ��С���������" << endl;
		abort();
	}
}
/***************************************
* ��������  split
* ���ã�    ��lexRes�а���Ŀ��token���ж�������и�
* ������    tmpResBin     ���������[in/out]
*           commaedPos    �ڲ��и����Ԫ������������[in/out]
*           end           ����и���[in]
*           mid           �ڲ��и���[in]
* ����ֵ��  ��
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
* ��������  split
* ���ã�    ��lexRes�а���Ŀ��token���е�������и�
* ������    tmpResBin     ���������[in/out]
*           end           �и���[in]
* ����ֵ��  ��
****************************************/
void GrammarAnalyser::split(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& tmpResBin,std::string end) {
	while (lexRes[cur].second.first != LEX_TYPE::SEPERATOR || lexRes[cur].second.second != end) {
		tmpResBin.push_back(lexRes[cur++]);
	}
}
/***************************************
* ��������  judgeInvalidExpression
* ���ã�    �жϱ��ʽ�Ƿ�Ϸ��������Ϸ��������ж�
* ������    bin:       token����         [in]
*           startPos:  ��ʼ����λ��      [in]
*           endPos:    ��������λ��      [in]
* ����ֵ��  ��
****************************************/
void judgeInvalidExpression(std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>>& bin, int startPos, int endPos) {
	// [1] ����Ƿ�Ϊ�ս���
	if (startPos>=endPos) {
		cout << "���ڿ��ӽ��" << endl;
		abort();
	}
	// [2] ����Ƿ���ڷǷ��ַ�
	bool flag = false;
	for (int cursor = startPos; cursor < endPos;cursor++) {
		if (bin[cursor].second.first==LEX_TYPE::SEPERATOR && bin[cursor].second.second == ";") {
			flag = true;
		}
	}
	if (flag) {
		cout << "���ڷǷ��ַ�" << endl;
		abort();
	}
	// ����Ƿ���������Գ������ࣨ�����ȱʡ��
	if (endPos-startPos>=2) {
		for (int cursor = startPos; cursor < endPos; cursor++) {
			if (bin[cursor].second.first == LEX_TYPE::SEPERATOR || bin[cursor].second.first == LEX_TYPE::OPERATOR) {
				flag = true;
			}
		}
		if (flag==false) {
			cout << "���������Գ�������" << endl;
			abort();
		}
	}
	// ��������Ƿ�ƥ��
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
		cout << "���Ų�ƥ��" << endl;
		abort();
	}
}
/***************************************
* ��������  searchForSpace
* ���ã�    ����ĳһ�е�����
* ������    spaceTable:  ������  [in]
*           line:        �к�    [in]
* ����ֵ��  int
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
* ��������  isModifier
* ���ã�    �ж��Ƿ�Ϊ���η���ؼ���
* ������    word��     ���������[in]
* ����ֵ��  bool
****************************************/
bool isModifier(string word) {
	return (word == "inline") || (word == "public") || (word == "static") || (word == "protected") || (word == "private") || (word=="unsigned")||(word=="const");
}
/***************************************
* ��������  isProtoType
* ���ã�    �ж��Ƿ��ǻ�������
* ������    word:      ���������    [in]
* ����ֵ��  bool
****************************************/
bool isProtoType(string word) {
	return (word == "void") || (word == "int") || (word == "char") || (word == "bool") || (word == "double") || (word == "float")||(word=="long")||(word=="auto")||(word=="unsigned");
}
/***************************************
* ��������  isClassName
* ���ã�    �ж��Ƿ���ע�������
* ������    word:      ���������    [in]
* ����ֵ��  bool
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
* ��������  isType
* ���ã�    �ж��Ƿ��ܹ���������
* ������    lex��      ����Ĵʷ�����[in]
*           word:      ���������    [in]
* ����ֵ��  bool
****************************************/
bool isType(LEX_TYPE lex, string word) {
	return ((lex == LEX_TYPE::KEYWORD) && isProtoType(word)) || ((lex == LEX_TYPE::IDENTIFIER) && (isClassName(word)));
}
/***************************************
* ��������  hasTemplate
* ���ã�    �ж������Ƿ���ڷ��ͣ�ģ�壩
* ������    word:      ���������    [in]
* ����ֵ��  bool
****************************************/
bool hasTemplate(string word) {
	for (auto it : existTemplate) {
		if (it == word) {
			return true;
		}
	}
	return false;
}