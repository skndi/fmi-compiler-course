#ifndef SEMANT_H_
#define SEMANT_H_

#include "cool-tree.h"
#include "list.h"
#include "stringtab.h"
#include "symtab.h"
#include <assert.h>
#include <iostream>
#include <vector>

#define TRUE 1
#define FALSE 0

class ClassTable;
typedef ClassTable *ClassTableP;

// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.

struct InheritanceGraphNode {
  explicit InheritanceGraphNode(Class_ cls = nullptr) : cls{cls} {}
  Class_ cls;
  std::vector<InheritanceGraphNode> children;

  bool add_child(Symbol parent, Class_ child_cls) {
    if (cls->get_name()->equal_string(parent->get_string(),
                                      parent->get_len())) {
      children.push_back(InheritanceGraphNode(child_cls));
      return true;
    }

    for (auto &child : children) {
      if (child.add_child(parent, child_cls)) {
        return true;
      }
    }

    return false;
  };

  Class_ get_class(Symbol name) {
    if (cls->get_name()->equal_string(name->get_string(), name->get_len())) {
      return cls;
    }

    for (auto &child : children) {
      Class_ child_class = child.get_class(name);
      if (child_class) {
        return child_class;
      }
    }

    return nullptr;
  }

  bool exists(Class_ other) {
    if (cls->get_name()->equal_string(other->get_name()->get_string(),
                                      other->get_name()->get_len())) {
      return true;
    }

    for (auto &child : children) {
      if (child.exists(other)) {
        return true;
      }
    }

    return false;
  }
};

struct InheritanceGraph {
private:
  InheritanceGraphNode base_class;

public:
  void set_base_class(Class_ cls) { base_class = InheritanceGraphNode(cls); };
  bool add_child(Symbol parent, Class_ child_cls) {
    return base_class.add_child(parent, child_cls);
  };
  Class_ get_class(Symbol name) { return base_class.get_class(name); };
  bool exists(Class_ cls) { return base_class.exists(cls); }
};

class ClassTable {
private:
  int semant_errors;
  void install_basic_classes();
  void install_user_defined_classes(Classes classes);
  inline bool check_illegal_inheritance(Class_ cls);

  ostream &error_stream;
  InheritanceGraph inheritance_graph;

public:
  ClassTable(Classes);
  int errors() { return semant_errors; }
  ostream &semant_error();
  ostream &semant_error(Class_ c);
  ostream &semant_error(Symbol filename, tree_node *t);
  Class_ get_class(Symbol name) { return inheritance_graph.get_class(name); }
};

struct TypeEnvironment {
  SymbolTable<Symbol, Symbol> *O;
  SymbolTable<Symbol, Symbol> *M;
  SymbolTable<Symbol, Symbol> *C;
  ClassTable *classtable;
};

#endif
