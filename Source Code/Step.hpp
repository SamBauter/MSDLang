//
//  Step.hpp
//  MSDLang
//
//  Created by Samuel Bauter on 3/25/20.
//  Copyright © 2020 U of U. All rights reserved.
//

#ifndef Step_hpp
#define Step_hpp

#include "pointer.hpp"
#include <iostream>

class Expr;
class Env;
class Val;
class Cont;


/**
 *A class generated to enable evaluation of expressions which requier lots of recursion.
*/
class Step{
public:
    typedef enum {
        interp_mode,
        continue_mode
    } mode_t;
    
    static mode_t mode;
    
    static PTR(Expr) expr;
    static PTR(Env) env;
    
    static PTR(Val) val;
    
    static PTR(Cont) cont;
    
    static PTR(Val) interp_by_steps(PTR(Expr) e);

    
    
};

#endif /* Step_hpp */
