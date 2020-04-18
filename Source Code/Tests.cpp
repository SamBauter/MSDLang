//
//  Tests.cpp
//  MSDLang
//
//  Created by Samuel Bauter on 4/16/20.
//  Copyright © 2020 U of U. All rights reserved.
//

//Testing Stub for library file


static std::string runOpt(std::string str){
    return parse_str(str)->optimize()->to_string();
}

static std::string runInterp(std::string str){
    PTR(Env) env =NEW(EmptyEnv)();
    return parse_str(str)->interp(env)->to_string();
}

static std::string runStepInterp(std::string str){
    PTR(Expr) e = parse_str(str);
    return Step::interp_by_steps(e)->to_string();
}

static std::string runInterpWithEnv(std::string str, PTR(Env) env){
    return parse_str(str)->interp(env)->to_string();
}

static std::string runSubst(std::string exprStr,std::string newVarName, PTR(Val) new_val){
    PTR(Expr) parsedExpr= parse_str(exprStr);
    return parsedExpr->subst(newVarName, new_val)->to_string();
}

TEST_CASE("CompArg parse Tests"){
    CHECK(parse_str("1==1")->equals(NEW(CompExpr)(NEW(NumExpr)(1),NEW(NumExpr)(1))));
    CHECK(parse_str("5+x==2")->equals(NEW(CompExpr)(NEW(AddExpr)( NEW(NumExpr)(5),NEW(VarExpr)("x")),NEW(NumExpr)(2))));
    CHECK(parse_str("x==x+3")->equals(NEW(CompExpr)(NEW(VarExpr)("x"),NEW(AddExpr)( NEW(VarExpr)("x"),NEW(NumExpr)(3)))));
    
}

TEST_CASE("Parsing CallExpr"){
    CHECK(parse_str("(_fun (x) x)(10)")->equals(NEW(CallExpr)(NEW(FunExpr)("x",NEW(VarExpr)("x")),NEW(NumExpr)(10))));
}
TEST_CASE("FunParser"){
    CHECK(parse_str("_fun (x) x")->equals(NEW(FunExpr)("x",NEW(VarExpr)("x"))));
    CHECK(parse_str("_fun (x) x+1")->to_string()=="_fun (x) (x+1)");
    CHECK(parse_str("_fun (y) x+y+1")->to_string()=="_fun (y) (x+(y+1))");
    CHECK_THROWS(parse_str("_fun x"));
    CHECK_THROWS(parse_str("_fun (1"));
    CHECK_THROWS(parse_str("_fun (x"));
}

TEST_CASE("Double Call Expr"){    CHECK(parse_str("f(10)(1)")->equals(NEW(CallExpr)(NEW(CallExpr)(NEW(VarExpr)("f"),NEW(NumExpr)(10)),NEW(NumExpr)(1))));
}
TEST_CASE("Parse if Expr"){
    CHECK(parse_str("_if 5 _then 6 _else 7")->equals(NEW(IfExpr)(NEW(NumExpr)(5),NEW(NumExpr)(6),NEW(NumExpr)(7))));
    CHECK(parse_str("_if _true _then _true _else _false")->equals(NEW(IfExpr)(NEW(BoolExpr)(true),NEW(BoolExpr)(true),NEW(BoolExpr)(false))));
    CHECK_THROWS(parse_str("_if _true then _false"));
    CHECK_THROWS(parse_str("_if _true _error "));
    CHECK_THROWS(parse_str("_if _true _then _false error"));
    CHECK_THROWS(parse_str("_if _true _then _false _notelse"));
    
    
}
TEST_CASE("Negative number Parse "){
    CHECK(parse_str("-1")->equals(NEW(NumExpr)(-1)));
}
TEST_CASE("Let Expr Exception cases"){
    CHECK_THROWS(parse_str("_let 5"));
    CHECK_THROWS(parse_str("_let x *"));
    CHECK_THROWS(parse_str("_let x = in"));
    
}
TEST_CASE("Associativity of *"){
    CHECK(parse_str("1*2+x")->equals(NEW(AddExpr)( NEW(MultExpr)( NEW(NumExpr)(1),NEW(NumExpr)(2)),NEW(VarExpr)("x"))));
}




