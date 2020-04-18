//
//  Env.hpp
//  MSDLang
//
//  Created by Samuel Bauter on 3/1/20.
//  Copyright © 2020 U of U. All rights reserved.
//

#ifndef Env_hpp
#define Env_hpp

#include <stdio.h>
#include "pointer.hpp"
#include <iostream>
class Val;

/**
 *An Abstract class that represents the environment in which an expression will be evaluated.
 *
*/
class Env {
public:
    virtual PTR(Val) lookup(std::string find_name) = 0;
    static PTR(Env) empty;

};

/**
 *A class that represents an environment containing no assigned variables or values.
*/
class EmptyEnv: public Env {
public:
    PTR(Val) lookup(std::string find_name);
};

/**
 *A class that represents an environment containing a variable name and corresponding variable value, and a pointer to nested other Envs.
*/
class ExtendedEnv: public Env {
public:
    std::string name;
    PTR(Val) val;
    PTR(Env) rest;
    
    ExtendedEnv(std::string name, PTR(Val) val, PTR(Env) rest);
    PTR(Val) lookup(std::string find_name);
    
};

#endif /* Env_hpp */
