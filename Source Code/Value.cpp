//
//  Value.cpp
//  MSDLang
//
//  Created by Samuel Bauter on 1/31/20.
//  Copyright © 2020 U of U. All rights reserved.
//

#include <stdexcept>
#include "catch.hpp"
#include "Value.hpp"
#include "Step.hpp"
#include "Expr.h"
#include "Env.hpp"
#include "Cont.hpp"

void Val::not_a(std::string what) {
    val_not_a(THIS, what);
}

void Val::val_not_a(PTR(Val) val,std::string what) {
    throw std::runtime_error("not a " + what + ": " + val->to_string());
}


PTR(Val) Val::add_to(PTR(Val) other_val){
    not_a("number");
}

PTR(Val) Val::sub_with(PTR(Val) other_val){
    not_a("number");
}

PTR(Val) Val::mult_with(PTR(Val) other_val){
    not_a("number");
}

PTR(Val) Val::div_with(PTR(Val) other_val){
    not_a("number");
}

PTR(Val) Val::mod_with(PTR(Val) other_val){
    not_a("number");
}

bool Val::is_true(){
    not_a("boolean");
}

PTR(Val) Val::call(PTR(Val) actual_arg){
    not_a("function");
}

void Val::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest){
    not_a("function");
}

NumVal::NumVal(int rep) {
  this->rep = rep;
}

int NumVal::other_rep(PTR(Val) other_val){
    PTR(NumVal) other_num_val = CAST(NumVal)(other_val);
    if(other_num_val == nullptr)
        val_not_a(other_val, "number");
    else
        return other_num_val->rep;
}


bool NumVal::equals(PTR(Val) other_val) {
  PTR(NumVal) other_num_val = CAST(NumVal)(other_val);
  if (other_num_val == nullptr)
    return false;
  else
    return rep == other_num_val->rep;
}

PTR(Val) NumVal::add_to(PTR(Val) other_val) {
        return NEW(NumVal)((unsigned)rep + (unsigned)other_rep(other_val));
}

PTR(Val) NumVal::sub_with(PTR(Val) other_val) {
        return NEW(NumVal)((unsigned)rep - (unsigned)other_rep(other_val));
}

PTR(Val) NumVal::mult_with(PTR(Val) other_val) {
  return NEW(NumVal)((unsigned)rep * (unsigned)other_rep(other_val));
}

PTR(Val) NumVal::div_with(PTR(Val) other_val) {
  return NEW(NumVal)(rep / other_rep(other_val));
}

PTR(Val) NumVal::mod_with(PTR(Val) other_val) {
  return NEW(NumVal)((unsigned)rep % (unsigned)other_rep(other_val));
}

PTR(Expr) NumVal::to_expr() {
  return NEW(NumExpr)(rep);
}

std::string NumVal::to_string() {
  return std::to_string(rep);
}







BoolVal::BoolVal(bool rep) {
  this->rep = rep;
}

bool BoolVal::equals(PTR(Val) other_val) {
  PTR(BoolVal) other_bool_val = CAST(BoolVal)(other_val);
  if (other_bool_val == nullptr)
    return false;
  else
    return rep == other_bool_val->rep;
}

PTR(Expr) BoolVal::to_expr() {
  return NEW(BoolExpr)(rep);
}

std::string BoolVal::to_string() {
  if (rep)
    return "_true";
  else
    return "_false";
}

bool BoolVal::is_true(){
    return this->rep==true;
}



FunVal::FunVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env){
    this->formal_arg=formal_arg;
    this->body=body;
    this->env = env;
}
bool FunVal::equals(PTR(Val) other_val){
    PTR(FunVal) other_fun_val = CAST(FunVal)(other_val);
    if (other_fun_val == nullptr)
        return false;
    else
        return formal_arg == other_fun_val->formal_arg && body->equals(other_fun_val->body);
}



PTR(Expr) FunVal::to_expr(){
    return NEW(FunExpr)(formal_arg,body);
}
std::string FunVal:: to_string(){
    return "_fun ("+formal_arg + ") "+body->to_string();
}



PTR(Val) FunVal::call(PTR(Val) actual_arg){
    return body->interp(NEW(ExtendedEnv)(formal_arg, actual_arg, env));
}

void FunVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest){
    Step::mode =Step::interp_mode;
    Step::expr = body;
    Step::env = NEW(ExtendedEnv)(formal_arg, actual_arg_val, env);
    Step::cont = rest;
};