TEST_CASE( "See if this works for ExprParser" ) {
  PTR(Expr) ten_plus_one = NEW(AddExpr)( NEW(NumExpr)(10), NEW(NumExpr)(1));
  

  CHECK( parse_str("10")->equals(NEW(NumExpr)(10)) );
  CHECK( parse_str("(10)")->equals(NEW(NumExpr)(10)) );
  CHECK( parse_str("10+1")->equals(ten_plus_one) );
  CHECK( parse_str("(10+1)")->equals(ten_plus_one) );
  CHECK( parse_str("(10)+1")->equals(ten_plus_one) );
  CHECK( parse_str("10+(1)")->equals(ten_plus_one) );
  CHECK( parse_str("1+2*3")->equals(NEW(AddExpr)( NEW(NumExpr)(1),
                                            NEW(MultExpr)( NEW(NumExpr)(2), NEW(NumExpr)(3)))) );
  CHECK( parse_str("1*2+3")->equals(NEW(AddExpr)( NEW(MultExpr)( NEW(NumExpr)(1), NEW(NumExpr)(2)),
                                            NEW(NumExpr)(3))) );
  CHECK( parse_str("4*2*3")->equals(NEW(MultExpr)( NEW(NumExpr)(4),
                                             NEW(MultExpr)( NEW(NumExpr)(2), NEW(NumExpr)(3)))) );
  CHECK( parse_str("4+2+3")->equals(NEW(AddExpr)( NEW(NumExpr)(4),
                                            NEW(AddExpr)( NEW(NumExpr)(2), NEW(NumExpr)(3)))) );
  CHECK( parse_str("4*(2+3)")->equals(NEW(MultExpr)( NEW(NumExpr)(4),
                                               NEW(AddExpr)( NEW(NumExpr)(2), NEW(NumExpr)(3)))) );
  CHECK( parse_str("(2+3)*4")->equals(NEW(MultExpr)( NEW(AddExpr)( NEW(NumExpr)(2), NEW(NumExpr)(3)),
                                               NEW(NumExpr)(4))) );
  CHECK( parse_str("xyz")->equals(NEW(VarExpr)("xyz")) );
  CHECK( parse_str("xyz+1")->equals(NEW(AddExpr)( NEW(VarExpr)("xyz"), NEW(NumExpr)(1))) );


  CHECK( parse_str(" 10 ")->equals(NEW(NumExpr)(10)) );
  CHECK( parse_str(" (  10 ) ")->equals(NEW(NumExpr)(10)) );
  CHECK( parse_str(" 10  + 1")->equals(ten_plus_one) );
  CHECK( parse_str(" ( 10 + 1 ) ")->equals(ten_plus_one) );
  CHECK( parse_str(" 11 * ( 10 + 1 ) ")->equals(NEW(MultExpr)( NEW(NumExpr)(11),
                                                         ten_plus_one)) );
  CHECK( parse_str(" ( 11 * 10 ) + 1 ")
        ->equals(NEW(AddExpr)( NEW(MultExpr)(NEW(NumExpr)(11), NEW(NumExpr)(10)),
                         NEW(NumExpr)(1))));
  CHECK( parse_str(" 1 + 2 * 3 ")
        ->equals(NEW(AddExpr)( NEW(NumExpr)(1),
                         NEW(MultExpr)( NEW(NumExpr)(2), NEW(NumExpr)(3)))) );

 
  

}


