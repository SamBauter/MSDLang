//
// Created by Samuel Bauter on 1/15/20.
//
#include <sstream>
#include <iostream>
#include "Expr.h"
#include "ExprParser.hpp"
//#include "catch.hpp"
#include "Step.hpp"
#include "Cont.hpp"
#include "Value.hpp"
#include "Env.hpp"
#include "op.hpp"





LiteralExpr::LiteralExpr(PTR(Val) val){
    this->val = val;
};

bool LiteralExpr::equals(PTR(Expr)other_expr){
    PTR(LiteralExpr) other_literal = CAST(LiteralExpr)(other_expr);
    if (other_literal == nullptr)
      return false;
    else
        return this->val->equals(other_literal->val);
}

std::string LiteralExpr::to_string(){
    return val->to_string();
}

PTR(Val) LiteralExpr::interp(PTR(Env) env) {
    return val;
}

PTR(Expr) LiteralExpr::optimize() {
  return NEW(LiteralExpr)(val);
}

bool LiteralExpr::containsVar(){
    return false;
}

void LiteralExpr::step_interp(){
    Step::mode = Step::continue_mode;
    Step::val = this->val;
    Step::cont = Step::cont;
}

PTR(Expr) LiteralExpr::subst(std::string str, PTR(Val) substExpr) {
    return this->val->to_expr();
}

BoolExpr::BoolExpr(bool rep)
: LiteralExpr(NEW(BoolVal)(rep))
{
}

NumExpr::NumExpr(int rep)
: LiteralExpr(NEW(NumVal)(rep))
{
}

BinaryOpExpr::BinaryOpExpr(PTR(BinaryOp) op, PTR(Expr) lhs, PTR(Expr) rhs){
    this->op = op;
    this->lhs = lhs;
    this->rhs = rhs;
}

bool BinaryOpExpr::containsVar(){
    return lhs->containsVar() || rhs->containsVar();
}


PTR(Expr) BinaryOpExpr::subst(std::string varName, PTR(Val) new_val){
    return NEW(BinaryOpExpr)(op,
                             lhs->subst(varName, new_val),
                             rhs->subst(varName,new_val));
}

PTR(Expr) BinaryOpExpr::optimize(){
    PTR(Expr) new_lhs = lhs->optimize();
    PTR(Expr) new_rhs = rhs->optimize();
    PTR(Expr) new_expr = NEW(BinaryOpExpr)(op,new_lhs,new_rhs);
    if (new_lhs->containsVar() || new_rhs->containsVar())
        return new_expr;
    else
        return new_expr->interp(Env::empty)->to_expr();
}



PTR(Val) BinaryOpExpr::interp(PTR(Env) env){
    return op->perform(lhs->interp(env), rhs->interp(env));
}




void BinaryOpExpr::step_interp(){
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::env = Step::env;
    Step::cont = NEW(RightThenOpCont)(op, rhs, Step::env, Step::cont);
}

std::string BinaryOpExpr::to_string(){
    return ("(" + lhs->to_string() + op->to_string() + rhs->to_string() + ")");
}


bool BinaryOpExpr::equals(PTR(Expr) other_expr){
    PTR(BinaryOpExpr) other_op = CAST(BinaryOpExpr)(other_expr);
    if(other_op == NULL)
        return false;
    else
        return (op->equals(other_op->op)
                && lhs->equals(other_op->lhs)
                && rhs->equals(other_op->rhs));
}

AddExpr::AddExpr(PTR(Expr) lhs, PTR(Expr) rhs)
: BinaryOpExpr(AddOp::get_instance(), lhs, rhs)
{
}

MultExpr::MultExpr(PTR(Expr) lhs, PTR(Expr) rhs)
: BinaryOpExpr(MultOp::get_instance(), lhs, rhs)
{
}

CompExpr::CompExpr(PTR(Expr) lhs, PTR(Expr) rhs)
: BinaryOpExpr(CompOp::get_instance(), lhs, rhs)
{
}




VarExpr::VarExpr(std::string name) {
    this->name = name;
}


bool VarExpr::equals(PTR(Expr) other_expr) {
    PTR(VarExpr) other_var_expr = CAST(VarExpr)(other_expr);
    if (other_var_expr == nullptr)
        return false;
    else return name.compare(other_var_expr->name) == 0;
}


