//
//  op.cpp
//  MSDLang
//
//  Created by Samuel Bauter on 4/9/20.
//  Copyright © 2020 U of U. All rights reserved.
//

#include "op.hpp"
#include "Value.hpp"
#include <iostream>


bool BinaryOp::equals(PTR(BinaryOp) other_op){
    return other_op == THIS;
}


AddOp::AddOp() { }

PTR(BinaryOp) AddOp::get_instance() {
  static PTR(AddOp) the_op;
  if (the_op == nullptr)
    the_op = NEW(AddOp)();
  return the_op;
}

PTR(Val) AddOp::perform(PTR(Val) lhs_val, PTR(Val) rhs_val) {
  return lhs_val->add_to(rhs_val);
}

std::string AddOp::to_string() {
  return "+";
}

SubOp::SubOp() { }

PTR(BinaryOp) SubOp::get_instance() {
  static PTR(SubOp) the_op;
  if (the_op == nullptr)
    the_op = NEW(SubOp)();
  return the_op;
}

PTR(Val) SubOp::perform(PTR(Val) lhs_val, PTR(Val) rhs_val) {
  return lhs_val->sub_with(rhs_val);
}

std::string SubOp::to_string() {
  return "-";
}


MultOp::MultOp() { }

PTR(BinaryOp) MultOp::get_instance() {
  static PTR(MultOp) the_op;
  if (the_op == nullptr)
    the_op = NEW(MultOp)();
  return the_op;
}

PTR(Val) MultOp::perform(PTR(Val) lhs_val, PTR(Val) rhs_val) {
  return lhs_val->mult_with(rhs_val);
}

std::string MultOp::to_string() {
  return "*";
}

DivOp::DivOp() { }

PTR(BinaryOp) DivOp::get_instance() {
  static PTR(DivOp) the_op;
  if (the_op == nullptr)
    the_op = NEW(DivOp)();
  return the_op;
}

PTR(Val) DivOp::perform(PTR(Val) lhs_val, PTR(Val) rhs_val) {
  return lhs_val->div_with(rhs_val);
}

std::string DivOp::to_string() {
  return "/";
}

ModOp::ModOp() { }

PTR(BinaryOp) ModOp::get_instance() {
  static PTR(ModOp) the_op;
  if (the_op == nullptr)
    the_op = NEW(ModOp)();
  return the_op;
}

PTR(Val) ModOp::perform(PTR(Val) lhs_val, PTR(Val) rhs_val) {
  return lhs_val->mod_with(rhs_val);
}

std::string ModOp::to_string() {
  return "%";
}





CompOp::CompOp() { }

PTR(BinaryOp) CompOp::get_instance() {
  static PTR(CompOp) the_op;
  if (the_op == nullptr)
    the_op = NEW(CompOp)();
  return the_op;
}

PTR(Val) CompOp::perform(PTR(Val) lhs_val, PTR(Val) rhs_val) {
  return  NEW(BoolVal)(lhs_val->equals(rhs_val));
}


std::string CompOp::to_string() {
  return "==";
}

CompNotOp::CompNotOp() { }

PTR(BinaryOp) CompNotOp::get_instance() {
  static PTR(CompNotOp) the_op;
  if (the_op == nullptr)
    the_op = NEW(CompNotOp)();
  return the_op;
}

PTR(Val) CompNotOp::perform(PTR(Val) lhs_val, PTR(Val) rhs_val) {
  return  NEW(BoolVal)(!lhs_val->equals(rhs_val));
}

std::string CompNotOp::to_string() {
  return "!=";
}




static std::vector<PTR(BinaryOp)> additive_table{
    AddOp::get_instance(),SubOp::get_instance()
};

static std::vector<PTR(BinaryOp)> multiplicative_table{
  MultOp::get_instance(), DivOp::get_instance(), ModOp::get_instance()
};

static std::vector<PTR(BinaryOp)> equality_table{
  CompOp::get_instance(),CompNotOp::get_instance()
};

std::vector<std::vector<PTR(BinaryOp)>> op_tables {
  equality_table,
  additive_table,
  multiplicative_table
};