TEST_CASE("OOPparse") {
    //Normal Expression testing.
    CHECK(parse_str("1")->equals(NEW(NumExpr)(1)));
    CHECK(parse_str("(1)")->equals(NEW(NumExpr)(1)));
    CHECK(parse_str("1+2")->equals(NEW(AddExpr)( NEW(NumExpr)(1), NEW(NumExpr)(2))));
    CHECK(parse_str("1*2")->equals(NEW(MultExpr)( NEW(NumExpr)(1), NEW(NumExpr)(2))));
    CHECK(parse_str("1+(2+3)")->equals(NEW(AddExpr)( NEW(NumExpr)(1), NEW(AddExpr)( NEW(NumExpr)(2), NEW(NumExpr)(3)))));
    CHECK(parse_str("1*9+(2+3)")->equals(
            NEW(AddExpr)( NEW(MultExpr)( NEW(NumExpr)(1), NEW(NumExpr)(9)), NEW(AddExpr)( NEW(NumExpr)(2), NEW(NumExpr)(3)))));
    CHECK(parse_str("(1*9)+(2+3)")->equals(
            NEW(AddExpr)( NEW(MultExpr)( NEW(NumExpr)(1), NEW(NumExpr)(9)), NEW(AddExpr)( NEW(NumExpr)(2), NEW(NumExpr)(3)))));
    CHECK(parse_str("A")->equals(NEW(VarExpr)("A")));
    CHECK(parse_str("A+B")->equals(NEW(AddExpr)( NEW(VarExpr)("A"), NEW(VarExpr)("B"))));
    CHECK(parse_str("A+3")->equals(NEW(AddExpr)( NEW(VarExpr)("A"), NEW(NumExpr)(3))));
    CHECK(parse_str("Billy+1")->equals(NEW(AddExpr)( NEW(VarExpr)("Billy"), NEW(NumExpr)(1))));
    CHECK(parse_str("(donuts+cops)*10")->equals(
            NEW(MultExpr)( NEW(AddExpr)( NEW(VarExpr)("donuts"), NEW(VarExpr)("cops")), NEW(NumExpr)(10))));
    //CHECK (parse_str_error("(1") == "expected a close parenthesis\n");
    CHECK(parse_str("1+1+1")->equals(NEW(AddExpr)( NEW(NumExpr)(1), NEW(AddExpr)( NEW(NumExpr)(1), NEW(NumExpr)(1)))));
    CHECK(parse_str("sam*sam*sam")->equals(
            NEW(MultExpr)( NEW(VarExpr)("sam"), NEW(MultExpr)( NEW(VarExpr)("sam"), NEW(VarExpr)("sam")))));

    //Whitespace testing
    CHECK(parse_str("   1  ")->equals(NEW(NumExpr)(1)));
    CHECK(parse_str(" ( 1 )  ")->equals(NEW(NumExpr)(1)));
    CHECK(parse_str("    1+   2")->equals(NEW(AddExpr)( NEW(NumExpr)(1), NEW(NumExpr)(2))));
    CHECK(parse_str("  1*  2")->equals(NEW(MultExpr)( NEW(NumExpr)(1), NEW(NumExpr)(2))));
    CHECK(parse_str("1+(  2+   3)")->equals(NEW(AddExpr)( NEW(NumExpr)(1), NEW(AddExpr)( NEW(NumExpr)(2), NEW(NumExpr)(3)))));
    CHECK(parse_str("  1*  9+(2+3)  ")->equals(
            NEW(AddExpr)( NEW(MultExpr)( NEW(NumExpr)(1), NEW(NumExpr)(9)), NEW(AddExpr)( NEW(NumExpr)(2), NEW(NumExpr)(3)))));
    CHECK(parse_str("(1  *9  )+(  2+3)")->equals(
            NEW(AddExpr)( NEW(MultExpr)( NEW(NumExpr)(1), NEW(NumExpr)(9)), NEW(AddExpr)( NEW(NumExpr)(2), NEW(NumExpr)(3)))));
    CHECK(parse_str("   A")->equals(NEW(VarExpr)("A")));
    CHECK(parse_str("A+     B")->equals(NEW(AddExpr)( NEW(VarExpr)("A"), NEW(VarExpr)("B"))));
    CHECK(parse_str("    A+ 3")->equals(NEW(AddExpr)( NEW(VarExpr)("A"), NEW(NumExpr)(3))));
    //CHECK(parse_str("Bil    ly+1")->equals(NEW(AddExpr)( NEW(VarExpr)("Billy"), NEW(NumExpr)(1))));
    CHECK(parse_str("(   donuts    +    cops)     *10")->equals(
            NEW(MultExpr)( NEW(AddExpr)( NEW(VarExpr)("donuts"), NEW(VarExpr)("cops")), NEW(NumExpr)(10))));
    //CHECK (parse_str_error("(   1") == "expected a close parenthesis\n");
    CHECK(parse_str("1    +1    +1")->equals(NEW(AddExpr)( NEW(NumExpr)(1), NEW(AddExpr)( NEW(NumExpr)(1), NEW(NumExpr)(1)))));
    CHECK(parse_str("sam    *    sam   *  sam")->equals(
            NEW(MultExpr)( NEW(VarExpr)("sam"), NEW(MultExpr)( NEW(VarExpr)("sam"), NEW(VarExpr)("sam")))));
}

TEST_CASE("_true and _false parsing"){
    CHECK(parse_str("_true")->equals(NEW(BoolExpr)(true)));
    CHECK(parse_str("_false")->equals(NEW(BoolExpr)(false)));
    CHECK(parse_str("_true + _false")->equals(NEW(AddExpr)( NEW(BoolExpr)(true),NEW(BoolExpr)(false))));
}



TEST_CASE("NumVal is_true"){
    CHECK_THROWS(NumVal(5).is_true());
}


TEST_CASE("NumVal call"){
    CHECK_THROWS(NumVal(5).call(NEW(NumVal)(5)));
}
TEST_CASE("BoolVal call"){
    CHECK_THROWS(BoolVal(true).call(NEW(BoolVal)(false)));
}
TEST_CASE("FunVal is_true"){
    PTR(Env) env = NEW(EmptyEnv)();
    CHECK_THROWS(FunVal("x",NEW(NumExpr)(2),env).is_true());
}
TEST_CASE( "values equals" ) {
  CHECK( (NEW(NumVal)(5))->equals(NEW(NumVal)(5)) );
  CHECK( ! (NEW(NumVal)(7))->equals(NEW(NumVal)(5)) );

  CHECK( (NEW(BoolVal)(true))->equals(NEW(BoolVal)(true)) );
  CHECK( ! (NEW(BoolVal)(true))->equals(NEW(BoolVal)(false)) );
  CHECK( ! (NEW(BoolVal)(false))->equals(NEW(BoolVal)(true)) );

  CHECK( ! (NEW(NumVal)(7))->equals(NEW(BoolVal)(false)) );
  CHECK( ! (NEW(BoolVal)(false))->equals(NEW(NumVal)(8)) );
}

