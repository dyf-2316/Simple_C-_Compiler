#include "TreeNode.h"
#include "Sym_table_map.h"
#include <fstream>

extern ofstream out;

void genCode(TreeNode *root);
void genData(Symbol *sym);
string getLable();

void genStmtCode(TreeNode *node);
void recursiveGenCode(TreeNode *node);