PTR(Val) VarExpr::interp(PTR(Env) env) {
    return env->lookup(name);
}

void VarExpr::step_interp(){
    Step::mode = Step::continue_mode;
    Step::val = Step::env->lookup(name);
    Step::cont = Step::cont;
}



PTR(Expr) VarExpr::optimize(){
    return NEW(VarExpr)(name);
}



bool VarExpr::containsVar() {
    return true;
}

PTR(Expr) VarExpr::subst(std::string varName, PTR(Val) new_val){
    if(this->name==varName){
        return new_val->to_expr();
    }
    return NEW(VarExpr)(name);
}

std::string VarExpr::to_string(){
    return this->name;
}

LetExpr::LetExpr(std::string varName,PTR(Expr) rhs, PTR(Expr) body){
    this->varName = varName;
    this->rhs = rhs;
    this->body = body;
}

bool LetExpr::equals(PTR(Expr) other_expr){
    PTR(LetExpr) other_let_expr = CAST(LetExpr)(other_expr);
    if (other_let_expr == nullptr)
            return false;
    return(this->varName ==other_let_expr->varName&&
       this->rhs->equals(other_let_expr->rhs)&&
           this->body->equals(other_let_expr->body));
}




PTR(Val) LetExpr::interp(PTR(Env) env){
    PTR(Val) rhs_val = rhs->interp(env);
    PTR(Env) new_env = NEW(ExtendedEnv)(varName, rhs_val, env);
    return body->interp(new_env);
}

void LetExpr::step_interp(){
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::env = Step::env;
    Step::cont = NEW(LetBodyCont)(varName,body,Step::env, Step::cont);
}



PTR(Expr) LetExpr::optimize(){
    PTR(Expr) optimizedRhs = rhs->optimize();
    PTR(Expr) optimizedLetExpr = NEW(LetExpr)(varName,rhs->optimize(),body->optimize());
    if(this->rhs->containsVar()){
        return optimizedLetExpr;
    }else if(CAST(LiteralExpr)(optimizedRhs)==nullptr){
    return optimizedLetExpr;
    }else{
        PTR(Env) emptyEnv = NEW(EmptyEnv)();
        return body->subst(varName,rhs->interp(emptyEnv))->optimize();
    }
}



//Only gets called on optimized things.
bool LetExpr::containsVar(){
    return rhs->containsVar()||body->containsVar();
}



PTR(Expr) LetExpr::subst(std::string newVarName, PTR(Val) new_val){
    if(newVarName==this->varName){
        PTR(LetExpr) newLetExpr = NEW(LetExpr)(varName,rhs->subst(varName,new_val),body);
        return newLetExpr;
    }else{
        PTR(LetExpr) newLetExpr =NEW(LetExpr)(varName,rhs->subst(newVarName, new_val),body->subst(newVarName, new_val));
        return newLetExpr;
    }
}



    
    

std::string LetExpr::to_string(){
    return "_let " + varName + " = " + rhs->to_string() + " _in " + body->to_string();
    }

IfExpr::IfExpr(PTR(Expr) condition, PTR(Expr) trueConseq, PTR(Expr) falseConseq){
    this->condition = condition;
    this->trueConseq = trueConseq;
    this->falseConseq = falseConseq;
}

bool IfExpr::equals(PTR(Expr) other_expr){
    PTR(IfExpr) other_if_expr = CAST(IfExpr)(other_expr);
        if (other_if_expr == nullptr)
                return false;
        return(condition->equals(other_if_expr->condition)&&
           trueConseq->equals(other_if_expr->trueConseq)&&
               falseConseq->equals(other_if_expr->falseConseq));
    }
std::string IfExpr::to_string(){
    return "_if "+condition->to_string() + " _then " +trueConseq->to_string() +" _else "+falseConseq->to_string();
}




PTR(Expr) IfExpr::subst(std::string newVarName, PTR(Val) new_val){
    return NEW(IfExpr)(condition->subst(newVarName, new_val),
                      trueConseq->subst(newVarName, new_val),
                      falseConseq->subst(newVarName, new_val));
}

PTR(Val) IfExpr::interp(PTR(Env) env) {
    if(condition->interp(env)->is_true()){
        return trueConseq->interp(env);
    }else if(!condition->interp(env)->is_true()){
        return falseConseq->interp(env);
    }else{
        throw std::runtime_error("if condition should have failed interp at this point");
    }
}