TEST_CASE( "add_to" ) {
  
  CHECK ( (NEW(NumVal)(5))->add_to(NEW(NumVal)(8))->equals(NEW(NumVal)(13)) );

  CHECK_THROWS( (NEW(NumVal)(5))->add_to(NEW(BoolVal)(false)));
  CHECK_THROWS( (NEW(BoolVal)(false))->add_to(NEW(BoolVal)(false)));
}

TEST_CASE( "mult_with" ) {
  
  CHECK ( (NEW(NumVal)(5))->mult_with(NEW(NumVal)(8))->equals(NEW(NumVal)(40)) );

  CHECK_THROWS( (NEW(NumVal)(5))->mult_with(NEW(BoolVal)(false)));
  CHECK_THROWS( (NEW(BoolVal)(false))->mult_with(NEW(BoolVal)(false)));
}

TEST_CASE( "value to_expr" ) {
  CHECK( (NEW(NumVal)(5))->to_expr()->equals(NEW(NumExpr)(5)) );
  CHECK( (NEW(BoolVal)(true))->to_expr()->equals(NEW(BoolExpr)(true)) );
  CHECK( (NEW(BoolVal)(false))->to_expr()->equals(NEW(BoolExpr)(false)) );
}

TEST_CASE( "value to_string" ) {
  CHECK( (NEW(NumVal)(5))->to_string() == "5" );
  CHECK( (NEW(BoolVal)(true))->to_string() == "_true" );
  CHECK( (NEW(BoolVal)(false))->to_string() == "_false" );
}


TEST_CASE("Num Opt"){
    CHECK(runOpt("5")=="5");
}

TEST_CASE("Number:toString"){
    CHECK(NumExpr(5).to_string()=="5");
}



TEST_CASE("Number::value") {
    CHECK(NumExpr(1).interp(NEW(EmptyEnv)())->equals(NEW(NumVal)( 1)));
    CHECK(NumExpr(0).interp(NEW(EmptyEnv)())->equals(NEW(NumVal)( 0)));
    CHECK(NumExpr(100).interp(NEW(EmptyEnv)())->equals(NEW(NumVal)( 100)));
}

TEST_CASE("Number::containsVar") {
    CHECK(NumExpr(1).containsVar() == false);
}

TEST_CASE("Step interp"){
    CHECK(runStepInterp("5")=="5");
}

TEST_CASE("Num interp"){
    CHECK(runInterp("5")=="5");
}


TEST_CASE("Bool Expr Equals"){
    CHECK(parse_str("_true")->equals(parse_str("_true")));
    CHECK(parse_str("_true")->equals(parse_str("1"))==false);
}

TEST_CASE("Bool Expr Subst"){
    CHECK(BoolExpr(true).subst("x",NEW(NumVal)(5))->equals(NEW(BoolExpr)(true)));
}

TEST_CASE("CompNotOps interp"){
    CHECK(runInterp("1!=0")=="_true");
    CHECK(runInterp("_let x=5 _in x+x != 11")=="_true");
    CHECK(runInterp("_let x=5 _in x+x != 10")=="_false");
}

TEST_CASE("Div interps"){
    CHECK(runInterp("5/3")=="1");
}
TEST_CASE("Mod interps"){
    CHECK(runInterp("5%3")=="2");
}

TEST_CASE("Sub interps"){
    CHECK(runInterp("2-5") == "-3");
}

TEST_CASE("BinaryOpExpr optimize tests"){
    CHECK(runOpt("5+5")=="10");
    
}

TEST_CASE("Add::equals()"){
    CHECK(AddExpr(NEW(NumExpr)(3),NEW(NumExpr)(4)).equals(NEW(VarExpr)("x"))==false);
}

TEST_CASE("Add interp"){
    CHECK(runInterp("5+5")=="10");
    CHECK(runInterp("5+5+5")=="15");
}

TEST_CASE("Add Opt"){
    CHECK(runOpt("5+3")=="8");
    CHECK(runOpt("5+x")=="(5+x)");
    CHECK(runOpt("5+x+5")=="(5+(x+5))");
}

