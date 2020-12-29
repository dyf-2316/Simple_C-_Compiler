#include "TreeNode.h"
int nodes = 0;
int temp_var_seq = 0;
unordered_map<int, string> optMap;


// TODO: char 类型变量声明
// TODO: 代码结构重构

vector<TreeNode*> rodataNodes;

void erroring(string str, Coordinate pos){
    cout << "ERROR " << pos.line << '_' << pos.column << " : " << str << endl;
}

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
        this->kind.decl = DeclKind(kind);
        break;
    case ProgK:
        break;
    }
    this->id = nodes++; // 全局变量
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

void recursive(TreeNode *node) {
    TreeNode *temp;
    int i = 0;
    for(int i = 0; i < MAXCHILDREN; i++) {
        if(node->childs[i] != NULL)
        {
            recursive(node->childs[i]);
        }
    }
    check_type(node);
    temp = node->sibling;
    if(temp != NULL) {
        recursive(temp);
    }
    return;
}

void check_type(TreeNode *node) {
    get_temp_var(node);
    if (node->nodekind == StmtK || node->nodekind == DeclK || node->nodekind == ProgK){
        node->type = Void;
    } else if (node->nodekind == ExpK){
        if (node->kind.exp == OpK){
            if (node->attr.op == Logical_not || node->attr.op == Logical_and || node->attr.op == Logical_or){
                node->type = Boolean;
            }
            else if (node->attr.op == Equ || node->attr.op == Gtr || node->attr.op == Lss || node->attr.op == Geq || node->attr.op == Leq || node->attr.op == Neq) {
                node->type = Boolean;
            }
            else{
                if (!node->childs[1] || (node->childs[1] && node->childs[0]->type == node->childs[1]->type)){
                    node->type = node->childs[0]->type;
                }else{
                    erroring("运算数类型不相同", node->pos);
                    node->type = Integer;
                }
            } 
        }
    }
    return;
}

// 在.data区域里写临时变量！
void get_temp_var(TreeNode *node) { // TODO: temp_var这点搞清楚！！！ 
    if (node->nodekind != ExpK || node->kind.exp != OpK)
        return;
    if (node->attr.op == Logical_and || node->attr.op == Logical_or || node->attr.op == Logical_not)
    {
        return;
    }
    TreeNode *arg1 = node->childs[0];
    TreeNode *arg2 = node->childs[1];
    if (arg1->kind.exp == OpK) {
        temp_var_seq--;
    }
    if (arg2 && arg2->kind.exp == OpK) {
        temp_var_seq--;
    }
    node->temp_var = temp_var_seq;
    switch (node->attr.op){
        case Assign:
        case Equ:
        case Gtr:
        case Lss:
        case Geq:
        case Leq:
        case Neq:
            return;
        case Add:
            break;
        default:
            break;
    }
    temp_var_seq++;
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
            if((Symbol*)p->attr.val){
                detail = "symbol: " + ((Symbol*)p->attr.val)->name +" #"+ to_string(((Symbol*)p->attr.val)->id);
            }else{
                detail = "未声明或重复定义";
            }
        } else if (p->kind.exp == IntConstK) {
            type = "IntConst Declaration";
            detail = "value: " + to_string(*((int*)p->attr.val));
        } else if (p->kind.exp == StrConstK) {
            type = "StrConst Declaration";
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
        switch (p->kind.decl)
        {
        case _DeclK:
            type = "Var Declaration";
            break;
        case InitK:
            type = "Var initializer";
            detail = "op: " + optMap.at(p->attr.op);
            break;
        case ConstK:
            type = "Const Specifier";
            detail = "const";
            break;
        case SpecK:
            type = "Declaration Specifier";
            break;
        default:
                break;
        }
        
    } else if (p->nodekind == ProgK){
        type = "Program";
    }
    cout << setw(3) << "@" << p->id << setw(22) << type << setw(15) << detail;
    // switch (p->type) {
    //     case Void:
    //         cout << setw(10) << "type: "<< "void";
    //         break;
    //     case Char:
    //         cout << setw(10) << "type: "<< "char";
    //         break;
    //     case Integer:
    //         cout << setw(10) << "type: "<< "integer";
    //         break;
    //     case Boolean:
    //         cout << setw(10) << "type: "<< "boolean";
    //         break;
    //     default:
    //         break;
    // }
    cout << setw(13) << "temp_val: " << p->temp_var;
    cout << setw(18) << child_lineno;
    for (int i = 0; i < MAXCHILDREN; ++i) {
        if (p->childs[i] != NULL) {
            cout << "@"<<p->childs[i]->id << "  ";
            TreeNode *temp = p->childs[i];
            while (temp->sibling != NULL) {
                cout << "@" <<temp->sibling->id << "  ";
                temp = temp->sibling;
            }
        }
    }
    if(!p -> pos){
        cout << endl;
        return;
    }
    if(p->nodekind == ExpK && p->kind.exp == OpK){
        cout<<endl;
    }else if(p->nodekind == StmtK && p->kind.stmt == CompK){
        cout << setw(20) << "line:" << p->pos->line << setw(10) << "column:" << p->pos->column;
        cout << setw(10) << "line:" << ((Coordinate*)p->attr.val)->line << setw(10) << "column:" << ((Coordinate*)p->attr.val)->column << endl;
    }else{
        cout << setw(20) << "line:" << p->pos->line << setw(10) << "column:" << p->pos->column << endl;
    }
}

TreeNode* newProgramNode(TreeNode* program){
    TreeNode* node = newTreeNode(ProgK, 0);
    node ->childs [0] = program;
    return node;

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
    node->type = Integer;
    return node;
}

TreeNode* newStrConstNode(char* str){
    TreeNode* node = newTreeNode(ExpK, StrConstK);
    node->attr.val = (void*)str;
    rodataNodes.push_back(node);
    node->type = Char;
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

TreeNode* newConstNode(){
    TreeNode* node = newTreeNode(DeclK, ConstK);
    return node;
}

TreeNode* newIdNode(char* name){
    TreeNode* node = newTreeNode(ExpK, IdK);
    node->attr.name = name;
    return node;
}

TreeNode* newDeclSpecNode(TreeNode *spec1, TreeNode *spec2) {
    TreeNode* node = new TreeNode(DeclK, SpecK);
    node->childs[0] = spec1;
    node->childs[1] = spec2;
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
    TreeNode *node = newTreeNode(DeclK, _DeclK);
    node->childs[0] = type;
    node->childs[1] = idlist;
    node->type = type->type;
    return node;
}

TreeNode* newInitNode(TreeNode* id, TreeNode* init){
    TreeNode *node = newTreeNode(DeclK, InitK);
    node -> childs[0] = id;
    node -> childs[1] = init;
    node->attr.op = Assign;
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
