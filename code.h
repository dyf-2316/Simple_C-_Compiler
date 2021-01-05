#include "TreeNode.h"
#include "Sym_table_map.h"
#include <fstream>

extern ofstream out;

void genCode(TreeNode *root);
void genData(Symbol *sym);
string getLable();
void genExprCode(TreeNode *node);
void genStmtCode(TreeNode *node);

void genLogicalExprCode(TreeNode *node);
string getReg(TreeNode *node);
void genBaseExprCode(TreeNode *node);
void genExprCode(TreeNode *node);
void genCmpExprCode(TreeNode *node);

void recursiveGenCode(TreeNode *node);


extern int tag;

int newLabel();
void recursive_get_label(TreeNode *t, int next_label);
void stmt_get_label(TreeNode *t, int next_label);
void expr_get_label(TreeNode *t);