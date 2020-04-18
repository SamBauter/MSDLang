//
// Created by Samuel Bauter on 1/20/20.
//

#ifndef CS6015_HW1_ARITHMETIC_PARSER_EXPR_H
#define CS6015_HW1_ARITHMETIC_PARSER_EXPR_H
//
// Created by Samuel Bauter on 1/15/20.
//
#include <sstream>
#include <iostream>
#include "pointer.hpp"

class Expr;
class Env;
class Val;
class BinaryOp;

/**
 *An Abstract Class representing all the methods each subclass of Expr must implement.
 */
class Expr {
public:
    /**
     *Checks the equality of two expr objects without interpretation. Note: Order matters! Ex: 1+2 != 2+1. Note: Expressions that simplify or interpret to the same result are not equal.
     */
    virtual bool equals(PTR(Expr) e) = 0;
    
    /**
     *Interprets the result of an Expr if possible. If the Expr contains unbound variable and appropriate exception will be generated
     */
    virtual PTR(Val) interp(PTR(Env) env) = 0;

    /**
     *Checks if the Expr contains a variable.
     */
    virtual bool containsVar() = 0;

    /**
     *Searches an Expr for an unbound variable: varName, and replaces that variable with new_val
     */
    virtual PTR(Expr) subst(std::string varName, PTR(Val)  new_val) = 0;
    
    /**
     *Simplifies an Expr if possible.
     */
    virtual PTR(Expr) optimize() = 0;

    /**
     *Converts an Expr to its mathematical string representation, may add parentheses to preserve order of operations
     */
    virtual std::string to_string() = 0;
    
    /**
     *A variation of the interp method which allocates  heap memory to support Exprs that require large amounts of recursion to interpret. Does not return a value but sets a global variable Step::val to the result. This value can be returned by the interp_by_steps method of the Step class
     */
    virtual void step_interp() = 0;

};

/**
 *A general class that is used to represent boolean and number expressions as objects.
 *This class has replaced BoolExpr and NumExpr however those classes can still be used.
 */
class LiteralExpr : public Expr {
public:
    PTR(Val) val;
    LiteralExpr(PTR(Val) val);

     bool equals(PTR(Expr) e) override;
    PTR(Val)  interp(PTR(Env) env) override;
       void step_interp() override;
       bool containsVar() override;
       PTR(Expr) subst(std::string varName, PTR(Val)  new_val) override;
       PTR(Expr) optimize() override;
       std::string to_string() override;
};

/**
 * A  class that is used to represent number expressions as objects.
 * At this point simply a stub class that calls a LiteralExpr Constructor. The stub is used to enable old tests.
 */
class NumExpr : public LiteralExpr {
public:
    int rep;
    Val *val;
    
    NumExpr(int rep);



};
/**
 * A  class that is used to represent number expressions as objects.
 * At this point simply a stub class that calls a LiteralExpr Constructor. The stub is used to enable old tests.
*/
class BoolExpr : public LiteralExpr {
public:
  bool rep;
  Val *val; /* cache value for this expression */
  
  BoolExpr(bool rep);

};


/**
 * A general class that is used to represent binary expressions for addition, subtraction, multiplication, division, modulo, equal to and not equal to.
 * This class has replaced many older classes such as AddExpr and BoolExpr. Those classes remain part of the library for testing purposes.
*/
class BinaryOpExpr : public Expr {
public:
    PTR(BinaryOp) op;
    PTR(Expr) rhs;
    PTR(Expr) lhs;
    
    BinaryOpExpr(PTR(BinaryOp) op, PTR(Expr) lhs, PTR(Expr) rhs);
    
    bool equals(PTR(Expr) other_expr) override;
    
    PTR(Val)  interp(PTR(Env) env) override;
    bool containsVar() override;
    PTR(Expr) subst(std::string varName, PTR(Val)  new_val) override;
    PTR(Expr) optimize() override;
    void step_interp() override;
    std::string to_string() override;

   
};


/**
 * A  class that is used to represent addition expressions as objects.
 * At this point simply a stub class that calls a BinaryOpExpr Constructor. The stub is used to enable old tests.
*/
class AddExpr : public BinaryOpExpr {
public:
  AddExpr(PTR(Expr) lhs, PTR(Expr) rhs);
};

/**
 * A  class that is used to represent multiplication expressions as objects.
 * At this point simply a stub class that calls a BinaryOpExpr Constructor. The stub is used to enable old tests.
*/
class MultExpr : public BinaryOpExpr {
public:
  MultExpr(PTR(Expr) lhs, PTR(Expr) rhs);
};

