//
//  Cont.cpp
//  MSDLang
//
//  Created by Samuel Bauter on 3/25/20.
//  Copyright © 2020 U of U. All rights reserved.
//

#include "Cont.hpp"
#include "Expr.h"
#include "Env.hpp"
#include "Cont.hpp"
#include "Step.hpp"
#include "Value.hpp"
#include "op.hpp"


PTR(Cont) Cont::done = NEW(DoneCont)();


DoneCont::DoneCont(){
};


void DoneCont::step_continue() {
  throw std::runtime_error("can't continue done");
}

RightThenOpCont::RightThenOpCont(PTR(BinaryOp) op, PTR(Expr) rhs, PTR(Env) env, PTR(Cont) rest){
    this->op = op;
    this->rhs = rhs;
    this->env = env;
    this->rest = rest;
}

void RightThenOpCont::step_continue(){
   Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::env = env;
    Step::cont = NEW(OpCont)(op, Step::val, rest);
}

OpCont::OpCont(PTR(BinaryOp) op, PTR(Val) lhs_val, PTR(Cont) rest){
    this->op = op;
    this->lhs_val = lhs_val;
    this->rest = rest;
}

void OpCont::step_continue(){
    Step::mode = Step::continue_mode;
    Step::val = op->perform(lhs_val, Step::val);
    Step::cont = rest;
}



IfBranchCont::IfBranchCont(PTR(Expr) then_part,PTR(Expr) else_part, PTR(Env) env, PTR(Cont) rest){
    this->then_part = then_part;
    this->else_part = else_part;
    this->env = env;
    this->rest = rest;
}
void IfBranchCont::step_continue(){
    PTR(Val) test_val = Step::val;
    Step::mode = Step::interp_mode;
    if(test_val->is_true())
        Step::expr = then_part;
    else
        Step::expr = else_part;
    
    Step::env = env;
    Step::cont = rest;
    
}

LetBodyCont::LetBodyCont(std::string varName, PTR(Expr) body, PTR(Env) env, PTR(Cont) rest){
    this->varName = varName;
    this->body=body;
    this->env = env;
    this->rest = rest;
}

void LetBodyCont::step_continue(){
    Step::mode = Step::interp_mode;
    Step::expr = body;
    Step::env = NEW(ExtendedEnv)(varName,Step::val,env);
    Step::cont = rest;
}


CallCont::CallCont(PTR(Val) to_be_called_val , PTR(Cont) rest){
    this->to_be_called_val = to_be_called_val;
    this->rest = rest;
}

void CallCont::step_continue(){
    to_be_called_val->call_step(Step::val, rest);
}


ArgThenCallCont::ArgThenCallCont(PTR(Expr) actual_arg, PTR(Env) env, PTR(Cont) rest){
    this->actual_arg = actual_arg;
    this->env = env;
    this->rest = rest;
}

void ArgThenCallCont::step_continue(){
    Step::mode = Step::interp_mode;
    Step::expr = actual_arg;
    Step::env = env;
    Step::cont = NEW(CallCont)(Step::val, rest);
}