TEST_CASE("Add subst() tests"){
    CHECK(AddExpr(NEW(NumExpr)(1),NEW(NumExpr)(2)).subst("x",NEW(NumVal)( 10))->equals(NEW(AddExpr)(NEW(NumExpr)(1),NEW(NumExpr)(2))));
    CHECK(AddExpr(NEW(VarExpr)("x"),NEW(NumExpr)(2)).subst("x",NEW(NumVal)( 10))->equals(NEW(AddExpr)(NEW(NumExpr)(10),NEW(NumExpr)(2))));
    CHECK(AddExpr(NEW(VarExpr)("x"),NEW(AddExpr)(NEW(VarExpr)("x"),NEW(NumExpr)(2))).subst("x",NEW(NumVal)( 10))->equals(NEW(AddExpr)(NEW(NumExpr)(10),
            NEW(AddExpr)(NEW(NumExpr)(10),NEW(NumExpr)(2)))));
}

TEST_CASE("Add value() tests") {
    CHECK(AddExpr(NEW(NumExpr)(3), NEW(NumExpr)(4)).interp(NEW(EmptyEnv)())->equals(NEW(NumVal)( 7)));
    CHECK(AddExpr(NEW(NumExpr)(4), NEW(AddExpr)(NEW(NumExpr)(4), NEW(NumExpr)(4))).interp(NEW(EmptyEnv)()) ->equals(NEW(NumVal)( 12)));
    CHECK(AddExpr(NEW(AddExpr)(NEW(NumExpr)(4), NEW(NumExpr)(4)), NEW(AddExpr)(NEW(NumExpr)(4), NEW(NumExpr)(4))).interp(NEW(EmptyEnv)())->equals(NEW(NumVal)( 16)));
}

TEST_CASE("Mult Equals Null"){
    CHECK(MultExpr(NEW(NumExpr)(5),NEW(NumExpr)(4)).equals(NEW(NumExpr)(3))==false);
}

TEST_CASE("Mult interp"){
    CHECK(runInterp("5*5")=="25");
    CHECK(runInterp("5*5*5")=="125");
}

TEST_CASE("Mult Optimize"){
    CHECK(runOpt("x*5*5")=="(x*25)");
    CHECK(runOpt("5*5*x")=="(5*(5*x))");
    CHECK(runOpt("5*5*5")=="125");
    
}

TEST_CASE("Mult::exprToString"){
    CHECK(MultExpr(NEW(VarExpr)("x"),NEW(VarExpr)("y")).to_string()=="(x*y)");
    CHECK(MultExpr(NEW(NumExpr)(5),NEW(NumExpr)(4)).to_string()=="(5*4)");
    
}

TEST_CASE("Mult subst() tests"){
    CHECK(MultExpr(NEW(NumExpr)(1),NEW(NumExpr)(2)).subst("x",NEW(NumVal)( 10))->equals(NEW(MultExpr)(NEW(NumExpr)(1),NEW(NumExpr)(2))));
    CHECK(MultExpr(NEW(VarExpr)("x"),NEW(NumExpr)(2)).subst("x",NEW(NumVal)( 10))->equals(NEW(MultExpr)(NEW(NumExpr)(10),NEW(NumExpr)(2))));
    CHECK(MultExpr(NEW(VarExpr)("y"),NEW(VarExpr)("y")).subst("y", NEW(NumVal)( 10))->
          equals(NEW(MultExpr)(NEW(NumExpr)(10),NEW(NumExpr)(10))));
}

TEST_CASE("Mult value()") {
    CHECK(MultExpr(NEW(NumExpr)(3), NEW(NumExpr)(4)).interp(NEW(EmptyEnv)())->equals(NEW(NumVal)( 12)));
    CHECK(MultExpr(NEW(NumExpr)(4), NEW(MultExpr)(NEW(NumExpr)(4), NEW(NumExpr)(4))).interp(NEW(EmptyEnv)()) ->equals(NEW(NumVal)( 64)));
    CHECK(MultExpr(NEW(MultExpr)(NEW(NumExpr)(4), NEW(NumExpr)(4)), NEW(MultExpr)(NEW(NumExpr)(4), NEW(NumExpr)(4))).interp(NEW(EmptyEnv)())->equals(NEW(NumVal)( 256)));
    CHECK(MultExpr(NEW(AddExpr)(NEW(NumExpr)(4), NEW(NumExpr)(4)), NEW(NumExpr)(4)).interp(NEW(EmptyEnv)())->equals(NEW(NumVal)( 32)));
}

TEST_CASE("Variable Equals"){
    CHECK(VarExpr("x").equals(NEW(NumExpr)(10))==false);
}

TEST_CASE("Var interp"){
    CHECK_THROWS(runInterp("x"));
}