/**
 * A  class that is used to represent equal to or == expressions as objects.
 * At this point simply a stub class that calls a BinaryOpExpr Constructor. The stub is used to enable old tests.
*/
class CompExpr : public BinaryOpExpr {
public:
  CompExpr(PTR(Expr) lhs, PTR(Expr) rhs);
};

/**
 *An Expr that represents a single variable.
 *Ex: x.
*/
class VarExpr : public Expr {
public:
    std::string name; //!< The name of the variable Ex: x
    explicit VarExpr(std::string name);
    bool equals(PTR(Expr) e) override;
    PTR(Val)  interp(PTR(Env) env) override;
    void step_interp() override;
    bool containsVar() override;
    PTR(Expr) subst(std::string varName, PTR(Val)  new_val) override;
    PTR(Expr) optimize() override;
    std::string to_string() override;


};

/**
 *An Expr that assigns an expression to a variable and evaluates a different expression in light of that assignment.
 *Ex. _let x = 2 _in x+2
*/

class LetExpr : public Expr {
public:
    std::string varName; //!< The name of the variable occuring after the keyword _let. Ex: _let x
    PTR(Expr) rhs;       //!<The Expr the variable is equal to. Ex: _let x = rhs
    PTR(Expr) body;      //!<The Expr the variable will be substituted in for occuring after the keyword _in. Ex: _in x+1
    
    
    LetExpr(std::string varName,PTR(Expr) varVal, PTR(Expr) bodyToSubst);
    bool equals(PTR(Expr) e) override;
    PTR(Val)  interp(PTR(Env) env) override;
    void step_interp() override;
    bool containsVar() override;
    PTR(Expr) subst(std::string varName, PTR(Val)  new_val) override;
    PTR(Expr) optimize() override;
    std::string to_string() override;

};


/**
 *An Expr that represents a conditional statement of the form _if condition _then trueConsequence_else falseConsequence
 *Ex. _if 1==1 _then 5 _else 0
*/
class IfExpr : public Expr {
public:
    PTR(Expr) condition; //!< The Expr condition being evaluated
    PTR(Expr) trueConseq; //!<The Expr that will be evaluated if the condition is true
    PTR(Expr) falseConseq; //!<The Expr that will be evaluated if the condition is false
    
    
    IfExpr(PTR(Expr) condition, PTR(Expr) trueConseq, PTR(Expr) falseConseq);
    bool equals(PTR(Expr) e) override;
    PTR(Val)  interp(PTR(Env) env) override;
    void step_interp() override;
    PTR(Expr) subst(std::string var, PTR(Val) newVal) override;
    std::string to_string() override;
    PTR(Expr) optimize() override;
    bool containsVar() override;

    
};

/**
 *An Expr that represents a function.
 *Ex. _fun ( x ) 1+x
*/
class FunExpr: public Expr {
public:
    std::string formal_arg; //!<The formal argument or input value of the FunExpr occuring between the parentheses. Ex: _fun (formal_arg) x+1
    PTR(Expr) body; //!<The body of the FunExpr occurring after the parentheses. Ex: _fun (x) body
    
    
    FunExpr(std::string formalArg, PTR(Expr) body);
    bool equals(PTR(Expr) e) override;
    PTR(Val)  interp(PTR(Env) env) override;
    void step_interp() override;
    PTR(Expr) subst(std::string var, PTR(Val) newVal) override;
    std::string to_string() override;
    PTR(Expr) optimize() override;
    bool containsVar() override;

};
/**
 *An Expr that represents a function being called with an argument.
 *Ex. 1==1
*/
class CallExpr: public Expr {
public:
    PTR(Expr) to_be_called; //!<The name of repesentation of the FunExpr that will be called. Ex: to_be_called(5)
    PTR(Expr) actual_arg; //!<The argument which will be supplied to to the function being called. Ex: f(actual_arg)
    
    
    CallExpr(PTR(Expr) to_be_called, PTR(Expr) actual_arg);
    bool equals(PTR(Expr) e) override;
    PTR(Val)  interp(PTR(Env) env) override;
    void step_interp() override;
    PTR(Expr) subst(std::string var, PTR(Val) newVal) override;
    std::string to_string() override;
    PTR(Expr) optimize() override;
    bool containsVar() override;

};

#endif //CS6015_HW1_ARITHMETIC_PARSER_EXPR_H
