//
//  Cont.hpp
//  MSDLang
//
//  Created by Samuel Bauter on 3/25/20.
//  Copyright © 2020 U of U. All rights reserved.
//

#ifndef Cont_hpp
#define Cont_hpp

#include "pointer.hpp"
#include <iostream>

class Expr;
class Val;
class Env;
class Cont;
class BinaryOp;

/**
 *A Class that represents a continutation, that will enable interpretation of functions that require large amounts of recursion.
 *Ex. _true
*/
class Cont ENABLE_THIS(Cont){
public:
    virtual void step_continue() = 0;
    static PTR(Cont) done;
};

/**
 *An empty Cont that signals the step-wise interpretation has finished
*/
class DoneCont: public Cont{
public:
    DoneCont();
    void step_continue() override;
};

/**
 *A general Cont used to evaluate the rhs of a BinaryOpExpr
*/
class RightThenOpCont : public Cont {
public:
    PTR(BinaryOp) op;
    PTR(Expr) rhs;
    PTR(Env) env;
    PTR(Cont) rest;
    
    RightThenOpCont(PTR(BinaryOp) op, PTR(Expr) rhs, PTR(Env) env, PTR(Cont) rest);
    
    void step_continue() override;
};
/**
 *A general Cont used to evaluate the lhs of a BinaryOpExpr
*/
class OpCont: public Cont {
public:
    PTR(BinaryOp) op;
    PTR(Val) lhs_val;
    PTR(Cont) rest;
    
    OpCont(PTR(BinaryOp) op, PTR(Val) lhs_val, PTR(Cont) rest);
    void step_continue() override;
};



/**
 *A Cont used to evaluate the condition of a IfExpr
*/
class IfBranchCont : public Cont {
public:
    PTR(Expr) then_part;
    PTR(Expr) else_part;
    PTR(Env) env;
    PTR(Cont) rest;
    
    IfBranchCont(PTR(Expr) then_part,PTR(Expr) else_part, PTR(Env) env, PTR(Cont) rest);
    
    void step_continue() override;

};

/**
 *A Cont used to evaluate the the lhs of a LetExpr
*/
class LetBodyCont: public Cont {
public:
    std::string varName;
    PTR(Expr) body;
    PTR(Env) env;
    PTR(Cont) rest;
    
    LetBodyCont(std::string varName,PTR(Expr) body, PTR(Env) env, PTR(Cont) rest);
    
    void step_continue() override;
};

/**
 *A Cont used to store the to_be_called_val of a CallExpr
*/
class ArgThenCallCont: public Cont {
public:
    PTR(Expr) actual_arg;
    PTR(Env) env;
    PTR(Cont) rest;
    
    ArgThenCallCont(PTR(Expr) actual_arg, PTR(Env) env, PTR(Cont) rest);
    
    void step_continue() override;


};

/**
 *A Cont used to store the actual arg of a CallExpr
*/
class CallCont: public Cont {
public:
    PTR(Val) to_be_called_val;
    PTR(Cont) rest;
    
    CallCont(PTR(Val) to_be_called_val, PTR(Cont) rest);
    
    
    void step_continue() override;


};



#endif /* Cont_hpp */
