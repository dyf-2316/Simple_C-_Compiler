//
// Created by DYF Macbook on 12/1/20.
//

#ifndef UNTITLED_SYM_TABLE_MAP_H
#define UNTITLED_SYM_TABLE_MAP_H
#include <unordered_map>
#include <vector>
#include <stack>
#include <string>

typedef enum {ValK, FunK, ArrK, StrK} SymKind;

using  namespace std;

class Coordinate{
public:
    int line;
    int column;

    Coordinate(int line, int column) ;

    Coordinate(Coordinate const &pos) ;

    Coordinate() {};

    bool operator < (const Coordinate& pos);

};

class Symbol{
public:
    int id;
    string name;
    Coordinate pos;
    int value;
    int ref;

    Symbol(int id, const string &name, const Coordinate& pos);

    int reference();

};

class Sym_table{
public:
    Coordinate scope_begin;
    Coordinate scope_end;
    unordered_map<string, Symbol*>* symbol_map;
    vector<Sym_table*> sub_tables;

    Sym_table(Coordinate scope_begin);

    void setScopeEnd(Coordinate scope_end);

    Symbol* insert_symbol(int id, const string& name, const Coordinate& pos);

    Symbol* find(const string& name, Coordinate pos);

};

class Sym_table_map{
    int id;
public:
    Sym_table* global_table;
    stack<Sym_table*> table_stack;

    Sym_table_map();

    void begin_sub_scope(int line, int column);

    void end_sub_scope(int line, int column);

    Symbol* insert_symbol(const string& name, int line, int column);

    Symbol* find(const string& name, int line, int column);
};
#endif //UNTITLED_SYM_TABLE_MAP_H
