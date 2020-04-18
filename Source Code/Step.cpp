//
//  Step.cpp
//  MSDLang
//
//  Created by Samuel Bauter on 3/25/20.
//  Copyright © 2020 U of U. All rights reserved.
//

#include "Step.hpp"
#include "Expr.h"
#include "Env.hpp"
#include "Cont.hpp"
#include "Value.hpp"

Step::mode_t Step::mode;

PTR(Cont) Step::cont;

PTR(Expr) Step::expr; /* only for Step::interp_mode */

PTR(Val) Step::val;

PTR(Env) Step::env;

PTR(Val) Step::interp_by_steps(PTR(Expr) e){
    Step::mode = Step::interp_mode;
    Step::expr = e;
    Step::env =  Env::empty;
    Step::val = nullptr;
    Step::cont = Cont::done;
    
    while(1){
        if(Step::mode == Step::interp_mode){
            Step::expr -> step_interp();
        }else{
            if(Step::cont == Cont::done){
                return Step::val;
            }else{
                Step::cont->step_continue();
            }
        }
    }
}


