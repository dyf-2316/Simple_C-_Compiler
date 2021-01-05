# include "code.h"

ofstream out;
int label_seq = 0;
int rodata_seq = 0;


int tag = 1;

int newLabel() {
    return tag++;
}

void recursive_get_label(TreeNode *node, int next_label) {
    if (node == NULL)
    {
        return;
    } 
    switch (node->nodekind) {
    case StmtK:
        stmt_get_label(node, next_label);
        break;
    case ExpK:
        expr_get_label(node);
        break;
    case DeclK:
        recursive_get_label(node->sibling, next_label);
        break;
    case ProgK:
        recursive_get_label(node->childs[0], next_label);
        break;
    default:
        break;
    }
}

void stmt_get_label(TreeNode *node, int next_label) {
    switch (node->kind.stmt) {
        case CompK:{
                if(node->childs[0]){
                    TreeNode *stmt = node->childs[0];
                    stmt->label.begin_label = node->label.begin_label;
                    for ( ; stmt; stmt = stmt->sibling){
                        recursive_get_label(stmt, next_label);
                    }
                }
            }
            break;
        case WhileK:{
                TreeNode *expr = node->childs[0];
                TreeNode *stmt = node->childs[1];
                if (node->label.begin_label == 0) {
                    node->label.begin_label = newLabel();
                }
                stmt->label.next_label = node->label.begin_label; // 继续循环
                stmt->label.begin_label = expr->label.true_label = newLabel(); // 真值 =》循环开始
                
                if (node->sibling) { // 若while语句有兄弟 
                    node->label.next_label = newLabel();
                    node->sibling->label.begin_label = expr->label.false_label = node->label.next_label;
                }
                else {
                    expr->label.false_label = node->label.next_label = next_label;
                }
                recursive_get_label(expr, 0);
                recursive_get_label(stmt, node->label.begin_label);
            }   
            break;

            case ForK: {
                TreeNode *expr_1 = node->childs[0];
                TreeNode *expr_2 = node->childs[1];
                TreeNode *expr_3 = node->childs[2];
                TreeNode *stmt = node->childs[3];
                if (node->label.begin_label != 0)
                {
                    expr_1->label.begin_label = node->label.begin_label;
                }
                node->label.begin_label = newLabel();
                stmt->label.next_label = node->label.begin_label;
                stmt->label.begin_label = expr_2->label.true_label = newLabel();

                if (node->sibling) { // 若for语句有兄弟
                    node->label.next_label = newLabel();
                    node->sibling->label.begin_label = expr_2->label.false_label = node->label.next_label;
                }
                else {
                    expr_2->label.false_label = node->label.next_label = next_label;
                }
                recursive_get_label(expr_2, 0);
                recursive_get_label(stmt, node->label.begin_label);
            }
            break;

        case IfK: {
                TreeNode *expr = node->childs[0];
                TreeNode *stmt = node->childs[1];
                TreeNode *else_stmt = node->childs[2];
                expr->label.true_label = stmt->label.begin_label = newLabel();

                if (node->sibling) {
                    node->label.next_label = newLabel();
                    node->sibling->label.begin_label = expr->label.false_label = stmt->label.next_label = node->label.next_label;
                }
                else
                    expr->label.false_label = stmt->label.next_label = node->label.next_label = next_label;

                if (else_stmt) {
                    expr->label.false_label = else_stmt->label.begin_label = newLabel();
                    else_stmt->label.next_label = node->label.next_label;
                }
                
                recursive_get_label(expr, 0);
                recursive_get_label(stmt, node->label.next_label);
                recursive_get_label(else_stmt, node->label.next_label);
            }
            break;
            
        default:
            break;
    }
}

void expr_get_label(TreeNode *t) {
    if (t->type != Boolean)
        return;
    TreeNode *expr_1 = t->childs[0];
    TreeNode *expr_2 = t->childs[1];
    switch (t->attr.op)
    {
    case Logical_and:
        expr_1->label.true_label = newLabel();
        expr_2->label.true_label = t->label.true_label;
        expr_1->label.false_label = expr_2->label.false_label = t->label.false_label;
        break;
    case Logical_or:
        expr_1->label.false_label = newLabel();
        expr_2->label.false_label = t->label.false_label;
        expr_1->label.true_label = expr_2->label.true_label = t->label.true_label;
        break;
    case Logical_not:
        expr_1->label.true_label = t->label.false_label;
        expr_1->label.false_label = t->label.true_label;
        break;
    default:
        break;
    }
    recursive_get_label(expr_1, 0);
    recursive_get_label(expr_2, 0);
}


string getLabel(int label) {
    return "L" + to_string(label);
}

