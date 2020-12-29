#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <unordered_map>
#include "Sym_table_map.h"

#define MAXCHILDREN 6

using namespace std;


typedef enum {StmtK, ExpK, DeclK, ProgK} NodeKind;
typedef enum {IfK, IfElseK, ElseK, WhileK, DoWhileK, AssignK, ForK, CompK, InputK, PrintK} StmtKind;
typedef enum {OpK,IntConstK,StrConstK,IdK,TypeK} ExpKind;
typedef enum {_DeclK, InitK, ConstK, SpecK} DeclKind;
typedef enum {Void,Integer,Char,Boolean} DeclType;

typedef enum {Add, Min, Mul, Div, Mod, Dadd, Dmin, Assign,
    Equ, Gtr, Lss, Geq, Leq, Neq, Logical_and, Logical_or, Logical_not} OpType;
extern unordered_map<int, string> optMap;
extern int temp_var_seq;
struct Label {
    int true_label;
    int false_label;
    int begin_label;
    int end_label;
    int next_label;
    int data_label;
};
class TreeNode {
public:
    int id;
    TreeNode* childs[MAXCHILDREN];
    TreeNode* sibling; // 用于遍历同层次的兄弟节点
    Coordinate* pos;
    NodeKind nodekind;
    union { StmtKind stmt; ExpKind exp; DeclKind decl;} kind;
    union {
        OpType op; // 操作符
        void* val; // 常量
        char* name; // id名字
    }attr;
    int temp_var;
    Label label;
    DeclType type; // 用于类型检查：此节点的类
    TreeNode(NodeKind nodekind, int kind);
};

extern vector<TreeNode*> rodataNodes;

void erroring(string str, Coordinate pos);
void check_type(TreeNode *node);
void get_temp_var(TreeNode *node);
void recursive(TreeNode *node);

void Display(TreeNode* p);
void ShowNode(TreeNode* p);

void ConstructMap();
void BuildSymTable(TreeNode *node, bool noParas = true);

TreeNode* newTreeNode(NodeKind nodekind, int kind);

TreeNode* newProgramNode(TreeNode* program);
TreeNode* newIntConstNode(char* num);
TreeNode* newStrConstNode(char* str);
TreeNode* newOpNode(OpType op);
TreeNode* newTypeNode(DeclType type);
TreeNode* newIdNode(char* name);
TreeNode* newConstNode();

TreeNode* newExpNode(OpType op, TreeNode *operand_1, TreeNode *operand_2);

TreeNode* newStmtNode(int kind);
TreeNode* newForStmtNode(TreeNode* exp_1, TreeNode* exp_2, TreeNode* exp_3, TreeNode* stmt);

TreeNode* newDeclNode(TreeNode* type, TreeNode *idlist);
TreeNode* newInitNode(TreeNode* id, TreeNode* init);
TreeNode* newDeclSpecNode(TreeNode *spec1, TreeNode *spec2);

TreeNode* newWhileStmtNode(int kind, TreeNode* expr, TreeNode* stmt);
TreeNode* newIfStmtNode(TreeNode *expr, TreeNode *stmt);
TreeNode* newElseStmtNode(TreeNode *stmt);
TreeNode* newIfElseStmtNode(TreeNode *If, TreeNode * Else);
TreeNode* newInputStmtNode(TreeNode *opd);
TreeNode* newOutputStmtNode(TreeNode *opd);
TreeNode* newComStmtNode(TreeNode *stmts);
