#include "TreeNode.h"
int nodes = 0;
unordered_map<int, string> optMap;

TreeNode * newTreeNode(NodeKind nodekind, int kind) {
    TreeNode * t = new TreeNode(NodeKind(nodekind), kind);
    return t;
}

TreeNode::TreeNode(NodeKind nodekind, int kind) {
    // 一定要给孩子节点初始化为空，否则会出现段错误，或者不知名的错误
    for (int i = 0; i < MAXCHILDREN; i++){
        this->childs[i] = NULL;
    }
    this->sibling = NULL;
    this->nodekind = nodekind;
    switch (nodekind){
    case StmtK:
        this->kind.stmt = StmtKind(kind);
        break;
    case ExpK:
        this->kind.exp = ExpKind(kind);
        break;
    case DeclK:
        this->kind.type = DeclType(kind);
        break;
    }
    this->node_id = nodes++; // 全局变量
}

void ConstructMap() {
    optMap.insert(make_pair(Add, "+"));
    optMap.insert(make_pair(Min, "-"));
    optMap.insert(make_pair(Mul, "*"));
    optMap.insert(make_pair(Div, "/"));
    optMap.insert(make_pair(Mod, "%"));
    optMap.insert(make_pair(Dadd, "++"));
    optMap.insert(make_pair(Dmin, "--"));
    optMap.insert(make_pair(Assign, "="));
    optMap.insert(make_pair(Equ, "=="));
    optMap.insert(make_pair(Gtr, ">"));
    optMap.insert(make_pair(Lss, "<"));
    optMap.insert(make_pair(Geq, ">="));
    optMap.insert(make_pair(Leq, "<="));
    optMap.insert(make_pair(Neq, "!="));
    optMap.insert(make_pair(Logical_and, "&&"));
    optMap.insert(make_pair(Logical_or, "||"));
    optMap.insert(make_pair(Logical_not, "!"));
    
}

void Operate(TreeNode* p) {
    Display(p);
}

void Display(TreeNode* p){
    TreeNode* bro;
    for(int i = 0; i < MAXCHILDREN; i++) {
        if(p->childs[i] != NULL)
        {
            Display(p->childs[i]);
        }
    }
    ShowNode(p);
    bro = p->sibling;
    if(bro != NULL){
        Display(bro);
    }
}

void ShowNode(TreeNode *p) {
    string type = "";
    string detail = "";
    string child_lineno = "Children: ";
    if (p->nodekind == StmtK) {

        switch (p->kind.stmt) //IfK, WhileK, AssignK, ForK, CompK, InputK, PrintK
        {
            case IfK:
                type = "if statement";
                break;
            case IfElseK:
                type = "if-else statement";
                break;
            case ElseK:
                type = "else statement";
                break;
            case DoWhileK:
                type = "do-while statement";
                break;
            case WhileK:
                type = "while statement";
                break;
            case AssignK:
                type = "assign statememt";
                break;
            case ForK:
                type = "for statement";
                break;
            case CompK:
                type = "compound statement";
                break;
            case InputK:
                type = "input statement";
                break;
            case PrintK:
                type = "print statement";
                break;
            default:
                break;
        }
    } else if (p->nodekind == ExpK) {
        if (p->kind.exp == OpK) {
            type = "Expr";
            detail = "op: " + optMap.at(p->attr.op);
        } else if (p->kind.exp == IdK) {
            type = "ID Declaration";
            detail = "symbol: " + string(p->attr.name);
        } else if (p->kind.exp == IntConstK) {
            type = "Const Declaration";
            detail = "value: " + to_string(*((int*)p->attr.val));
        } else if (p->kind.exp == StrConstK) {
            type = "Const Declaration";
            detail = "value: " + string((char*)p->attr.val);
        }else if (p->kind.exp == TypeK) {
            type = "Type Specifier";
            switch (p->type) {
                case Void:
                    detail = "void";
                    break;
                case Char:
                    detail = "char";
                    break;
                case Integer:
                    detail = "integer";
                    break;
                case Boolean:
                    detail = "boolean";
                    break;
                default:
                    break;
            }
        }
    } else if (p->nodekind == DeclK) {
        type = "Var Declaration";
        
    }
    cout << p->node_id << setw(20) << type << setw(20) << detail << setw(20) << child_lineno;
    for (int i = 0; i < MAXCHILDREN; ++i) {
        if (p->childs[i] != NULL) {
            cout << p->childs[i]->node_id << "   ";
            TreeNode *temp = p->childs[i];
            while (temp->sibling != NULL) {
                cout << temp->sibling->node_id << "   ";
                temp = temp->sibling;
            }
        }
    }
    cout << endl;
}