void genData(){
    if(symTables.sym_table.size()){
        out << "\t.data" << endl;
        for(auto sym: symTables.sym_table){
            out << "\t.align\t4" << endl;
            out << "\t.size\t" << sym->label << ", 4" << endl;
            out << sym->label << ":" << endl;
            out << 	"\t.long\t" << sym->value.Int << endl; 
        }
        for (int i = 0; i <= max_temp_var_seq; i++) {
                out << "\t.align\t4" << endl;
                out << "\t.size\tt" << i << ", 4" << endl;
                out << "t" << i << ":" << endl;
                out << 	"\t.long\t" << 0 << endl; 
        }
    }
}

void genRodata(){
    if(rodataNodes.size()){
        out << "\t.section\t.rodata" << endl;
        for(auto rodataNode: rodataNodes){
            rodataNode->label.data_label = rodata_seq;
            rodata_seq++;
            out <<  "RO" + to_string(rodataNode->label.data_label) + ":" << endl;
            out << "\t.string\t" << (char*)rodataNode->attr.val << endl;
        }
    }
}

void genCode(TreeNode *root) { 
    BuildSymTable(root);
    recursive(root);
    recursive_get_label(root, 0);
    cout<< "************************************************ AST 节点 ************************************************\n";
    Display(root);
    cout<< "\n\n************************************************ 符号表 ************************************************\n";
    symTables.ShowSymTable();

    out.open("res.asm");

    genData();
    genRodata();

    out << "\t.text" << endl;
    out << "\t.globl	main" << endl;
    out << "main:" << endl;
    out << "\tpushl\t%ebp" << endl;
    out << "\tmovl\t%esp, %ebp" << endl;

    recursiveGenCode(root);
    out << "L0:" << endl;
    out << "\tmovl\t$0, %eax" << endl;
    out << "\tpopl\t%ebp" << endl;
    out << "\tret" << endl;
}


string jmpCode(int label, TreeNode *node) {
    string code = "";
    if (node->sibling ){
        code =  "\tjmp \t" + getLabel(label) + "\n";
    }
    return code;
}

void recursiveGenCode(TreeNode *node){
    if (node == NULL){
        return;
    } else if (node->nodekind == StmtK){
        genStmtCode(node);
    } else if (node->nodekind == ExpK && node->kind.exp == OpK){
        genExprCode(node);
    } else if (node->nodekind == ProgK){
        recursiveGenCode(node->childs[0]); 
        for (TreeNode *brothe = node->childs[0]->sibling; brothe; brothe = brothe->sibling){
            recursiveGenCode(brothe);
        }
    } else if (node->nodekind == DeclK){
        if (node->label.begin_label){
            out << getLabel(node->label.begin_label) << ":" << endl;
        }
    }
}

void genExprCode(TreeNode *node) {
    ShowNode(node);
    if (node->label.begin_label){
        out << getLabel(node->label.begin_label) << ":" << endl;
    }
    switch (node->attr.op){
        case Assign:
            recursiveGenCode(node->childs[1]);
            if (node->childs[1]->kind.exp == IntConstK){
                out << "\tmovl\t" << getReg(node->childs[1]) << ", " << getReg(node->childs[0]) << endl;
            } else if(node->childs[1]->kind.exp == OpK && node->childs[1]->attr.op == Assign){
                out << "\tmovl\t" << getReg(node->childs[1]->childs[0]) << ", %eax" << endl;
                out << "\tmovl\t" << "%eax, " << getReg(node->childs[0]) << endl;
            }else{
                out << "\tmovl\t" << getReg(node->childs[1]) << ", %eax" << endl;
                out << "\tmovl\t" << "%eax, " << getReg(node->childs[0]) << endl;
            }
            break;
        case Equ: 
        case Gtr: 
        case Lss: 
        case Geq:
        case Leq: 
        case Neq:
            genCmpExprCode(node);
            break;
        case Logical_and:
        case Logical_not:
        case Logical_or:
            genLogicalExprCode(node);
            break;
        default:
            genBaseExprCode(node);
            break;
    }
}

