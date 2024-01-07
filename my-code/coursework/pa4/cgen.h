#include "cool-tree.h"
#include "emit.h"
#include "symtab.h"
#include <assert.h>
#include <stdio.h>
#include <unordered_map>

enum Basicness { Basic, NotBasic };
#define TRUE 1
#define FALSE 0

struct Context {
  std::unordered_map<std::string,
                     SymbolTable<Symbol, std::pair<std::string, int32_t>>>
      E;
  std::unordered_map<std::string,
                     SymbolTable<Symbol, std::pair<std::string, int32_t>>>
      method_environment;
  SymbolTable<Symbol, Symbol> C;
};

class CgenClassTable;
typedef CgenClassTable *CgenClassTableP;

class CgenNode;
typedef CgenNode *CgenNodeP;

class CgenClassTable : public SymbolTable<Symbol, CgenNode> {
private:
  List<CgenNode> *nds;
  ostream &str;
  int stringclasstag;
  int intclasstag;
  int boolclasstag;

  // The following methods emit code for
  // constants and global declarations.

  void code_global_data();
  void code_global_text();
  void code_bools(int);
  void code_select_gc();
  void code_constants();

  int get_class_tag(CgenNodeP nd);
  int get_numof_attrs(CgenNodeP nd);
  void set_default_values_of_attributes(CgenNodeP nd);
  void code_protobject(CgenNodeP nd);
  void code_protobjects();

  void code_class_name_entry(CgenNodeP nd);
  void code_class_name_table();

  void code_class_object_entry(CgenNodeP nd);
  void code_class_object_table();

  void code_dispatch_methods(CgenNodeP nd);
  void code_dispatch_table(CgenNodeP nd);
  void code_dispatch_tables();

  void code_initializer(CgenNodeP nd);
  void code_initializers();

  void code_method(CgenNodeP nd, method_class *m);
  void code_class_methods(CgenNodeP nd);
  void code_all_methods();

  // The following creates an inheritance graph from
  // a list of classes.  The graph is implemented as
  // a tree of `CgenNode', and class names are placed
  // in the base class symbol table.

  void install_basic_classes();
  void install_class(CgenNodeP nd);
  void install_classes(Classes cs);
  void build_method_and_attribute_indices(CgenNodeP current_class, CgenNodeP nd,
                                          int32_t &method_counter,
                                          int32_t &attribute_counter);
  void build_inheritance_tree();
  void set_relations(CgenNodeP nd);

public:
  CgenClassTable(Classes, ostream &str);
  void code();
  CgenNodeP root();
  int32_t get_index_of_method_in_dispatch_table(Symbol class_name,
                                                Symbol method_name);
  int32_t get_index_of_attribute_in_dispatch_table(Symbol class_name,
                                                   Symbol attribute_name);
  Context context;
  int32_t free_label{};
};

class CgenNode : public class__class {
private:
  CgenNodeP parentnd;       // Parent of class
  List<CgenNode> *children; // Children of class
  Basicness basic_status;   // `Basic' if class is basic
                            // `NotBasic' otherwise

public:
  CgenNode(Class_ c, Basicness bstatus, CgenClassTableP class_table);

  void add_child(CgenNodeP child);
  List<CgenNode> *get_children() { return children; }
  void set_parentnd(CgenNodeP p);
  CgenNodeP get_parentnd() { return parentnd; }
  int basic() { return (basic_status == Basic); }
};

class BoolConst {
private:
  int val;

public:
  BoolConst(int);
  void code_def(ostream &, int boolclasstag);
  void code_ref(ostream &) const;
};