TEST_CASE("Var optimize"){
    CHECK(runOpt("x")=="x");
}

TEST_CASE("Let Expr Equals"){
    CHECK(parse_str("_let x = 1 _in x")->equals(parse_str("_let x = 1 _in x")));
    CHECK(parse_str("_let x = 1 _in x")->equals(parse_str("x+1"))==false);

}


TEST_CASE("Let interp"){
    CHECK(runInterp("_let x=5 _in x+x")=="10");
    CHECK(runInterp("_let x=5 _in _let x=3 _in x+x")=="6");
    CHECK(runInterp("_let x=5 _in _let y=3 _in x+y")=="8");
    CHECK_THROWS(runInterp("_let x=5 _in y+y"));
    
}

TEST_CASE("Let optimize"){
    CHECK(runOpt("_let x=5 _in x+x")=="10");
    CHECK(runOpt("_let x=5 _in _let x=3 _in x+x")=="6");
    CHECK(runOpt("_let x=5 _in _let y=3 _in x+y")=="8");
    CHECK(runOpt("_let x=5 _in _let y=z+4 _in x+y+4")=="_let y = (z+4) _in (5+(y+4))");
    CHECK(runOpt("_let x=5 _in _let x=6 _in x")=="6");
    
}
TEST_CASE("Let Expr Contains Var"){
    CHECK(LetExpr("x",NEW(VarExpr)("x"),NEW(NumExpr)(2)).containsVar()==true);
    CHECK(LetExpr("x",NEW(VarExpr)("x"), NEW(NumExpr)(2)).containsVar()==true);
    CHECK(LetExpr("x",NEW(NumExpr)(2), NEW(NumExpr)(2)).containsVar()==false);

}
TEST_CASE("LetExpr NEsting?"){
    CHECK(runOpt("_let x=5 _in _let x=6 _in _let x=7 _in x ")=="7");
    CHECK(runOpt("_let y=3 _in 5+y")=="8");
}

TEST_CASE("IfExpr to string and equals"){
    CHECK(parse_str("_if 2 _then 3 _else 4")->to_string()=="_if 2 _then 3 _else 4");
    CHECK(parse_str("_if _true _then 1 _else 2")->equals(parse_str("1"))==false);
}

TEST_CASE("If Expr Subst"){
    CHECK(parse_str("_if x _then x _else y")->subst("x", NEW(NumVal)( 3))->equals(parse_str("_if 3 _then 3 _else y")));
}

TEST_CASE("If Expr Interp"){
    CHECK(runInterp("_if _true _then 1 _else 0")=="1");
    CHECK(runInterp("_if 5==5 _then 1 _else 0")=="1");
    CHECK(runInterp("_if (5)==(5) _then 1 _else 0")=="1");
    CHECK_THROWS(runInterp("_if x==x _then 1 _else 0"));
    
}

TEST_CASE("If Expr Optimize"){
    CHECK(runOpt("_if x==x _then 1 _else 2")=="_if (x==x) _then 1 _else 2");
    CHECK(runOpt("_if 1==1 _then 1 _else 2") == "1");
    CHECK(runOpt("_if _false _then 1 _else 2")=="2");
    CHECK(runOpt("_if _true _then 1 _else 2")=="1");
    CHECK(runOpt("_if 7 _then 1 _else 2")=="_if 7 _then 1 _else 2");
    
}

TEST_CASE("CompExpr interp"){
    CHECK(runInterp("2==1+1")=="_true");
    CHECK(runInterp("_true == 4")=="_false");
    CHECK_THROWS(runInterp("x==3"));
    CHECK_THROWS(runInterp("x+1 == x+1"));
    CHECK(runInterp("_let x=5 _in x+x") == "10");
    
}

TEST_CASE("CompExpr Optimize"){
    CHECK(runOpt("1==1")=="_true");
    CHECK(runOpt("x==x")=="(x==x)");
    CHECK(runOpt("1==2")=="_false");
    CHECK(runOpt("x+1==x+1")=="((x+1)==(x+1))");
}

TEST_CASE("CompExpr::containsVar"){
    CHECK(CompExpr(NEW(NumExpr)(5),NEW(NumExpr)(5)).containsVar()==false);
    CHECK(CompExpr(NEW(NumExpr)(5),NEW(VarExpr)("x")).containsVar()==true);
    CHECK(CompExpr(NEW(VarExpr)("y"),NEW(NumExpr)(5)).containsVar()==true);
    
}

TEST_CASE("CompExpr Equals for coverage"){
    CHECK(parse_str("1==1")->equals(parse_str("1"))==false);
}