void genStmtCode(TreeNode *node){
    ShowNode(node);
    switch (node->kind.stmt){
        case CompK:
            if(node->childs[0]){
                recursiveGenCode(node->childs[0]); 
                for (TreeNode *brothe = node->childs[0]->sibling; brothe; brothe = brothe->sibling)
                {
                    recursiveGenCode(brothe);
                }
            }
            break;
        case InputK:
            if (node->label.begin_label){
                out << getLabel(node->label.begin_label) << ":" << endl;
            }
            out << "\tsubl\t$8, %esp" << endl;
            out << "\tpushl\t$" << getReg(node->childs[1]) << endl;
            out << "\tpushl\t$RO" << node->childs[0]->label.data_label << endl;
            out << "\tcall\tscanf" << endl;
            out << "\taddl\t$16, %esp" << endl;
            break;
        case PrintK:
            if (node->label.begin_label){
                out << getLabel(node->label.begin_label) << ":" << endl;
            }
            if (node->childs[1]){
                recursiveGenCode(node->childs[1]);
                out << "\tmovl\t" << getReg(node->childs[1]) << ", %eax" << endl;
                out << "\tsubl\t$8, %esp" << endl;
                out << "\tpushl\t%eax" << endl;
                out << "\tpushl\t$RO" << node->childs[0]->label.data_label << endl;
                out << "\tcall\tprintf" << endl;
                out << "\taddl\t$16, %esp" << endl;
            } else {
                out << "\tsubl\t$12, %esp" << endl;
                out << "\tpushl\t$RO" << node->childs[0]->label.data_label << endl;
                out << "\tcall\tprintf" << endl;
                out << "\taddl\t$16, %esp" << endl;
            }
            break;
        case WhileK:
            if (node->label.begin_label){
                out << getLabel(node->label.begin_label) << ":" << endl;
            }
            recursiveGenCode(node->childs[0]);
            recursiveGenCode(node->childs[1]);
            out << jmpCode(node->childs[1]->label.next_label, node);
            break;
        case ForK:
            recursiveGenCode(node->childs[0]); // 初始化
            out << getLabel(node->label.begin_label) << ":" << endl;
            recursiveGenCode(node->childs[1]);
            recursiveGenCode(node->childs[3]);
            recursiveGenCode(node->childs[2]);
            out << jmpCode(node->childs[3]->label.next_label, node);
            break;
        case IfK:
            if (node->label.begin_label){
                out << getLabel(node->label.begin_label) << ":" << endl;
            }
            recursiveGenCode(node->childs[0]);
            recursiveGenCode(node->childs[1]);
            if (node->childs[2]) {
                out << "\tjmp " << getLabel(node->childs[1]->label.next_label) << endl;
                recursiveGenCode(node->childs[2]);
            }
            out << jmpCode(node->childs[1]->label.next_label, node);
            break;
    }
}

void genLogicalExprCode(TreeNode *node) {
    TreeNode *expr_1 = node->childs[0];
    TreeNode *expr_2 = node->childs[1];
    switch (node->attr.op){
        case Logical_and:
            recursiveGenCode(expr_1);
            out << getLabel(expr_1->label.true_label) << ":" << endl;
            recursiveGenCode(expr_2);
            break;
    case Logical_not:
        recursiveGenCode(expr_1);
        break;
    case Logical_or:
        recursiveGenCode(expr_1);
        out << getLabel(expr_1->label.false_label) << ":" << endl;
        recursiveGenCode(expr_2);
        break;
    default:
        break;
    }
}

void genCmpExprCode(TreeNode *node) {
    recursiveGenCode(node->childs[0]);
    recursiveGenCode(node->childs[1]);
    out << "\tmovl\t" << getReg(node->childs[0]) << ", %eax" << endl;
    out << "\tmovl\t" << getReg(node->childs[1]) << ", %ebx" << endl;
    out << "\tcmpl\t" << "%ebx, %eax" << endl;
    switch (node->attr.op) {
    case Equ:
        out << "\tje  \t";
        break;
    case Gtr:
        out << "\tjg  \t";
        break;
    case Lss:
        out << "\tjl  \t";
        break;
    case Geq:
        out << "\tjge \t";
        break;
    case Leq:
        out << "\tjbe \t";
        break;
    case Neq:
        out << "\tjne \t";
    default:
        break;
    }
    out << getLabel(node->label.true_label) << endl;
    out << "\tjmp \t" << getLabel(node->label.false_label) << endl;
}

string getReg(TreeNode *node) {
    string id;
    if (node->kind.exp == IdK){
        id = ((Symbol*)node->attr.val)->label;
    } else if (node->kind.exp == IntConstK || node->kind.exp == CharConstK) {
        id = "$" + to_string(*(int*)node->attr.val);
    } else 
        id = "t" + to_string(node->temp_var);
    return id;
}

void genBaseExprCode(TreeNode *node) {
    TreeNode *expr_1 = node->childs[0];
    TreeNode *expr_2 = node->childs[1];
    recursiveGenCode(expr_1);
    recursiveGenCode(expr_2);
    out << "\tmovl\t" << getReg(expr_1) << ", %eax" << endl;
    switch (node->attr.op){
    case Add:
        out << "\taddl\t" << getReg(expr_2) << ", %eax" << endl;
        break;
    case Min:
        if(expr_2){
            out << "\tsubl\t" << getReg(expr_2) << ", %eax" << endl;
        } else{
            out << "\tnegl\t" << "%eax" << endl;
        }
        break;
    case Mul:
        out << "\timull\t" << getReg(expr_2) << ", %eax" << endl;
        break;
    case Div:
        out << "\tmovl\t" << getReg(expr_2) << ", %ecx" << endl;
        out << "\tcltd" << endl;
        out << "\tidivl\t" << "%ecx" << endl;
        break;
    case Mod:
        out << "\tmovl\t" << getReg(expr_2) << ", %ecx" << endl;
        out << "\tcltd" << endl;
        out << "\tidivl\t" << "%ecx" << endl;
        out << "\tmovl\t" << "%edx, %eax" << endl;
        break;
    default:
        break;
    }
    out << "\tmovl\t" << "%eax, t" << node->temp_var << endl;
}
