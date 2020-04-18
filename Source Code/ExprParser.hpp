#ifndef parse_hpp
#define parse_hpp

#include <iostream>
#include "pointer.hpp"

class Expr;

PTR(Expr) parse(std::istream &in);

PTR(Expr) parse_str(std::string s);



#endif /* parse_hpp */

