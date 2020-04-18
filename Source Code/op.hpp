//
//  op.hpp
//  MSDLang
//
//  Created by Samuel Bauter on 4/9/20.
//  Copyright © 2020 U of U. All rights reserved.
//

#ifndef op_hpp
#define op_hpp

#include <stdio.h>
#include "pointer.hpp"
#include <iostream>
#include <vector>
#include <memory>

class Val;


class BinaryOp;

/**
 * An abstract class representing the operation that will be performed for a BinaryOpExpr
 */
class BinaryOp ENABLE_THIS(BinaryOp)  {
private:
public:
  /**
   *Performs the appropriate operation to the two Val class objects and returns the result.
   */
    virtual PTR(Val) perform(PTR(Val) lhs, PTR(Val) rhs) = 0;
    
    /**
     *Returns the symbolic representation of a binary operator. Ex: !=
     */
    virtual std::string to_string() = 0;

  /**
   *Check whether the given operator object is this one via pointer equality inherited by subclasses
   */
  bool equals(PTR(BinaryOp) other_op);
};

/**
 * A class representing the addition operation that will be used in a BinaryOpExpr
*/
class AddOp : public BinaryOp {
private:
public:
    AddOp();
  static PTR(BinaryOp)  get_instance();
  PTR(Val) perform(PTR(Val) lhs, PTR(Val) rhs);
  std::string to_string();
};

/**
 * A class representing the subtraction operation that will be used in a BinaryOpExpr
*/
class SubOp : public BinaryOp {
private:
public:
    SubOp();
  static PTR(BinaryOp)  get_instance();
  PTR(Val) perform(PTR(Val) lhs, PTR(Val) rhs);
  std::string to_string();
};


/**
 * A class representing the multiplication operation that will be used in a BinaryOpExpr
*/
class MultOp : public BinaryOp {
private:
public:
    MultOp();

  static PTR(BinaryOp)  get_instance();
  PTR(Val) perform(PTR(Val) lhs, PTR(Val) rhs);
  std::string to_string();
};

/**
 * A class representing the division operation that will be used in a BinaryOpExpr
*/
class DivOp : public BinaryOp {
private:
public:
    DivOp();

  static PTR(BinaryOp)  get_instance();
  PTR(Val) perform(PTR(Val) lhs, PTR(Val) rhs);
  std::string to_string();
};

/**
 * A class representing the modulo operation that will be used in a BinaryOpExpr
*/
class ModOp : public BinaryOp {
private:
public:
    ModOp();

  static PTR(BinaryOp)  get_instance();
  PTR(Val) perform(PTR(Val) lhs, PTR(Val) rhs);
  std::string to_string();
};

/**
 * A class representing the equal to comparison operation that will be used in a BinaryOpExpr
*/
class CompOp : public BinaryOp {
private:
public:
    CompOp();
  static PTR(BinaryOp)  get_instance();
  PTR(Val) perform(PTR(Val) lhs, PTR(Val) rhs);
  std::string to_string();
};

/**
 * A class representing the not equal to comparison operation that will be used in a BinaryOpExpr
*/
class CompNotOp : public BinaryOp {
private:
public:
    CompNotOp();
    static PTR(BinaryOp)  get_instance();
    PTR(Val) perform(PTR(Val) lhs, PTR(Val) rhs);
    std::string to_string();
};

/**
 A table that contains subtables for different types of Binary Operations.
 For example, it contains a multiplicative table comprised of MultOp, DivOp, and ModOp instances.
 */
extern std::vector<std::vector<PTR(BinaryOp)>> op_tables;




#endif /* op_hpp */
