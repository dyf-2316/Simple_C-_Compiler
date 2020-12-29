# include "code.h"

Sym_table_map symTables;

ofstream out;
int label_seq = 0;
int rodata_seq = 0;

string getLable(){
     string temp = ".LC" + to_string(label_seq) + ":";
     label_seq ++;
     return temp.c_str();
}

void genData(){
    if(symTables.sym_table.size()){
        out << "\t.data" << endl;
        for(auto sym: symTables.sym_table){
            if(sym->type == 1){
                out << "\t.align\t4" << endl;
                out << "\t.size\t" << sym->label << ", 4" << endl;
                out << sym->label << ":" << endl;
                out << 	"\t.long\t" << sym->value.Int << endl; 
            }
        }
    }
}

void genRodata(){
    if(rodataNodes.size()){
        out << "\t.section\t.rodata" << endl;
        for(auto rodataNode: rodataNodes){
            rodataNode->label.data_label = rodata_seq;
            rodata_seq++;
            out <<  ".RO" + to_string(rodataNode->label.data_label) + ":" << endl;
            out << "\t.string\t" << (char*)rodataNode->attr.val << endl;
        }
    }
}

void genCode(TreeNode *root) {
    BuildSymTable(root);
    recursive(root);
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

    recursiveGenCode(root->childs[0]);

    out << "\tmovl\t$0, %eax" << endl;
    out << "\tpopl\t%ebp" << endl;
    out << "\tret" << endl;
}

void recursiveGenCode(TreeNode *node) {
    if (node == NULL)
    {
        return;
    }
    else if (node->nodekind == StmtK)
    {
        genStmtCode(node);
    }
}

void genStmtCode(TreeNode *node){
    switch (node->kind.stmt){
        case CompK:
            recursiveGenCode(node->childs[0]); 
            for (TreeNode *brothe = node->childs[0]->sibling; brothe; brothe = brothe->sibling)
            {
                recursiveGenCode(brothe);
            }
            break;
        case PrintK:
            out << "\tsubl\t$12, %esp" << endl;
            out << "\tpushl\t$" << node->childs[0]->label.data_label << endl;
            out << "\tcall\tputs" << endl;
            out << "\taddl\t$16, %esp" << endl;
            break;
    }
}

void BuildSymTable(TreeNode *node, bool noParas){
    if(!node){
        return;
    }
    if(node->nodekind == StmtK && node->kind.stmt == ForK && node->childs[0] && node->childs[0]->nodekind == DeclK){
        symTables.begin_sub_scope(node->pos);
        if(node->childs[0])BuildSymTable(node->childs[0], true);
        if(node->childs[1])BuildSymTable(node->childs[1], true);
        if(node->childs[2])BuildSymTable(node->childs[2], true);
        BuildSymTable(node->childs[3], false);
        if(node->sibling == nullptr){
            return;
        }
        BuildSymTable(node->sibling, true);
        return;
    }

    if(node->nodekind == DeclK && node->kind.decl == _DeclK){
        TreeNode *temp = node->childs[1];
        while (temp){
            if(temp->nodekind == DeclK && temp->kind.decl == InitK){
                Symbol *sym = symTables.insert_symbol(temp->childs[0]->attr.name, temp->childs[0]->pos);
                if(sym){
                    if (temp->childs[1]->nodekind == ExpK && temp->childs[1]->kind.exp == IntConstK){
                        sym->value.Int = *(int*)temp->childs[1]->attr.val;
                    }else if (temp->childs[1]->nodekind == ExpK && temp->childs[1]->kind.exp == StrConstK)
                    {
                        sym->value.Char = (char*)temp->childs[1]->attr.val;
                    }
                    if (node->childs[0]->childs[0]){
                        if(node->childs[0]->childs[0]->nodekind == ExpK){
                            sym->type = node->childs[0]->childs[0]->type;
                            temp->childs[0]->type = node->childs[0]->childs[0]->type;
                        }else{
                            sym->type = node->childs[0]->childs[1]->type;
                            temp->childs[0]->type = node->childs[0]->childs[0]->type;
                        }
                    }
                    sym->label = (to_string(sym->id) + '_' + sym->name).c_str();
                    temp->childs[0]->attr.val = (void*) sym;
                } else{
                    erroring("变量重复定义", temp->childs[0]->pos);
                    temp->childs[0]->attr.val = nullptr;
                }
            }else{
                Symbol *sym = symTables.insert_symbol(temp->attr.name, temp->pos);
                if (sym){
                    if (node->childs[0]->childs[0]){
                        if(node->childs[0]->childs[0]->nodekind == ExpK){
                            sym->type = node->childs[0]->childs[0]->type;
                            temp->type = node->childs[0]->childs[0]->type;
                        }else{
                            sym->type = node->childs[0]->childs[1]->type;
                            temp->type = node->childs[0]->childs[1]->type;
                        }
                    }
                    sym->label = (to_string(sym->id) + '_' + sym->name).c_str();
                    temp->attr.val = (void*) sym;
                } else {
                    erroring("变量重复定义", temp->pos);
                    temp->attr.val = nullptr;
                }
            }
            temp = temp->sibling;
        }
        if(node->sibling == nullptr){
            return;
        }
        BuildSymTable(node->sibling, true);
        return;
    }
    if(node->nodekind == ExpK && node->kind.exp == IdK){
        Symbol *sym = symTables.find(node->attr.name, node->pos);
        if(!sym){
            erroring("变量未声明", node->pos);
        }else{
            node->attr.val = (void*) symTables.find(node->attr.name, node->pos);
            node->type = (DeclType)sym->type;
        }
        if(node->sibling == nullptr){
            return;
        }
        BuildSymTable(node->sibling, true);
        return;
    }


    if(node->nodekind == StmtK && node->kind.stmt == CompK){
        if(noParas){
            symTables.begin_sub_scope(node->pos);
        }
        for(int i = 0; i < MAXCHILDREN; i++) {
            if(node->childs[i] != NULL)
            {
                BuildSymTable(node->childs[i], true);
            }
        }
        symTables.end_sub_scope((Coordinate*)node->attr.val);
        if(node->sibling == nullptr){
            return;
        }
        BuildSymTable(node->sibling, true);
        return;
    }

    for(int i = 0; i < MAXCHILDREN; i++) {
        if(node->childs[i] != NULL)
        {
            BuildSymTable(node->childs[i], true);
        }
    }

    if(node->sibling == nullptr){
        return;
    }
    BuildSymTable(node->sibling, true);
    return ;
}
