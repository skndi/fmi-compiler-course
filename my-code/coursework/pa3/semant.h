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

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol arg, arg2, Bool, concat, cool_abort, copy, Int, in_int, in_string,
    IO, length, Main, main_meth, No_class, No_type, Object, out_int, out_string,
    prim_slot, self, SELF_TYPE, Str, str_field, substr, type_name, val;

class ClassTable;
typedef ClassTable *ClassTableP;

// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.

struct TypeEnvironment {
  SymbolTable<Symbol, Symbol> O;
  SymbolTable<Symbol, Symbol> M;
  SymbolTable<Symbol, Symbol> C;
};

struct InheritanceGraphNode {
  explicit InheritanceGraphNode(Class_ cls = nullptr) : cls{cls} {
    type_env.O.enterscope();
    type_env.M.enterscope();
    type_env.C.enterscope();
  };

  Class_ cls;
  TypeEnvironment type_env;
  std::vector<InheritanceGraphNode> children;

  bool add_child(Symbol parent, Class_ child_cls) {
    if (cls->get_name()->equal_string(parent->get_string(),
                                      parent->get_len())) {
      children.emplace_back(child_cls);
      return true;
    }

    for (auto &child : children) {
      if (child.add_child(parent, child_cls)) {
        return true;
      }
    }

    return false;
  };

  void add_method(Symbol name, Symbol *type) {
    type_env.M.addid(name, type);

    for (auto &child : children) {
      child.add_method(name, type);
    }
  }

  Symbol *get_type_of_method(Symbol name) { return type_env.M.lookup(name); }

  void add_attribute(Symbol name, Symbol *type) {
    type_env.O.addid(name, type);

    for (auto &child : children) {
      child.add_attribute(name, type);
    }
  }

  void add_variable(Symbol name, Symbol *type) { type_env.O.addid(name, type); }

  Symbol *get_type_of_object(Symbol name) { return type_env.O.lookup(name); }

  InheritanceGraphNode *get_node(Symbol name) {
    if (cls->get_name()->equal_string(name->get_string(), name->get_len())) {
      return this;
    }

    for (auto &child : children) {
      InheritanceGraphNode *child_class = child.get_node(name);
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

  InheritanceGraphNode *current_class;

public:
  void set_base_class(Class_ cls) { base_class = InheritanceGraphNode(cls); };

  inline void set_current_class(Symbol name) {
    current_class = base_class.get_node(name);
    current_class->type_env.C.addid(SELF_TYPE, &name);
  }

  Class_ get_current_class() { return current_class->cls; }

  bool add_child(Symbol parent, Class_ child_cls) {
    return base_class.add_child(parent, child_cls);
  };

  void add_method(Symbol name, Symbol *type) {
    current_class->add_method(name, type);
  }

  Symbol *get_type_of_method(Symbol name) {
    return current_class->get_type_of_method(name);
  }

  void add_attribute(Symbol name, Symbol *type) {
    current_class->add_attribute(name, type);
  }

  void add_variable(Symbol name, Symbol *type) {
    current_class->add_variable(name, type);
  }

  Symbol *get_type_of_object(Symbol name) {
    return current_class->get_type_of_object(name);
  }

  void enterscope() { current_class->type_env.O.enterscope(); }

  void exitscope() { current_class->type_env.O.exitscope(); }

  bool exists(Class_ cls) { return base_class.exists(cls); }
};

class ClassTable {
private:
  int semant_errors;
  void install_basic_classes();
  void install_user_defined_classes_first_pass(
      Classes classes, Classes &possibly_forward_inheriting_classes);
  void install_user_defined_classes_second_pass(Classes classes);
  inline bool check_illegal_inheritance(Class_ cls);

  ostream &error_stream;
  InheritanceGraph inheritance_graph;

public:
  ClassTable(Classes);
  int errors() { return semant_errors; }
  ostream &semant_error();
  ostream &semant_error(Class_ c);
  ostream &semant_error(Symbol filename, tree_node *t);

  void set_current_class(Symbol name) {
    inheritance_graph.set_current_class(name);
  }

  Class_ get_current_class() { return inheritance_graph.get_current_class(); }

  void add_method(Symbol name, Symbol *type) {
    inheritance_graph.add_method(name, type);
  }

  Symbol *get_type_of_method(Symbol name) {
    return inheritance_graph.get_type_of_method(name);
  }

  void add_attribute(Symbol name, Symbol *type) {
    inheritance_graph.add_attribute(name, type);
  }

  void add_variable(Symbol name, Symbol *type) {
    inheritance_graph.add_variable(name, type);
  }

  Symbol *get_type_of_object(Symbol name) {
    return inheritance_graph.get_type_of_object(name);
  }

  void enterscope() { inheritance_graph.enterscope(); }

  void exitscope() { inheritance_graph.exitscope(); }
};

#endif