TEST_CASE("FunExpr equals"){
    CHECK(FunExpr("x",NEW(VarExpr)("x")).equals(NEW(FunExpr)("x",NEW(VarExpr)("x"))));
    CHECK(FunExpr("x",NEW(VarExpr)("x")).equals(NEW(BoolExpr)(true))==false);
}

TEST_CASE("FunExpr containsVar"){
    CHECK(FunExpr("x",NEW(NumExpr)(5)).containsVar()==false);
    CHECK(FunExpr("x",NEW(VarExpr)("x")).containsVar()==true);
}

TEST_CASE("FunExpr Subst"){
    CHECK(runSubst("_fun (x) y+1", "y", NEW(NumVal)( 10))=="_fun (x) (10+1)");
    CHECK(runSubst("_fun (x) x+1", "x", NEW(NumVal)( 10))=="_fun (x) (x+1)");
    CHECK(runSubst("_fun (x) y","y", NEW(NumVal)( 10))=="_fun (x) 10");
}

TEST_CASE("FunExpr interp"){
    PTR(Env) test = NEW(EmptyEnv)();
    CHECK(FunExpr("x",NEW(VarExpr)("x")).interp(test)->equals(NEW(FunVal)( "x",NEW(VarExpr)("x"),test)));
}

TEST_CASE("FunExpr optimize"){
    CHECK(FunExpr("x",NEW(VarExpr)("x")).optimize()->equals(NEW(FunExpr)("x",NEW(VarExpr)("x"))));
    CHECK(FunExpr("x",NEW(AddExpr)(NEW(NumExpr)(1),NEW(NumExpr)(2))).optimize()->equals(NEW(FunExpr)("x",NEW(NumExpr)(3))));
}

TEST_CASE("CallExpr equals"){
    CHECK(parse_str("(_fun (x) x) (10)")->equals(NEW(CallExpr)(NEW(FunExpr)("x",NEW(VarExpr)("x")),NEW(NumExpr)(10))));
    CHECK(parse_str("(_fun (x) x) (10)")->equals(NEW(BoolExpr)(true))==false);
}

TEST_CASE("Call Expr to String"){
    CHECK(parse_str("(_fun (x) x) (10)")->to_string()=="( _fun (x) x (10) )");
}

TEST_CASE("Call Expr containsVar"){
    CHECK(CallExpr(NEW(VarExpr)("x"),NEW(BoolExpr)(true)).containsVar()==true);
    CHECK(CallExpr(NEW(NumExpr)(5),NEW(BoolExpr)(true)).containsVar()==false);
    CHECK(CallExpr(NEW(NumExpr)(5),NEW(VarExpr)("x")).containsVar()==true);
}

TEST_CASE("Call Expr interp"){
    PTR(EmptyEnv) env = NEW(EmptyEnv)();
    CHECK(parse_str("(_fun (x) x )(10)")->interp(env)->equals(NEW(NumVal)( 10)));
    CHECK(parse_str("(_fun (x) x+5 )(10)")->interp(env)->equals(NEW(NumVal)( 15)));
    //This case should indeed throw because the interp on the actual arg will fail due to it not containing a Val. It contains a variable.
    CHECK_THROWS(parse_str("(_fun (x) x )(x)")->interp(env));
}

TEST_CASE("Interp Stress Tests Fun/Call Exprs"){
const char * fctrl ="_let factrl = _fun (factrl) _fun (x)"
"_if x == 1"
"_then 1"
"_else x * factrl(factrl)(x + -1)"
    "_in _let factorial = factrl(factrl) _in factorial(5)";
    CHECK(runInterp(fctrl)=="120");
    const char* fibonacci = "_let fib = _fun (fib)"
                 "_fun (x)"
                   " _if x == 0"
                    "_then 1"
                    "_else _if x == 2 + -1"
                    "_then 1"
                    "_else fib(fib)(x + -1)"
                          "+ fib(fib)(x + -2)"
    "_in fib(fib)(10)";
    CHECK(runInterp(fibonacci)=="89");
}

TEST_CASE("Call Expr subst"){
    CHECK(parse_str("(_fun (x) x )(y)")->subst("y",NEW(NumVal)( 10))->equals(parse_str("(_fun(x) x )(10)")));
    
}

TEST_CASE("Variables: subst()"){
    CHECK(VarExpr("x").subst("x",NEW(NumVal)( 10))->equals(NEW(NumExpr)(10)));
    CHECK(VarExpr("dog").subst("dog",NEW(NumVal)( 20))->equals(NEW(NumExpr)(20)));
    CHECK(VarExpr("cat").subst("dog", NEW(NumVal)( 10))->equals(NEW(VarExpr)("cat")));

}