void IfExpr::step_interp(){
    Step::mode = Step::interp_mode;
    Step::expr = condition;
    Step::env = Step::env;
    Step::cont = NEW(IfBranchCont)(trueConseq,falseConseq,Step::env, Step::cont);
    
}

PTR(Expr) IfExpr::optimize(){
    PTR(IfExpr) optimizedIfExpr = NEW(IfExpr)(condition->optimize(),trueConseq->optimize(),falseConseq->optimize());
    if(optimizedIfExpr->condition->containsVar()){
        return optimizedIfExpr;
    }else if(optimizedIfExpr->condition->equals(NEW(BoolExpr)(true))){
        return optimizedIfExpr->trueConseq;
    }else if(optimizedIfExpr->condition->equals(NEW(BoolExpr)(false))){
        return optimizedIfExpr->falseConseq;
    }else{
        PTR(BoolExpr) conditionChecker = CAST(BoolExpr)(optimizedIfExpr->condition);
        if(conditionChecker==nullptr){
            return optimizedIfExpr;
        }else{
            throw std::runtime_error("Boolean Expr in the condition space should have already the If Expr to a different Expr");
        }
    }
}


bool IfExpr::containsVar(){
    return trueConseq->containsVar()||falseConseq->containsVar();
}








FunExpr::FunExpr(std::string formal_arg, PTR(Expr)body){
    this->formal_arg = formal_arg;
    this->body = body;
}


bool FunExpr::equals(PTR(Expr) other_expr){
    PTR(FunExpr) other_fun_expr = CAST(FunExpr)(other_expr);
        if (other_fun_expr == nullptr)
                return false;
        return(formal_arg==other_fun_expr->formal_arg)&&
           body->equals(other_fun_expr->body);
}



std::string FunExpr::to_string(){
    return "_fun ("+formal_arg + ") "+body->to_string();
}
bool FunExpr::containsVar(){
    return body->containsVar();
}



PTR(Expr) FunExpr::subst(std::string varName, PTR(Val) newVal){
    if(this->formal_arg==varName){
        return NEW(FunExpr)(this->formal_arg,this->body);
    }else{
        return NEW(FunExpr)(this->formal_arg,this->body->subst(varName, newVal));
    }
}


PTR(Val) FunExpr::interp(PTR(Env) env){
    return NEW(FunVal)(formal_arg,body,env);
    }
    
void FunExpr::step_interp(){
    Step::mode = Step::continue_mode;
    Step::val = NEW(FunVal)(formal_arg,body,Step::env);
    Step::cont = Step::cont;
}




PTR(Expr) FunExpr:: optimize(){
    return NEW(FunExpr)(this->formal_arg,this->body->optimize());
}



CallExpr::CallExpr(PTR(Expr) to_be_called, PTR(Expr) actual_arg){
    this->to_be_called=to_be_called;
    this->actual_arg=actual_arg;
}

bool CallExpr::equals(PTR(Expr) other_expr){
    PTR(CallExpr) other_call_expr = CAST(CallExpr)(other_expr);
    if (other_call_expr == nullptr)
            return false;
    return(to_be_called->equals(other_call_expr->to_be_called)&&
           actual_arg->equals(other_call_expr->actual_arg));
}


std::string CallExpr::to_string(){
    return "( " +to_be_called->to_string() + " ("+ actual_arg->to_string()+")"+" )";
}



bool CallExpr::containsVar(){
    return to_be_called->containsVar()||actual_arg->containsVar();
}

           
PTR(Val) CallExpr::interp(PTR(Env) env){
    PTR(Val) to_be_called_val = to_be_called->interp(env);
    PTR(Val) actual_arg_val = actual_arg->interp(env);

        return to_be_called_val->call(actual_arg_val);
}

void CallExpr::step_interp(){
    Step::mode = Step::interp_mode;
    Step::expr = to_be_called;
    Step::cont = NEW(ArgThenCallCont)(actual_arg, Step::env, Step::cont);
}





PTR(Expr) CallExpr::subst(std::string varName, PTR(Val) newVal){
    PTR(CallExpr) substCallExpr = NEW(CallExpr)(this->to_be_called->subst(varName, newVal),this->actual_arg->subst(varName, newVal));
    
    return substCallExpr;
}




