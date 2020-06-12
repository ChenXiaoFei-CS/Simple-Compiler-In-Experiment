#include "SemanticAnalyser.h"
#include "SemanticOpTable.h"
#include <iostream>
using namespace std;
/***************************************
* 函数名：  SemanticAnalyser
* 作用：    构造器
* 参数：    root   解析树头节点[in]
****************************************/
SemanticAnalyser::SemanticAnalyser(Statement *root) {
    this->parseTreeRoot = root;
    this->level = 0;
}
/***************************************
* 函数名：  ~SemanticAnalyser
* 作用：    析构函数
****************************************/
SemanticAnalyser::~SemanticAnalyser() {}
/***************************************
* 函数名：  semantics
* 作用：    开始进行语义分析
* 参数：    无
* 返回值：  void
****************************************/
void SemanticAnalyser::semantics() {
    cout << "进行语义分析!!" << endl;
    this->level = 0;
    analyserNode(parseTreeRoot); // 树状解析，递归写法，引入栈麻烦
}
/***************************************
* 函数名：  printFuncInfo
* 作用：    打印函数表
* 参数：    无
* 返回值：  void
****************************************/
void SemanticAnalyser::printFuncInfo() {
    std::cout << "下面打印函数表" << std::endl;
    for (int i = 0; i < 50;i++) {
        std::cout << "=";
    }
    std::cout << std::endl;
    for (auto it:functionTable) {
        it->printInfo();
    }
    for (int i = 0; i < 50; i++) {
        std::cout << "=";
    }
    std::cout << std::endl;
}
/***************************************
* 函数名：  printVarInfo
* 作用：    打印变量表
* 参数：    无
* 返回值：  void
****************************************/
void SemanticAnalyser::printVarInfo() {
    std::cout << "下面打印变量表" << std::endl;
    for (int i = 0; i < 50; i++) {
        std::cout << "=";
    }
    std::cout << std::endl;
    for (auto it : variableTable) {
        it->printInfo();
    }
    for (int i = 0; i < 50; i++) {
        std::cout << "=";
    }
    std::cout << std::endl;
}
/***************************************
* 函数名：  analyserNode
* 作用：    自下而上对某一个结点进行语义检查
* 参数：    root         结点的指针[in]
* 返回值：  void
****************************************/
void SemanticAnalyser::analyserNode(Statement* root) {
    level++;
    if (root->getInstanceName()=="RootStatement") {
        for (Statement* child : root->getChildList()) {
            analyserNode(child);
        }
    }
    else if(root->getInstanceName() == "FuncDefineStatement"){
        level++;
        registerFunc((FuncDefineStatement*)root);
        level--;
        // 检查是否每一条路径都有return
        checkReturnComplement((FuncDefineStatement*)root);
        for (Statement* child:root->getChildList()) {
            analyserNode(child);
        }
        // 此处实现将ID表中大于level变量全部置为Deprecated
        for (auto it : variableTable) {
            if (it->getDeprecated()==false && it->getLevel() > level) {
                it->setDeprecated(true);
            }
        }
    }
    else if (root->getInstanceName() == "VarDeclarationStatement") {
        registerVar((VarDeclarationStatement*)root);
    }
    else if (root->getInstanceName() == "branchStatement") {
        checkJudge((branchStatement*)root);
        for (Statement* child : root->getChildList()) {
            analyserNode(child);
        }
        for (auto it : variableTable) {
            if (it->getDeprecated() == false && it->getLevel() > level) {
                it->setDeprecated(true);
            }
        }
    }
    else if (root->getInstanceName()=="breakStatement") {
    // 此处暂时无检查
    }
    else if (root->getInstanceName() == "returnStatement") {
        checkReturn((returnStatement*)root);
    }
    else if (root->getInstanceName() == "OpNodeStatement") {
        SemanticType type;
        classInfo* info;
        operate((OpNodeStatement*)root,type,info);
    }
    else if (root->getInstanceName()=="IdNodeStatement") {
        SemanticType type;
        classInfo* info;
        judgeIDType((IdNodeStatement*)root,type,info);
    }
    else if (root->getInstanceName() == "FunctionStatement") {
        SemanticType type;
        classInfo* info;
        judgeFunctionType((FunctionStatement*)root, type, info);
    }
    else {
        cout <<"Other:" <<root->getInstanceName() << endl;
        abort();
    }
    level--;
}
/***************************************
* 函数名：  registerVar
* 作用：    对于变量表中的表项进行注册
* 参数：    varDeclNode  解析树变量声明结点的指针[in]
* 返回值：  void
****************************************/
void SemanticAnalyser::registerVar(VarDeclarationStatement* varDeclNode) {
    std::string name = varDeclNode->getName();
    std::string type = varDeclNode->getType();
    std::vector<std::string> modifiers = varDeclNode->getModifiers();
    int line = varDeclNode->getLine();
    // 1. 解析type
    classInfo* info;
    SemanticType Semantype;
    parseType(type, Semantype, info);
    // 2. 生成VarTableItem表项
    varTableItem* item = new varTableItem(level,name,Semantype,info,modifiers,line);
    // 3. 查找ID表，看一下有没有相同level并且重名的
    for (auto it : variableTable) {
        if (it->getDeprecated()==false && it->getName() == name && level==it->getLevel()) {
            cout << "Error2: 变量重定义,Line:" << line << endl;
            abort();
        }
    }
    // 4. 填充ID表
    variableTable.push_back(item);
}
/***************************************
* 函数名：  registerVar
* 作用：    对于变量表中的表项进行注册
* 参数：    name        变量名        [in]
*           type        变量的语法类型[in]
*           line        变量声明的函数[in]
* 返回值：  void
****************************************/
void SemanticAnalyser::registerVar(std::string name, std::string type, int line) {
    std::vector<std::string> modifiers;
    // 1. 解析type
    classInfo* info;
    SemanticType Semantype;
    parseType(type, Semantype, info);
    // 2. 生成VarTableItem表项
    varTableItem* item = new varTableItem(level, name, Semantype, info, modifiers, line);
    // 3. 查找ID表，看一下有没有相同level并且重名的
    for (auto it : variableTable) {
        if (it->getDeprecated() == false && it->getName() == name && level == it->getLevel()) {
            cout << "Error2: 变量重定义,Line:" << line << endl;
            abort();
        }
    }
    // 4. 填充ID表
    variableTable.push_back(item);
}
/***************************************
* 函数名：  registerFunc
* 作用：    对于函数表中的表项进行注册
* 参数：    funcNode  解析树函数定义结点的指针[in]
* 返回值：  void
****************************************/
void SemanticAnalyser::registerFunc(FuncDefineStatement *funcNode) {
    std::string name = funcNode->getName();
    std::vector<std::string> modifiers = funcNode->getModifiers();
    std::string returnType = funcNode->getReturnType();
    std::vector<std::pair<std::string, std::string>> args = funcNode->getArgTable();
    int stratLine = funcNode->getStartLine();
    int endLine = funcNode->getEndLine();
    std::vector<SemanticType> argTypes;
    // 1. 解析返回值
    classInfo* info;
    SemanticType rTtype;
    parseType(returnType, rTtype, info);
    funcTableItem* item = new funcTableItem(level,name, rTtype,stratLine,endLine);
    if (info!=NULL) {
        item->fillInReturnClassInfo(info);
    }
    // 2. 解析参数列表
    for (auto it : args) { 
        classInfo* info;
        SemanticType type;
        parseType(it.first,type,info);
        argTypes.push_back(type);
        if (info!=NULL) {
            item->fillInArgClassInfo(info);
        }
        registerVar(it.second,it.first, stratLine);
    }
    item->fillInArgTable(argTypes);
    // 3.检查函数表
    for (funcTableItem* it : functionTable) {
        if (it->getName()==name && it->getArgTable().size()== argTypes.size()) {
            bool flag = true;
            int classNamePtr = 0;
            for (int i = 0; i < argTypes.size();i++) {
                if (it->getArgTable()[i]!=argTypes[i]) {
                    flag = false;
                    break;
                }
                else if (argTypes[i]==SemanticType::CLASS || argTypes[i] == SemanticType::CLASS_PTR) {
                    if (it->getArgClassInfo()[classNamePtr]->className != item->getArgClassInfo()[classNamePtr]->className) {
                        flag = false;
                        break;
                    }
                    classNamePtr++;
                }
            }
            if (flag) {
                cout << "Error1: 函数表重复定义,Line:" << stratLine << endl;
                abort();
            }
        }
    }
    // 解析修饰符
    item->fillInModfiers(modifiers);
    // 登记注册函数表
    functionTable.push_back(item);
}
/***************************************
* 函数名：  parseType
* 作用：    将语法类型解析为语义类型
* 参数：    proto       原始语法类型          [in]
*           type        语义类型输出句柄      [in/out]
*           info        语义类型关联类信息句柄[in/out]
* 返回值：  void
****************************************/
void SemanticAnalyser::parseType(std::string proto, SemanticType& type, classInfo*& info) {
    if (proto=="int") {
        type = SemanticType::INT;
        info = NULL;
    }
    else if (proto == "int*") {
        type = SemanticType::INT_PTR;
        info = NULL;
    }
    else if (proto == "char") {
        type = SemanticType::CHAR;
        info = NULL;
    }
    else if (proto == "char*") {
        type = SemanticType::CHAR_PTR;
        info = NULL;
    }
    else if (proto == "char**") {
        type = SemanticType::CHAR_PTR_PTR;
        info = NULL;
    }
    else if (proto == "double") {
        type = SemanticType::DOUBLE;
        info = NULL;
    }
    else if (proto == "double*") {
        type = SemanticType::DOUBLE_PTR;
        info = NULL;
    }
    else if (proto == "float") {
        type = SemanticType::FLOAT;
        info = NULL;
    }
    else if (proto == "float*") {
        type = SemanticType::FLOAT_PTR;
        info = NULL;
    }
    else if (proto == "long") {
        type = SemanticType::LONG;
        info = NULL;
    }
    else if (proto == "long*") {
        type = SemanticType::LONG_PTR;
        info = NULL;
    }
    else if (proto == "short") {
        type = SemanticType::SHORT;
        info = NULL;
    }
    else if (proto == "short*") {
        type = SemanticType::SHORT_PTR;
        info = NULL;
    }
    else if (proto == "void") {
        type = SemanticType::VOID;
        info = NULL;
    }
    else if (proto == "void*") {
        type = SemanticType::VOID_PTR;
        info = NULL;
    }
    else if (proto == "bool") {
        type = SemanticType::BOOL;
        info = NULL;
    }
    else if (proto == "bool*") {
        type = SemanticType::BOOL_PTR;
        info = NULL;
    }
    else {  // 判定为类或者类的指针
        std::string className = "";
        vector<SemanticType> templates;
        bool hasPtr = false;
        int cur = 0;
        for (;cur<proto.size();cur++) {
            if (proto[cur]=='*' || proto[cur]=='<') {
                break;
            }
            className += proto[cur];
        }
        if (proto[cur]=='<') {
            cur++;
            int leftBranck = 1;
            while (proto[cur]!='>' && cur<proto.size()) {
                std::string templateProto = "";
                SemanticType typeProto;
                classInfo* Templateinfo;
                while ((proto[cur]!='>'|| leftBranck!=1)&& proto[cur]!=',' && cur<proto.size()) {
                    if (proto[cur] == '<') {
                        leftBranck++;
                    }
                    if (proto[cur] == '>') {
                        leftBranck--;
                    }
                    templateProto += proto[cur++];
                }
                if (proto[cur] == ',') {
                    cur++;
                }
                parseType(templateProto,typeProto,Templateinfo);
                templates.push_back(typeProto);
            }
        }
        if (proto[cur]=='*' && cur== proto.size()-1) { // 目前只处理单个指针
            hasPtr = true;
        }
        else if(proto[cur] == '*'){
            cout << "出现对象的多重指针，超过编译器处理范围" << endl;
            abort();
        }
        if (hasPtr==true) {
            type = SemanticType::CLASS_PTR;
            info = new classInfo(className, templates);
        }
        else {
            type = SemanticType::CLASS;
            info = new classInfo(className, templates);
        }
    }
}
/***************************************
* 函数名：  judgeIDType
* 作用：    判断IDNode所对应的语义类型
* 参数：    idNode     IDNode结点的指针[in]
*           type       语义类型输出句柄[in/out]
*           classInfo  语义类型关联类信息句柄[in/out]
* 返回值：  void
****************************************/
void SemanticAnalyser::judgeIDType(IdNodeStatement* idNode, SemanticType& type, classInfo*& info) {
    if (idNode->getLex()==LEX_TYPE::IDENTIFIER) {
        bool flag = false;
        varTableItem* objIt = NULL;
        for (auto it:variableTable) {
            if (it->getName() == idNode->getName() && it->getDeprecated()==false) {
                flag = true;
                objIt = it;
                break;
            }
        }
        if (flag) {
            type = objIt->getSemanType();
            info = objIt->getClassInfo();
        }
        else {
            cout << "Error3: 使用未声明的变量,Line:" << idNode->getLine() << endl;
            abort();
        }
    }
    else if (idNode->getLex() == LEX_TYPE::CONST) {
        if (idNode->getName()[0]=='\'') {
            type = SemanticType::CHAR;
            info = NULL;
        }
        else if (idNode->getName()[0]=='\"') {
            type = SemanticType::CHAR_PTR;
            info = NULL;
        }
        else{
            bool flag = false;
            for (char m : idNode->getName()) {
                if ((m == '.'||m=='E'||m=='e') && idNode->getName()[idNode->getName().size() - 1] == 'F') {
                    type = SemanticType::FLOAT;
                    info = NULL;
                    flag = true;
                    break;
                }
                else if (m == '.' || m == 'E' || m == 'e') {
                    type = SemanticType::DOUBLE;
                    info = NULL;
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                if (idNode->getName()[idNode->getName().size() - 1] == 'L') {
                    type = SemanticType::LONG;
                    info = NULL;
                }
                else {
                    type = SemanticType::INT;
                    info = NULL;
                }
            }
        }
    }
    else {
        if (idNode->getName()=="true" || idNode->getName()=="false") {
            type = SemanticType::BOOL;
            info = NULL;
        }
    }
}
/***************************************
* 函数名：  judgeFunctionType
* 作用：    解析FunctionNode所对应的语义类型
* 参数：    root       FunctionNode结点的指针[in]
*           type       语义类型输出句柄[in/out]
*           classInfo  语义类型关联类信息句柄[in/out]
* 返回值：  void
****************************************/
void SemanticAnalyser::judgeFunctionType(FunctionStatement* root, SemanticType& type, classInfo*& info) {
    std::string name = root->getName();
    // 查找函数表
    bool flag = false;
    funcTableItem* item = NULL;
    for (auto it:functionTable) {
        if (it->getName() == name && it->getArgTable().size() == root->getArgList().size()) {
            flag = true;
            item = it;
        }
    }
    if (!flag) {
        cout << "Error5: 使用未定义的函数,Line:" << root->getLine() << endl;
        abort();
    }
    int classCur = 0;
    for (int i = 0; i < root->getArgList().size();i++) {
        SemanticType argType;
        classInfo* argInfo;
        if (root->getArgList()[i]->getInstanceName()=="OpNodeStatement") {
            operate((OpNodeStatement*)root->getArgList()[i],argType, argInfo);
        }
        else if (root->getArgList()[i]->getInstanceName() == "IdNodeStatement") {
            judgeIDType((IdNodeStatement*)root->getArgList()[i],argType,argInfo);
        }
        else {
            judgeFunctionType((FunctionStatement*)root->getArgList()[i],argType,argInfo);
        }
        // 匹配
        if (argType !=item->getArgTable()[i]) {
            std::cout << "Error6: 调用函数参数类型不匹配,Line:" << root->getLine() << std::endl;
            abort();
        }
        if (argType == SemanticType::CLASS || argType == SemanticType::CLASS_PTR) {
            if (argInfo->className != item->getArgClassInfo()[classCur++]->className) {
                std::cout << "Error6: 调用函数参数类型不匹配,Line:" << root->getLine() << std::endl;
            }
        }
    }
    type = item->getReturnType();
    info = item->getReturnInfo();
}
/***************************************
* 函数名：  operate
* 作用：    解析OpNodeStatement所对应的语义类型
* 参数：    opNode       FunctionNode结点的指针[in]
*           type         语义类型输出句柄[in/out]
*           classInfo    语义类型关联类信息句柄[in/out]
* 返回值：  void
****************************************/
void SemanticAnalyser::operate(OpNodeStatement* opNode, SemanticType& type, classInfo* &info) {
    std::string castType = opNode->getCastType();
    if (opNode->getOpDir() == "M") {
        Statement* op1 = opNode->getChildList()[0];
        Statement* op2 = opNode->getChildList()[1];
        SemanticType castSemanType;
        classInfo* castInfo;
        SemanticType typeOp1, typeOp2;
        classInfo* classInfo1=NULL, * classInfo2=NULL;
        if (op1->getInstanceName() == "OpNodeStatement") {
            operate((OpNodeStatement*)op1, typeOp1, classInfo1);
        }
        else if (op1->getInstanceName() == "IdNodeStatement") {
            judgeIDType((IdNodeStatement*)op1, typeOp1, classInfo1);
        }
        else {
            judgeFunctionType((FunctionStatement*)op1,typeOp1,classInfo1);
        }
        if (op2->getInstanceName() == "OpNodeStatement") {
            operate((OpNodeStatement*)op2, typeOp2, classInfo2);
        }
        else if (op2->getInstanceName() == "IdNodeStatement") {
            judgeIDType((IdNodeStatement*)op2, typeOp2, classInfo2);
        }
        else {
            judgeFunctionType((FunctionStatement*)op2, typeOp2, classInfo2);
        }
        checkOperation(opNode->getOpType(), opNode->getOpDir(),typeOp1,classInfo1,typeOp2,classInfo2,type,info,opNode->getLine(),castSemanType,castInfo);
    }
    else {
        SemanticType castSemanType;
        classInfo* castInfo;
        if (opNode->getOpType()=="Cast") {
            parseType(castType,castSemanType,castInfo);
        }
        SemanticType typeOp1, typeOp2;
        classInfo* classInfo1 = NULL,*classInfo2=NULL;
        Statement* op1 = opNode->getChildList()[0];
        if (op1->getInstanceName() == "OpNodeStatement") {
            operate((OpNodeStatement*)op1, typeOp1, classInfo1);
        }
        else if (op1->getInstanceName() == "IdNodeStatement") {
            judgeIDType((IdNodeStatement*)op1, typeOp1, classInfo1);
        }
        else {
            judgeFunctionType((FunctionStatement*)op1,typeOp1,classInfo1);
        }
        checkOperation(opNode->getOpType(), opNode->getOpDir(), typeOp1, classInfo1, typeOp2, classInfo2,type,info, opNode->getLine(),castSemanType,castInfo);
    }
}
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
void SemanticAnalyser::checkOperation(std::string OpType, std::string OpDir, SemanticType &typeOp1, classInfo* &classInfo1, SemanticType &typeOp2, classInfo* &classInfo2, SemanticType& resType, classInfo*& resInfo,int line,SemanticType &castSemanType,classInfo *&castInfo) {
    cout << "Handle " << OpType << OpDir << " || " << convertSemanticType2String(typeOp1)<<"\t" << convertSemanticType2String(typeOp2) << endl;
    resType = semanticTableOpCheck(OpType + OpDir, typeOp1, typeOp2,castSemanType);
    if (resType==SemanticType::NONE) {
        cout << "Error4: 运算符不合法或不支持,Line:" <<line << endl;
        abort();
    }
}
/***************************************
* 函数名：  checkJudge
* 作用：    检查分支是否合法
* 参数：    judge        分支结点指针[in]
* 返回值：  void
****************************************/
void SemanticAnalyser::checkJudge(branchStatement* judge) {
    std::string branchType = judge->getbranchType();
    if (branchType=="if" || branchType=="while" || branchType=="else if") {
        SemanticType type;
        classInfo* info;
        if (judge->judgement->getInstanceName()=="OpNodeStatement") {
            operate((OpNodeStatement*)(judge->judgement),type,info);
        }
        else if (judge->judgement->getInstanceName() == "IdNodeStatement") {
            judgeIDType((IdNodeStatement*)(judge->judgement),type,info);
        }
        else if (judge->judgement->getInstanceName() == "FunctionStatement"){
            judgeFunctionType((FunctionStatement*)(judge->judgement),type,info);
        }
        else {
            cout << "Error7:判断表达式内部不支持此种格式，Line" << judge->getStartLine() << endl;
            abort();
        }
        if (type != SemanticType::BOOL && type!=SemanticType::INT) {
            cout << "Error8:判断表达式不合法,Line" << judge->getStartLine() << endl;
        }
    }
    else if (branchType == "for") {
        for (auto it : judge->init) {
            analyserNode(it);
        }
        SemanticType type;
        classInfo* info;
        if (judge->end->getInstanceName() == "OpNodeStatement") {
            operate((OpNodeStatement*)(judge->end), type, info);
        }
        else if (judge->end->getInstanceName() == "IdNodeStatement") {
            judgeIDType((IdNodeStatement*)(judge->end), type, info);
        }
        else if (judge->end->getInstanceName() == "FunctionStatement") {
            judgeFunctionType((FunctionStatement*)(judge->end), type, info);
        }
        else {
            cout << "Error7:判断表达式内部不支持此种格式，Line" << judge->getStartLine() << endl;
            abort();
        }
        if (type != SemanticType::BOOL && type != SemanticType::INT) {
            cout << "Error8:判断表达式不合法,Line" << judge->getStartLine() << endl;
        }
        for (auto it : judge->iterate) {
            analyserNode(it);
        }
    }
    else if (branchType == "switch") {
        SemanticType type;
        classInfo* info;
        if (judge->judgement->getInstanceName() == "OpNodeStatement") {
            operate((OpNodeStatement*)(judge->judgement), type, info);
        }
        else if (judge->judgement->getInstanceName() == "IdNodeStatement") {
            judgeIDType((IdNodeStatement*)(judge->judgement), type, info);
        }
        else if (judge->judgement->getInstanceName() == "FunctionStatement") {
            judgeFunctionType((FunctionStatement*)(judge->judgement), type, info);
        }
        else {
            cout << "Error7:判断表达式内部不支持此种格式，Line" << judge->getStartLine() << endl;
            abort();
        }
        if (type != SemanticType::CHAR && type != SemanticType::INT) {
            cout << "Error8:判断表达式不合法,Line" << judge->getStartLine() << endl;
        }
    }
    else if (branchType=="else") {
    // 此处无检查
    }
    else if (branchType== "break") {
    // 此处无检查
    }
    else if (branchType == "default") {
    // 此处无检查
    }
    else if (branchType == "case") {
        SemanticType type;
        classInfo* info;
        if (judge->judgement->getInstanceName() == "OpNodeStatement") {
            operate((OpNodeStatement*)(judge->judgement), type, info);
        }
        else if (judge->judgement->getInstanceName() == "IdNodeStatement") {
            judgeIDType((IdNodeStatement*)(judge->judgement), type, info);
        }
        else if (judge->judgement->getInstanceName() == "FunctionStatement") {
            judgeFunctionType((FunctionStatement*)(judge->judgement), type, info);
        }
        else {
            cout << "Error7:判断表达式内部不支持此种格式，Line" << judge->getStartLine() << endl;
            abort();
        }
        if (type != SemanticType::CHAR && type != SemanticType::INT) {
            cout << "Error8:判断表达式不合法,Line" << judge->getStartLine() << endl;
            abort();
        }
    }
}
/***************************************
* 函数名：  checkReturn
* 作用：    检return语句是否与函数定义匹配
* 参数：    judge        分支结点指针[in]
* 返回值：  void
****************************************/
void SemanticAnalyser::checkReturn(returnStatement* root) {
    if (root->getChildList().size()>1) {
        cout << "Error9:Return语句格式错误，超过一个句子" << endl;
        abort();
    }
    if (root->getChildList().size() == 1) {
        SemanticType type;
        classInfo* info = NULL;
        if (root->getChildList()[0]->getInstanceName() == "OpNodeStatement") {
            operate((OpNodeStatement*)(root->getChildList()[0]), type, info);
        }
        else if (root->getChildList()[0]->getInstanceName() == "IdNodeStatement") {
            judgeIDType((IdNodeStatement*)(root->getChildList()[0]), type, info);
        }
        else if (root->getChildList()[0]->getInstanceName() == "FunctionStatement") {
            judgeFunctionType((FunctionStatement*)(root->getChildList()[0]), type, info);
        }
        Statement* func = root->getParent();
        while (func->getInstanceName() != "FuncDefineStatement" && func->getInstanceName() != "RootStatement")
        {
            func = func->getParent();
        }
        std::string returnType = ((FuncDefineStatement*)func)->getReturnType();
        SemanticType returnSemanType;
        classInfo* returnInfo;
        parseType(returnType, returnSemanType, returnInfo);
        if (returnSemanType != type) {
            cout << "Error10:Return 返回类型与函数返回类型不一致,Line" << endl;
            abort();
        }
        if (type == SemanticType::CLASS || type == SemanticType::CLASS_PTR) {
            if (info->className != returnInfo->className) {
                cout << "Error10:Return 返回类型与函数返回类型不一致,Line" << endl;
                abort();
            }
        }
    }
}
/***************************************
* 函数名：  checkReturnComplement
* 作用：    函数定义结点是否存在return
* 参数：    root        函数定义结点指针[in]
* 返回值：  void
****************************************/
void SemanticAnalyser::checkReturnComplement(FuncDefineStatement* root) {
    if (root->getReturnType()!="void") {
        bool flag = false;
        for (auto it:root->getChildList()) {
            if (it->getInstanceName()=="returnStatement"&& ((returnStatement*)it)->getChildList().size() > 0) {
                flag = true;
                break;
            }
        }
        if (!flag) {
            // 1.检查有无if,else if,else分支结构
            bool flag1 = false;
            for (auto it : root->getChildList()) {
                if (it->getInstanceName() == "branchStatement"){
                    std::string branchType = ((branchStatement*)it)->getbranchType();
                    if (branchType=="if" || branchType=="else if" || branchType=="else") {
                        flag = true;
                        break;
                    }
                }
            }
            if (!flag1) {
                cout << "Error11: 函数无return语句结尾,Line:" << root->getStartLine() << endl;
                abort();
            }
            // 2. 检查是否每条路径都有return（暂时不支持重复嵌套）
            flag1 = true;
            for (auto it : root->getChildList()) {
                if (it->getInstanceName() == "branchStatement") {
                    if ((((branchStatement*)it)->getbranchType() == "if") || (((branchStatement*)it)->getbranchType() == "else") || (((branchStatement*)it)->getbranchType() == "else if")) {
                        bool flag2 = false;
                        for (auto itt : it->getChildList()) {
                            if (itt->getInstanceName() == "returnStatement" && ((returnStatement*)itt)->getChildList().size()>0) {
                                flag2 = true;
                                break;
                            }
                        }
                        if (!flag2) {
                            flag1 = false;
                            break;
                        }
                    }
                }
            }
            if (!flag1) {
                cout << "Error11: 函数无return语句结尾,Line:" << root->getStartLine() << endl;
                abort();
            }
        }
    }
}

/***************************************
* 函数名：  fillInArgTable
* 作用：    填充函数表项中的参数类型
* 参数：    args       参数列表的语义类型[in]
* 返回值：  void
****************************************/
void funcTableItem::fillInArgTable(std::vector<SemanticType> args) {
    this->argList = args;
}
/***************************************
* 函数名：  fillInReturnClassInfo
* 作用：    备用项，填充函数表项中的返回值类型关联类信息
* 参数：    info       返回值类型关联类信息[in]
* 返回值：  void
****************************************/
void funcTableItem::fillInReturnClassInfo(classInfo *info) {
    this->returnClass = info;
}
/***************************************
* 函数名：  fillInReturnClassInfo
* 作用：    备用项，填充函数表项中的参数类型关联类信息
* 参数：    info       参数类型关联类信息[in]
* 返回值：  void
****************************************/
void funcTableItem::fillInArgClassInfo(classInfo* info) {
    this->argClassInfoTable.push_back(info);
}
/***************************************
* 函数名：  fillInModfiers
* 作用：    填充函数表项中的修饰符列表
* 参数：    modifiers       修饰符列表[in]
* 返回值：  void
****************************************/
void funcTableItem::fillInModfiers(std::vector<std::string> modifiers) {
    this->modifiers = modifiers;
}
std::string funcTableItem::getName() {
    return this->funcName;
}
std::vector<SemanticType> funcTableItem::getArgTable() {
    return this->argList;
}
std::vector<classInfo*> funcTableItem::getArgClassInfo() {
    return this->argClassInfoTable;
}
/***************************************
* 函数名：  printInfo
* 作用：    打印函数表表项信息
* 参数：    无
* 返回值：  void
****************************************/
void funcTableItem::printInfo() {
    std::cout << "[ Name: " << this->getName() << " ]";
    std::cout << "[ returnType: " << convertSemanticType2String(this->returnType);
    if (this->returnType == SemanticType::CLASS || this->returnType == SemanticType::CLASS_PTR) {
        std::cout << "-" << this->returnClass->className;
        if (this->returnClass->templates.size()>0) {
            std::cout << "<";
            for(auto it : this->returnClass->templates) {
                cout << convertSemanticType2String(it);
                cout << " ";
            }
            std::cout << ">";
        }
    }
    std::cout << "]";
    std::cout << "[argTable:{";
    int record = 0;
    for (auto it:argList) {
        std::cout << convertSemanticType2String(it);
        if (it == SemanticType::CLASS || it == SemanticType::CLASS_PTR) {
            std::cout << "-" << this->argClassInfoTable[record]->className;
            if (this->argClassInfoTable[record]->templates.size() > 0) {
                std::cout << "<";
                for (auto it : this->argClassInfoTable[record]->templates) {
                    cout << convertSemanticType2String(it);
                    cout << " ";
                }
                std::cout << ">";
            }
            record++;
        }
        std::cout << " ";
    }
    std::cout << "}]";
    std::cout << std::endl;
}
/***************************************
* 函数名：  printInfo
* 作用：    打印变量表表项信息
* 参数：    无
* 返回值：  void
****************************************/
void varTableItem::printInfo() {
    std::cout << "[ Name: " << this->getName() << " ]";
    std::cout << "[ Level: " << this->getLevel() << " ]";
    std::cout << "[ Type: " << convertSemanticType2String(this->type);
    if (this->type == SemanticType::CLASS || this->type == SemanticType::CLASS_PTR) {
        std::cout << "-" << this->info->className;
        if (this->info->templates.size() > 0) {
            std::cout << "<";
            for (auto it : this->info->templates) {
                cout << convertSemanticType2String(it);
                cout << " ";
            }
            std::cout << ">";
        }
    }
    std::cout << "]";
    std::cout << std::endl;
}

/***************************************
* 函数名：  convertSemanticType2String
* 作用：    将语义类型转换未字符串
* 参数：    type       语义类型[in]
* 返回值：  string
****************************************/
std::string convertSemanticType2String(SemanticType type) {
    if (type == SemanticType::CHAR) {
        return "CHAR";
    }
    if (type == SemanticType::CHAR_PTR) {
        return "CHAR_PTR";
    }
    if (type == SemanticType::CHAR_PTR_PTR) {
        return "CHAR_PTR_PTR";
    }
    if (type == SemanticType::CLASS) {
        return "CLASS";
    }
    if (type == SemanticType::CLASS_PTR) {
        return "CLASS_PTR";
    }
    if (type == SemanticType::DOUBLE) {
        return "DOUBLE";
    }
    if (type == SemanticType::DOUBLE_PTR) {
        return "DOUBLE_PTR";
    }
    if (type == SemanticType::FLOAT) {
        return "FLOAT";
    }
    if (type == SemanticType::FLOAT_PTR) {
        return "FLOAT_PTR";
    }
    if (type == SemanticType::INT) {
        return "INT";
    }
    if (type == SemanticType::INT_PTR) {
        return "INT_PTR";
    }
    if (type == SemanticType::LONG) {
        return "LONG";
    }
    if (type == SemanticType::LONG_PTR) {
        return "LONG_PTR";
    }
    if (type == SemanticType::NONE) {
        return "NONE";
    }
    if (type == SemanticType::SHORT) {
        return "SHORT";
    }
    if (type == SemanticType::BOOL) {
        return "BOOL";
    }
    if (type == SemanticType::BOOL_PTR) {
        return "BOOL_PTR";
    }
    if (type == SemanticType::SHORT_PTR) {
        return "SHORT_PTR";
    }
    if (type==SemanticType::VOID) {
        return "VOID";
    }
    if (type==SemanticType::VOID_PTR) {
        return "VOID_PTR";
    }
    return "";
}