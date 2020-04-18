//
//  Value.hpp
//  MSDLang
//
//  Created by Samuel Bauter on 1/31/20.
//  Copyright © 2020 U of U. All rights reserved.
//

#ifndef value_hpp
#define value_hpp

#include "pointer.hpp"


class Expr;
class Env;
class Cont;
class Val;


/**
 *An Abstract Class representing all the methods each subclass of Expr must implement.
 */
class Val ENABLE_THIS(Val) {
    
    public:
    virtual bool equals(PTR(Val) val) = 0;
    virtual PTR(Expr) to_expr() = 0;
    virtual std::string to_string() = 0;
    virtual PTR(Val) add_to(PTR(Val) other_val);
    virtual PTR(Val) sub_with(PTR(Val) other_val);
    virtual PTR(Val) mult_with(PTR(Val) other_val);
    virtual PTR(Val) div_with(PTR(Val) other_val);
    virtual PTR(Val) mod_with(PTR(Val) other_val);
    virtual bool is_true();
    virtual PTR(Val) call(PTR(Val) actual_arg);
    virtual void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest);
private:
    [[ noreturn ]] void not_a(std::string what);
    
protected:
    [[noreturn]] void val_not_a(PTR(Val) val, std::string what);
    
    
    

};
/**
 *A Val that represents a single number.
 *Ex. 1
*/
class NumVal : public Val {
private:
    int other_rep(PTR(Val) other_val);
public:
  int rep;
    NumVal(int rep);
    bool equals(PTR(Val) val) override;
    PTR(Val) add_to(PTR(Val) other_val) override;
    PTR(Val) sub_with(PTR(Val) other_val) override;

    PTR(Val) mult_with(PTR(Val) other_val) override;
    PTR(Val) div_with(PTR(Val) other_val) override;
    PTR(Val) mod_with(PTR(Val) other_val) override;

    PTR(Expr) to_expr()override;
    std::string to_string()override;
    
};

/**
 *A Val that represents a single number.
 *Ex. _true
 */
class BoolVal : public Val {
public:
  bool rep;
  BoolVal(bool rep);
  bool equals(PTR(Val) val)override;
    PTR(Expr) to_expr()override;
    std::string to_string()override;
    bool is_true()override;


};

/**
 *A Val that represents a function declaration and the Env in which it will be evaluated.
 *Ex. _true
*/
class FunVal: public Val {
public:
    std::string formal_arg;
    PTR(Expr) body;
    PTR(Env) env;
    
    FunVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env);
    bool equals(PTR(Val) val)override;
    PTR(Expr) to_expr()override;
    std::string to_string()override;
    PTR(Val) call(PTR(Val) actual_arg)override;
    void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) override;

};



#endif /* value_hpp */