int str2int(string num){
    int val = 0, curr;
    if(num == "0"){
        return 0;
    }
    if(num[0] != '0'){
        val = num[0] - '0';
        curr = 1;
        while(num[curr]){
            val *= 10;
            val += num[curr] - '0';
            curr ++;
        }
    }else if(num[1] != 'X' && num[1] != 'x'){
        val = num[1] - '0';
        curr = 2;
        while(num[curr]){
            val *= 8;
            val += num[curr] - '0';
            curr ++;
        }
    }else{
        if(num[2] >= '0' && num[2] <= '9'){
            val += num[2] - '0';
        }else{
            val += num[2] - 'A' + 10;
        }
        curr = 3;
        while(num[curr]){
            val *= 16;
            if(num[curr] >= '0' && num[curr] <= '9'){
                val += num[curr] - '0';
            }else{
                val += num[curr] - 'A' + 10;
            }
            curr ++;
        }
    }
    return val;
}

TreeNode* newIntConstNode(char* num){
    int val = str2int(num);
    TreeNode* node = newTreeNode(ExpK, IntConstK);
    int* temp = new int[1];
    *temp = val;
    node->attr.val = (void*)temp;
    return node;
}

TreeNode* newStrConstNode(char* str){
    TreeNode* node = newTreeNode(ExpK, StrConstK);
    node->attr.val = (void*)str;
    return node;
}

TreeNode* newOpNode(OpType op){
    TreeNode* node = newTreeNode(ExpK, OpK);
    node->attr.op = op;
    return node;
}

TreeNode* newTypeNode(DeclType type){
    TreeNode* node = newTreeNode(ExpK, TypeK);
    node->type = type;
    return node;
}

TreeNode* newIdNode(char* name){
    TreeNode* node = newTreeNode(ExpK, IdK);
    node->attr.name = name;
    return node;
}

TreeNode* newExpNode(OpType op, TreeNode *operand_1, TreeNode *operand_2) {
    TreeNode* node = new TreeNode(ExpK, OpK);
    node->attr.op = op;
    node->childs[0] = operand_1;
    node->childs[1] = operand_2;
    return node;
}

TreeNode* newStmtNode(int kind) {
    TreeNode *node = newTreeNode(StmtK, kind);
    return node;
}

TreeNode* newForStmtNode(TreeNode* exp_1, TreeNode* exp_2, TreeNode* exp_3, TreeNode* stmt) {
    TreeNode* node = newTreeNode(StmtK, ForK);
    node->childs[0] = exp_1;
    node->childs[1] = exp_2;
    node->childs[2] = exp_3;
    node->childs[3] = stmt;
    return node;
}

TreeNode * newDeclNode(TreeNode* type, TreeNode *idlist) {
    TreeNode *node = newTreeNode(DeclK, type->kind.type);
    node->childs[0] = type;
    node->childs[1] = idlist;
    TreeNode *temp = idlist;
    // 符号表在语法树建好后自顶向下构建
    return node;
}

TreeNode * newWhileStmtNode(int kind, TreeNode* expr, TreeNode* stmt){
    TreeNode *node = newTreeNode(StmtK, kind);
    node->childs[0] = expr;
    node->childs[1] = stmt;
    return node;
}

TreeNode * newIfStmtNode(TreeNode *expr, TreeNode *stmt) {
    TreeNode *t = newTreeNode(StmtK, IfK); // if-stmt or else-if-stmt
    t->childs[0] = expr;
    t->childs[1] = stmt;
    return t;
}

TreeNode * newElseStmtNode(TreeNode *stmt) {
    TreeNode *t = newTreeNode(StmtK, ElseK);
    t->childs[0] = stmt;
    return t;
}

TreeNode * newIfElseStmtNode(TreeNode *If, TreeNode * Else) {
    TreeNode *t = newTreeNode(StmtK, IfElseK);
    t->childs[0] = If;
    t->childs[1] = Else;
    return t;
}

TreeNode * newInputStmtNode(TreeNode *opd) {
    TreeNode *t = newTreeNode(StmtK, InputK);
    t->childs[0] = opd;
    return t;
}
TreeNode * newOutputStmtNode(TreeNode *opd) {
    TreeNode *t = newTreeNode(StmtK, PrintK);
    t->childs[0] = opd;
    return t;
}

TreeNode * newComStmtNode(TreeNode *stmts) {
    TreeNode *t = newTreeNode(StmtK, CompK);
    t->childs[0] = stmts;
    return t;
}
