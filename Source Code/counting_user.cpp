//
//  counting_user.cpp
//  MSDLang
//
//  Created by Samuel Bauter on 4/17/20.
//  Copyright © 2020 U of U. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include "Expr.h"
#include "ExprParser.hpp"
#include "Env.hpp"
#include "Value.hpp"
#include "catch.hpp"
#include "Step.hpp"

int main(int argc, char **argv) {
    
    std::ifstream functionFile("/Users/samuelbauter/SamBauter/CS6015/MSDLang/MSDLang/count.msd");
    std::stringstream buffer;
    buffer << functionFile.rdbuf();
    std::string content = buffer.str();

    int count;
    std::cout<<"Please type an integer number to count to:"<<std::endl;
    std::cin>>count;
    
    std::cout<<"We are now counting with MSD Lang"<<std::endl;
    
    for(int i=0;i<=count; i++){
        std::string expr = "_let numtocount = "+ std::to_string(i) + "_in" + content;
        PTR(Expr) e = parse_str(expr);
        //std::string output = e->interp(NEW(EmptyEnv)())->to_string();
        std::string output = Step::interp_by_steps(e)->to_string();
        std::cout<<output<<std::endl;
    }
}
    
    
    
    
    
    


