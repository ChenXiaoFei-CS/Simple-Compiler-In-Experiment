#include "LexAnalyser.h"
#include "GrammarAnalyser.h"
#include "SemanticAnalyser.h"
#include <iostream>
#include <fstream>
#include <istream>
int main(int argc,char *argv[])
{
    std::vector<std::pair<int, std::pair<LEX_TYPE, std::string>>> Res;
    std::vector<std::pair<int, int>> Record;
    std::ifstream fin = openFile(inputDir);
    LexAnalyser analyser(fin);
    analyser.lex();
    fin.close();
    analyser.getLexRes(Res);
    analyser.getSpaceRecord(Record);
    analyser.printLexRes();
    GrammarAnalyser analyser2(Res,Record);
    analyser2.grammar();
    Statement* parseTree = NULL;
    analyser2.getParseTree(parseTree);
    analyser2.printParseTree();
    SemanticAnalyser analyser3(parseTree);
    analyser3.semantics();
    analyser3.printFuncInfo();
    analyser3.printVarInfo();
	return 0;
}