PTR(Expr) CallExpr::optimize(){
    return NEW(CallExpr)(to_be_called->optimize(), actual_arg->optimize());
}


/*
TEST_CASE("Variables: subst()"){
    CHECK(VarExpr("x").subst("x",NEW(NumVal)( 10))->equals(NEW(NumExpr)(10)));
    CHECK(VarExpr("dog").subst("dog",NEW(NumVal)( 20))->equals(NEW(NumExpr)(20)));
    CHECK(VarExpr("cat").subst("dog", NEW(NumVal)( 10))->equals(NEW(VarExpr)("cat")));

}




TEST_CASE("equals Number") {
    CHECK((NEW(NumExpr)(1))->equals(NEW(NumExpr)(1)));
    CHECK(!(NEW(NumExpr)(1))->equals(NEW(NumExpr)(2)));
    CHECK(!(NEW(NumExpr)(1))->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(4))));

}






TEST_CASE("Add containsVar() tests") {
    CHECK(AddExpr(NEW(NumExpr)(3), NEW(NumExpr)(4)).containsVar() == false);
    CHECK(AddExpr(NEW(NumExpr)(3), NEW(VarExpr)("x")).containsVar() == true);
    CHECK(AddExpr(NEW(VarExpr)("x"), NEW(NumExpr)(3)).containsVar() == true);
    CHECK(AddExpr(NEW(VarExpr)("x"), NEW(VarExpr)("x")).containsVar() == true);


}

TEST_CASE("Mult containsVar() tests") {
    CHECK(MultExpr(NEW(NumExpr)(3), NEW(NumExpr)(4)).containsVar() == false);
    CHECK(MultExpr(NEW(NumExpr)(3), NEW(VarExpr)("x")).containsVar() == true);
    CHECK(MultExpr(NEW(VarExpr)("x"), NEW(NumExpr)(3)).containsVar() == true);
    CHECK(MultExpr(NEW(VarExpr)("x"), NEW(VarExpr)("x")).containsVar() == true);
}

TEST_CASE("Var containsVar() tests") {
    CHECK(VarExpr("x").containsVar() == true);
}


TEST_CASE("Harness Generated Failures") {
    //Had to fix the interp part of Optimize for MULT AND ADD to check for bools
    CHECK_THROWS(runOpt("_true * 5")=="(_true * 5)");
    CHECK_THROWS(runOpt("_false + _false * 739586036 == _false")=="_false");
    CHECK_THROWS(runOpt("_if 2057750526 _then 419251295 _else 1802082212 + _fun ( SZUdYnFSiENvThttQjtzdGhc ) 861174277")=="_if 2057750526 _then 419251295 _else 1802082212 + _fun (SZUdYnFSiENvThttQjtzdGhc) 861174277");
    //Had to fix the interp part of Optimize for rhs of Let Exprs to check for Bools in addition to vars
    CHECK(runOpt("_let OxbrRWeExzGvErdLU = _if 1219341124 _then _true _else 789026548 _in HKBecgdxpH")=="_let OxbrRWeExzGvErdLU = _if 1219341124 _then _true _else 789026548 _in HKBecgdxpH");
    CHECK_THROWS(runOpt("_let vUqWN = 1596458983 + _false _in _fun ( ayKXCaBRlxaEmDwJrJTixoC ) RmpLkXX")=="_let vUqWN = 1596458983 + _false _in _fun (ayKXCaBRlxaEmDwJrJTixoC) RmpLkXX");
    CHECK(runOpt("_let vXYvMRHZCUIcv = _false ( 1184395709 )  _in 1821317764")=="_let vXYvMRHZCUIcv = ( _false (1184395709) ) _in 1821317764");
    CHECK(runOpt("_let OxbrRWeExzGvErdLU = _if 1219341124 _then _true _else 789026548 _in HKBecgdxpH")=="_let OxbrRWeExzGvErdLU = _if 1219341124 _then _true _else 789026548 _in HKBecgdxpH");
}

TEST_CASE("Env Test Cases"){
    PTR(EmptyEnv) emptyenv = NEW(EmptyEnv)();
    PTR(NumVal) five = NEW(NumVal)(5);
    PTR(NumVal) ten = NEW(NumVal)(10);
    PTR(BoolVal) trueVal = NEW(BoolVal)(true);


    PTR(ExtendedEnv) XtoFive = NEW(ExtendedEnv)("x",five,emptyenv);
    PTR(ExtendedEnv) YtoTen = NEW(ExtendedEnv)("y",ten,XtoFive);
    PTR(ExtendedEnv) XtoTrue = NEW(ExtendedEnv)("x",trueVal,emptyenv);
    PTR(FunVal) funVal = NEW(FunVal)("x",parse_str("x+20"),emptyenv);
    PTR(ExtendedEnv) funEnv = NEW(ExtendedEnv)("f",funVal,emptyenv);
    
    
    CHECK(runInterpWithEnv("x+5", XtoFive)=="10");
    CHECK(runInterpWithEnv("x+y+5", YtoTen) == "20");
    CHECK(runInterpWithEnv("_let x=2+y _in x ", YtoTen)=="12");
    CHECK(runInterpWithEnv("_if x _then 1 _else 2", XtoTrue)=="1");
    CHECK(runInterpWithEnv("_let f = _fun(x) 2+x _in f(10)",YtoTen)=="12");
    CHECK(runInterpWithEnv("f(10)", funEnv) == "30");
    

}

TEST_CASE("Num Opt"){
    CHECK(runOpt("5")=="5");
}

TEST_CASE("Number:toString"){
    CHECK(NumExpr(5).to_string()=="5");
}



TEST_CASE("Number::value") {
    CHECK(NumExpr(1).interp(NEW(EmptyEnv)())->equals(NEW(NumVal)( 1)));
    CHECK(NumExpr(0).interp(NEW(EmptyEnv)())->equals(NEW(NumVal)( 0)));
    CHECK(NumExpr(100).interp(NEW(EmptyEnv)())->equals(NEW(NumVal)( 100)));
}

TEST_CASE("Number::containsVar") {
    CHECK(NumExpr(1).containsVar() == false);
}


TEST_CASE("Step interp"){
    CHECK(runStepInterp("5")=="5");
}

TEST_CASE("Num interp"){
    CHECK(runInterp("5")=="5");
}


TEST_CASE("Bool Expr Equals"){
    CHECK(parse_str("_true")->equals(parse_str("_true")));
    CHECK(parse_str("_true")->equals(parse_str("1"))==false);
}

TEST_CASE("Bool Expr Subst"){
    CHECK(BoolExpr(true).subst("x",NEW(NumVal)(5))->equals(NEW(BoolExpr)(true)));
}


TEST_CASE("CompNotOps interp"){
    CHECK(runInterp("1!=0")=="_true");
    CHECK(runInterp("_let x=5 _in x+x != 11")=="_true");
    CHECK(runInterp("_let x=5 _in x+x != 10")=="_false");
}

TEST_CASE("Div interps"){
    CHECK(runInterp("5/3")=="1");
}
TEST_CASE("Mod interps"){
    CHECK(runInterp("5%3")=="2");
}

TEST_CASE("Sub interps"){
    CHECK(runInterp("2-5") == "-3");
}

TEST_CASE("BinaryOpExpr optimize tests"){
    CHECK(runOpt("5+5")=="10");
    
}

TEST_CASE("Add::equals()"){
    CHECK(AddExpr(NEW(NumExpr)(3),NEW(NumExpr)(4)).equals(NEW(VarExpr)("x"))==false);
}

TEST_CASE("Add interp"){
    CHECK(runInterp("5+5")=="10");
    CHECK(runInterp("5+5+5")=="15");
}

TEST_CASE("Add Opt"){
    CHECK(runOpt("5+3")=="8");
    CHECK(runOpt("5+x")=="(5+x)");
    CHECK(runOpt("5+x+5")=="(5+(x+5))");
}

TEST_CASE("Add subst() tests"){
    CHECK(AddExpr(NEW(NumExpr)(1),NEW(NumExpr)(2)).subst("x",NEW(NumVal)( 10))->equals(NEW(AddExpr)(NEW(NumExpr)(1),NEW(NumExpr)(2))));
    CHECK(AddExpr(NEW(VarExpr)("x"),NEW(NumExpr)(2)).subst("x",NEW(NumVal)( 10))->equals(NEW(AddExpr)(NEW(NumExpr)(10),NEW(NumExpr)(2))));
    CHECK(AddExpr(NEW(VarExpr)("x"),NEW(AddExpr)(NEW(VarExpr)("x"),NEW(NumExpr)(2))).subst("x",NEW(NumVal)( 10))->equals(NEW(AddExpr)(NEW(NumExpr)(10),
            NEW(AddExpr)(NEW(NumExpr)(10),NEW(NumExpr)(2)))));
}

TEST_CASE("Add value() tests") {
    CHECK(AddExpr(NEW(NumExpr)(3), NEW(NumExpr)(4)).interp(NEW(EmptyEnv)())->equals(NEW(NumVal)( 7)));
    CHECK(AddExpr(NEW(NumExpr)(4), NEW(AddExpr)(NEW(NumExpr)(4), NEW(NumExpr)(4))).interp(NEW(EmptyEnv)()) ->equals(NEW(NumVal)( 12)));
    CHECK(AddExpr(NEW(AddExpr)(NEW(NumExpr)(4), NEW(NumExpr)(4)), NEW(AddExpr)(NEW(NumExpr)(4), NEW(NumExpr)(4))).interp(NEW(EmptyEnv)())->equals(NEW(NumVal)( 16)));
}

TEST_CASE("Mult Equals Null"){
    CHECK(MultExpr(NEW(NumExpr)(5),NEW(NumExpr)(4)).equals(NEW(NumExpr)(3))==false);
}

TEST_CASE("Mult interp"){
    CHECK(runInterp("5*5")=="25");
    CHECK(runInterp("5*5*5")=="125");
}

TEST_CASE("Mult Optimize"){
    CHECK(runOpt("x*5*5")=="(x*25)");
    CHECK(runOpt("5*5*x")=="(5*(5*x))");
    CHECK(runOpt("5*5*5")=="125");
    
}

TEST_CASE("Mult::exprToString"){
    CHECK(MultExpr(NEW(VarExpr)("x"),NEW(VarExpr)("y")).to_string()=="(x*y)");
    CHECK(MultExpr(NEW(NumExpr)(5),NEW(NumExpr)(4)).to_string()=="(5*4)");
    
}

TEST_CASE("Mult subst() tests"){
    CHECK(MultExpr(NEW(NumExpr)(1),NEW(NumExpr)(2)).subst("x",NEW(NumVal)( 10))->equals(NEW(MultExpr)(NEW(NumExpr)(1),NEW(NumExpr)(2))));
    CHECK(MultExpr(NEW(VarExpr)("x"),NEW(NumExpr)(2)).subst("x",NEW(NumVal)( 10))->equals(NEW(MultExpr)(NEW(NumExpr)(10),NEW(NumExpr)(2))));
    CHECK(MultExpr(NEW(VarExpr)("y"),NEW(VarExpr)("y")).subst("y", NEW(NumVal)( 10))->
          equals(NEW(MultExpr)(NEW(NumExpr)(10),NEW(NumExpr)(10))));
}

TEST_CASE("Mult value()") {
    CHECK(MultExpr(NEW(NumExpr)(3), NEW(NumExpr)(4)).interp(NEW(EmptyEnv)())->equals(NEW(NumVal)( 12)));
    CHECK(MultExpr(NEW(NumExpr)(4), NEW(MultExpr)(NEW(NumExpr)(4), NEW(NumExpr)(4))).interp(NEW(EmptyEnv)()) ->equals(NEW(NumVal)( 64)));
    CHECK(MultExpr(NEW(MultExpr)(NEW(NumExpr)(4), NEW(NumExpr)(4)), NEW(MultExpr)(NEW(NumExpr)(4), NEW(NumExpr)(4))).interp(NEW(EmptyEnv)())->equals(NEW(NumVal)( 256)));
    CHECK(MultExpr(NEW(AddExpr)(NEW(NumExpr)(4), NEW(NumExpr)(4)), NEW(NumExpr)(4)).interp(NEW(EmptyEnv)())->equals(NEW(NumVal)( 32)));
}

TEST_CASE("Variable Equals"){
    CHECK(VarExpr("x").equals(NEW(NumExpr)(10))==false);
}

TEST_CASE("Var interp"){
    CHECK_THROWS(runInterp("x"));
}

TEST_CASE("Var optimize"){
    CHECK(runOpt("x")=="x");
}

TEST_CASE("Let Expr Equals"){
    CHECK(parse_str("_let x = 1 _in x")->equals(parse_str("_let x = 1 _in x")));
    CHECK(parse_str("_let x = 1 _in x")->equals(parse_str("x+1"))==false);

}


TEST_CASE("Let interp"){
    CHECK(runInterp("_let x=5 _in x+x")=="10");
    CHECK(runInterp("_let x=5 _in _let x=3 _in x+x")=="6");
    CHECK(runInterp("_let x=5 _in _let y=3 _in x+y")=="8");
    CHECK_THROWS(runInterp("_let x=5 _in y+y"));
    
}

TEST_CASE("Let optimize"){
    CHECK(runOpt("_let x=5 _in x+x")=="10");
    CHECK(runOpt("_let x=5 _in _let x=3 _in x+x")=="6");
    CHECK(runOpt("_let x=5 _in _let y=3 _in x+y")=="8");
    CHECK(runOpt("_let x=5 _in _let y=z+4 _in x+y+4")=="_let y = (z+4) _in (5+(y+4))");
    CHECK(runOpt("_let x=5 _in _let x=6 _in x")=="6");
    
}
TEST_CASE("Let Expr Contains Var"){
    CHECK(LetExpr("x",NEW(VarExpr)("x"),NEW(NumExpr)(2)).containsVar()==true);
    CHECK(LetExpr("x",NEW(VarExpr)("x"), NEW(NumExpr)(2)).containsVar()==true);
    CHECK(LetExpr("x",NEW(NumExpr)(2), NEW(NumExpr)(2)).containsVar()==false);

}
TEST_CASE("LetExpr NEsting?"){
    CHECK(runOpt("_let x=5 _in _let x=6 _in _let x=7 _in x ")=="7");
    CHECK(runOpt("_let y=3 _in 5+y")=="8");
}

TEST_CASE("IfExpr to string and equals"){
    CHECK(parse_str("_if 2 _then 3 _else 4")->to_string()=="_if 2 _then 3 _else 4");
    CHECK(parse_str("_if _true _then 1 _else 2")->equals(parse_str("1"))==false);
}

TEST_CASE("If Expr Subst"){
    CHECK(parse_str("_if x _then x _else y")->subst("x", NEW(NumVal)( 3))->equals(parse_str("_if 3 _then 3 _else y")));
}

TEST_CASE("If Expr Interp"){
    CHECK(runInterp("_if _true _then 1 _else 0")=="1");
    CHECK(runInterp("_if 5==5 _then 1 _else 0")=="1");
    CHECK(runInterp("_if (5)==(5) _then 1 _else 0")=="1");
    CHECK_THROWS(runInterp("_if x==x _then 1 _else 0"));
    
}

TEST_CASE("If Expr Optimize"){
    CHECK(runOpt("_if x==x _then 1 _else 2")=="_if (x==x) _then 1 _else 2");
    CHECK(runOpt("_if 1==1 _then 1 _else 2") == "1");
    CHECK(runOpt("_if _false _then 1 _else 2")=="2");
    CHECK(runOpt("_if _true _then 1 _else 2")=="1");
    CHECK(runOpt("_if 7 _then 1 _else 2")=="_if 7 _then 1 _else 2");
    
}

TEST_CASE("CompExpr interp"){
    CHECK(runInterp("2==1+1")=="_true");
    CHECK(runInterp("_true == 4")=="_false");
    CHECK_THROWS(runInterp("x==3"));
    CHECK_THROWS(runInterp("x+1 == x+1"));
    CHECK(runInterp("_let x=5 _in x+x") == "10");
    
}

TEST_CASE("CompExpr Optimize"){
    CHECK(runOpt("1==1")=="_true");
    CHECK(runOpt("x==x")=="(x==x)");
    CHECK(runOpt("1==2")=="_false");
    CHECK(runOpt("x+1==x+1")=="((x+1)==(x+1))");
}

TEST_CASE("CompExpr::containsVar"){
    CHECK(CompExpr(NEW(NumExpr)(5),NEW(NumExpr)(5)).containsVar()==false);
    CHECK(CompExpr(NEW(NumExpr)(5),NEW(VarExpr)("x")).containsVar()==true);
    CHECK(CompExpr(NEW(VarExpr)("y"),NEW(NumExpr)(5)).containsVar()==true);
    
}

TEST_CASE("CompExpr Equals for coverage"){
    CHECK(parse_str("1==1")->equals(parse_str("1"))==false);
}

TEST_CASE("FunExpr equals"){
    CHECK(FunExpr("x",NEW(VarExpr)("x")).equals(NEW(FunExpr)("x",NEW(VarExpr)("x"))));
    CHECK(FunExpr("x",NEW(VarExpr)("x")).equals(NEW(BoolExpr)(true))==false);
}

TEST_CASE("FunExpr containsVar"){
    CHECK(FunExpr("x",NEW(NumExpr)(5)).containsVar()==false);
    CHECK(FunExpr("x",NEW(VarExpr)("x")).containsVar()==true);
}

TEST_CASE("FunExpr Subst"){
    CHECK(runSubst("_fun (x) y+1", "y", NEW(NumVal)( 10))=="_fun (x) (10+1)");
    CHECK(runSubst("_fun (x) x+1", "x", NEW(NumVal)( 10))=="_fun (x) (x+1)");
    CHECK(runSubst("_fun (x) y","y", NEW(NumVal)( 10))=="_fun (x) 10");
}

TEST_CASE("FunExpr interp"){
    PTR(Env) test = NEW(EmptyEnv)();
    CHECK(FunExpr("x",NEW(VarExpr)("x")).interp(test)->equals(NEW(FunVal)( "x",NEW(VarExpr)("x"),test)));
}

TEST_CASE("FunExpr optimize"){
    CHECK(FunExpr("x",NEW(VarExpr)("x")).optimize()->equals(NEW(FunExpr)("x",NEW(VarExpr)("x"))));
    CHECK(FunExpr("x",NEW(AddExpr)(NEW(NumExpr)(1),NEW(NumExpr)(2))).optimize()->equals(NEW(FunExpr)("x",NEW(NumExpr)(3))));
}
TEST_CASE("CallExpr equals"){
    CHECK(parse_str("(_fun (x) x) (10)")->equals(NEW(CallExpr)(NEW(FunExpr)("x",NEW(VarExpr)("x")),NEW(NumExpr)(10))));
    CHECK(parse_str("(_fun (x) x) (10)")->equals(NEW(BoolExpr)(true))==false);
}

TEST_CASE("Call Expr to String"){
    CHECK(parse_str("(_fun (x) x) (10)")->to_string()=="( _fun (x) x (10) )");
}

TEST_CASE("Call Expr containsVar"){
    CHECK(CallExpr(NEW(VarExpr)("x"),NEW(BoolExpr)(true)).containsVar()==true);
    CHECK(CallExpr(NEW(NumExpr)(5),NEW(BoolExpr)(true)).containsVar()==false);
    CHECK(CallExpr(NEW(NumExpr)(5),NEW(VarExpr)("x")).containsVar()==true);
}

TEST_CASE("Call Expr interp"){
    PTR(EmptyEnv) env = NEW(EmptyEnv)();
    CHECK(parse_str("(_fun (x) x )(10)")->interp(env)->equals(NEW(NumVal)( 10)));
    CHECK(parse_str("(_fun (x) x+5 )(10)")->interp(env)->equals(NEW(NumVal)( 15)));
    //This case should indeed throw because the interp on the actual arg will fail due to it not containing a Val. It contains a variable.
    CHECK_THROWS(parse_str("(_fun (x) x )(x)")->interp(env));
}

TEST_CASE("Interp Stress Tests Fun/Call Exprs"){
const char * fctrl ="_let factrl = _fun (factrl) _fun (x)"
"_if x == 1"
"_then 1"
"_else x * factrl(factrl)(x + -1)"
    "_in _let factorial = factrl(factrl) _in factorial(5)";
    CHECK(runInterp(fctrl)=="120");
    const char* fibonacci = "_let fib = _fun (fib)"
                 "_fun (x)"
                   " _if x == 0"
                    "_then 1"
                    "_else _if x == 2 + -1"
                    "_then 1"
                    "_else fib(fib)(x + -1)"
                          "+ fib(fib)(x + -2)"
    "_in fib(fib)(10)";
    CHECK(runInterp(fibonacci)=="89");
}

TEST_CASE("Call Expr subst"){
    CHECK(parse_str("(_fun (x) x )(y)")->subst("y",NEW(NumVal)( 10))->equals(parse_str("(_fun(x) x )(10)")));
    
}*/



