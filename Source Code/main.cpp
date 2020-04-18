#include <iostream>
#include "ExprParser.hpp"
#include "Env.hpp"
#include "Expr.h"
#include "Value.hpp"
#include "Step.hpp"
#include "Cont.hpp"
#include "pointer.hpp"
#include "op.hpp"

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "Tests.cpp"



int main(int argc, char **argv) {
    if ((argc > 1) && !strcmp(argv[1], "--test"))
        return Catch::Session().run(1, argv);
    else{
    PTR(Expr) e;
    bool optimize_mode = false, step_mode = false;
    
    if ((argc > 1) && !strcmp(argv[1], "--opt")) {
        optimize_mode = true;
        argc--;
        argv++;
    } else if((argc >1) && !strcmp(argv[1], "--step"))  {
        step_mode = true;
        argc--;
        argv++;
    }
    
    if(argc > 1){
        std::ifstream prog_in(argv[1]);
        e = parse(prog_in);
    } else
        e= parse(std::cin);
    
    if(optimize_mode)
        std::cout<<e->optimize()->to_string() <<"\n";
    else if(step_mode)
        std::cout<<Step::interp_by_steps(e)->to_string() << "\n";
    else
        try {
            std::cout << e->interp(NEW(EmptyEnv)())->to_string() << "\n";
        } catch (const std::exception& e) {
            std::cout<<e.what()<<"\n";
            return 5;
        }
    
    return 0;
    }
    }
  
    
    
    

    