TEST_CASE("equals Number") {
    CHECK((NEW(NumExpr)(1))->equals(NEW(NumExpr)(1)));
    CHECK(!(NEW(NumExpr)(1))->equals(NEW(NumExpr)(2)));
    CHECK(!(NEW(NumExpr)(1))->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(4))));

}






TEST_CASE("Add containsVar() tests") {
    CHECK(AddExpr(NEW(NumExpr)(3), NEW(NumExpr)(4)).containsVar() == false);
    CHECK(AddExpr(NEW(NumExpr)(3), NEW(VarExpr)("x")).containsVar() == true);
    CHECK(AddExpr(NEW(VarExpr)("x"), NEW(NumExpr)(3)).containsVar() == true);
    CHECK(AddExpr(NEW(VarExpr)("x"), NEW(VarExpr)("x")).containsVar() == true);


}

TEST_CASE("Mult containsVar() tests") {
    CHECK(MultExpr(NEW(NumExpr)(3), NEW(NumExpr)(4)).containsVar() == false);
    CHECK(MultExpr(NEW(NumExpr)(3), NEW(VarExpr)("x")).containsVar() == true);
    CHECK(MultExpr(NEW(VarExpr)("x"), NEW(NumExpr)(3)).containsVar() == true);
    CHECK(MultExpr(NEW(VarExpr)("x"), NEW(VarExpr)("x")).containsVar() == true);
}

TEST_CASE("Var containsVar() tests") {
    CHECK(VarExpr("x").containsVar() == true);
}


TEST_CASE("Harness Generated Failures") {
    //Had to fix the interp part of Optimize for MULT AND ADD to check for bools
    CHECK_THROWS(runOpt("_true * 5")=="(_true * 5)");
    CHECK_THROWS(runOpt("_false + _false * 739586036 == _false")=="_false");
    CHECK_THROWS(runOpt("_if 2057750526 _then 419251295 _else 1802082212 + _fun ( SZUdYnFSiENvThttQjtzdGhc ) 861174277")=="_if 2057750526 _then 419251295 _else 1802082212 + _fun (SZUdYnFSiENvThttQjtzdGhc) 861174277");
    //Had to fix the interp part of Optimize for rhs of Let Exprs to check for Bools in addition to vars
    CHECK(runOpt("_let OxbrRWeExzGvErdLU = _if 1219341124 _then _true _else 789026548 _in HKBecgdxpH")=="_let OxbrRWeExzGvErdLU = _if 1219341124 _then _true _else 789026548 _in HKBecgdxpH");
    CHECK_THROWS(runOpt("_let vUqWN = 1596458983 + _false _in _fun ( ayKXCaBRlxaEmDwJrJTixoC ) RmpLkXX")=="_let vUqWN = 1596458983 + _false _in _fun (ayKXCaBRlxaEmDwJrJTixoC) RmpLkXX");
    CHECK(runOpt("_let vXYvMRHZCUIcv = _false ( 1184395709 )  _in 1821317764")=="_let vXYvMRHZCUIcv = ( _false (1184395709) ) _in 1821317764");
    CHECK(runOpt("_let OxbrRWeExzGvErdLU = _if 1219341124 _then _true _else 789026548 _in HKBecgdxpH")=="_let OxbrRWeExzGvErdLU = _if 1219341124 _then _true _else 789026548 _in HKBecgdxpH");
}

TEST_CASE("Env Test Cases"){
    PTR(EmptyEnv) emptyenv = NEW(EmptyEnv)();
    PTR(NumVal) five = NEW(NumVal)(5);
    PTR(NumVal) ten = NEW(NumVal)(10);
    PTR(BoolVal) trueVal = NEW(BoolVal)(true);


    PTR(ExtendedEnv) XtoFive = NEW(ExtendedEnv)("x",five,emptyenv);
    PTR(ExtendedEnv) YtoTen = NEW(ExtendedEnv)("y",ten,XtoFive);
    PTR(ExtendedEnv) XtoTrue = NEW(ExtendedEnv)("x",trueVal,emptyenv);
    PTR(FunVal) funVal = NEW(FunVal)("x",parse_str("x+20"),emptyenv);
    PTR(ExtendedEnv) funEnv = NEW(ExtendedEnv)("f",funVal,emptyenv);
    
    
    CHECK(runInterpWithEnv("x+5", XtoFive)=="10");
    CHECK(runInterpWithEnv("x+y+5", YtoTen) == "20");
    CHECK(runInterpWithEnv("_let x=2+y _in x ", YtoTen)=="12");
    CHECK(runInterpWithEnv("_if x _then 1 _else 2", XtoTrue)=="1");
    CHECK(runInterpWithEnv("_let f = _fun(x) 2+x _in f(10)",YtoTen)=="12");
    CHECK(runInterpWithEnv("f(10)", funEnv) == "30");
    

}

