#include "Sym_table_map.h"

Coordinate :: Coordinate(int line, int column) {
    this->line = line;
    this->column = column;
}

Coordinate :: Coordinate(Coordinate const &pos) {
    this->line = pos.line;
    this->column = pos.column;
}

Coordinate :: Coordinate(Coordinate *pos) {
    this->line = pos->line;
    this->column = pos->column;
}

bool Coordinate :: operator < (const Coordinate& pos){
    if(this->line < pos.line || (this->line == pos.line && this->column < pos.column)){
        return true;
    }else{
        return false;
    }
}

Symbol :: Symbol(int id, const string &name, const Coordinate& pos){
    this->id = id;
    this->name.assign(name);
    this->pos = Coordinate(pos);
    this->label = "";
    this->type = 0;
    this->ref = 0;
}

int Symbol :: reference(){
    return ++ ref;
}

Sym_table :: Sym_table(Coordinate scope_begin) {
    this->scope_begin = scope_begin;
    this->scope_end = Coordinate(INT_MAX,INT_MAX);
    symbol_map = new unordered_map<string, Symbol*>;
}

void Sym_table :: setScopeEnd(Coordinate scope_end) {
    this->scope_end = scope_end;
}

Symbol* Sym_table :: insert_symbol(int id, const string& name, const Coordinate& pos){
    if(symbol_map->count(name)){
        symbol_map -> at(name) -> reference();
    }else{
        Symbol* symbol = new Symbol(id, name, pos);
        symbol_map -> insert(pair<string, Symbol*>(name, symbol));
    }
    return symbol_map -> at(name);
}

Sym_table_map :: Sym_table_map() {
    id = 0;
    global_table = new Sym_table(Coordinate(0,0));
    global_table->setScopeEnd(Coordinate(INT_MAX,INT_MAX));
    table_stack.push(global_table);
}

void Sym_table_map :: begin_sub_scope(Coordinate *pos){
    Sym_table* sub_table = new Sym_table(*pos);
    table_stack.top()->sub_tables.push_back(sub_table);
    table_stack.push(sub_table);
}

void Sym_table_map :: end_sub_scope(Coordinate *pos){
    if (table_stack.size() == 1){
        return;
    }
    table_stack.top()->setScopeEnd(*pos);
    table_stack.pop();
}

Symbol* Sym_table_map :: insert_symbol(const string& name, Coordinate *pos){
    Symbol* symbol = table_stack.top()->insert_symbol(id, name, *pos);
    if(symbol->pos.line == pos->line && symbol->pos.column == pos->column){
        sym_table.push_back(symbol);
        id++ ;
        return symbol;
    }
    return nullptr;
}

Symbol* Sym_table_map :: find(const string& name, Coordinate *pos){
    Symbol* symbol = global_table->find(name, *pos);
    if(symbol){
        symbol->reference();
    }
    return symbol;
}

Symbol* Sym_table :: find(const string& name, Coordinate pos) {
    Symbol *symbol = nullptr;
    if (symbol_map->count(name)) {
        symbol = symbol_map->at(name);
    }
    Symbol *temp = nullptr;
    for (int i = 0; i < sub_tables.size(); i++) {
        if (sub_tables[i]->scope_begin < pos && pos < sub_tables[i]->scope_end) {
            temp = sub_tables[i]->find(name, pos);
        }
    }
    if(temp){
        return temp;
    }else{
        return symbol;
    }
}

void Sym_table_map :: ShowSymTable(){
    for(int i = 0; i < id; i++){
        cout <<setw(3) << "#" << sym_table[i]->id ;
        cout << setw(12) << "name: " << sym_table[i]->name;
        cout << setw(12) << "type: " << sym_table[i]->type;
        if (sym_table[i]->type == 1){
            cout << setw(12) << "value: " << sym_table[i]->value.Int;
        } else
        {
            cout << setw(12) << "value: " << sym_table[i]->value.Char;
        }
        cout << setw(15) << "reference: "  << sym_table[i]->ref;
        cout << setw(10) << "line:" << sym_table[i]->pos.line << setw(10) << "column:" << sym_table[i]->pos.column << endl;
    }

}