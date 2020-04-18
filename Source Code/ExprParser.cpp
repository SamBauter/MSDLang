#include <sstream>
#include "ExprParser.hpp"
//#include "catch.hpp"
#include "Expr.h"
#include "op.hpp"


static PTR(Expr) parse_expr(std::istream &in);
static PTR(Expr) parse_oparg(int precedence, std::istream &in);
static PTR(Expr) parse_multicand(std::istream &in);
static PTR(Expr) parse_oparg_n(std::istream &in);
static PTR(BinaryOp) parse_binary_op(int precedence, std::istream &in);
static PTR(Expr) parse_inner(std::istream &in);
static PTR(Expr) parse_number(std::istream &in);
static PTR(Expr) parse_variable(std::istream &in);
static PTR(Expr) parse_let_expr(std::istream &in);
static PTR(Expr) parse_if_expr(std::istream &in);
static PTR(Expr) parse_fun_expr(std::istream &in);
PTR(Expr) parse_str(std::string str);
static std::string parse_keyword(std::istream &in);
static void parse_expected_keyword(std::istream &in, std::string kw, std::string for_kw);
static std::string parse_alphabetic(std::istream &in, std::string prefix);
static char peek_after_spaces(std::istream &in);
static void consume(std::istream &in, char expect);

static void parse_expected_keyword(std::istream &in, std::string kw, std::string for_kw) {
  if (peek_after_spaces(in) != '_')
    throw std::runtime_error("expected `" + kw + "` for `" + for_kw + "`");
  consume(in, '_');
  std::string name = parse_alphabetic(in, "_");
  if (name != kw)
    throw std::runtime_error("expected `" + kw + "` for `" + for_kw + "`");
}




// Take an input stream that contains an expression,
// and returns the parsed representation of that expression.
// Throws `runtime_error` for parse errors.
PTR(Expr) parse(std::istream &in) {
  PTR(Expr) e = parse_expr(in);
  
  char c = in.get();
  if (!in.eof())
    throw std::runtime_error((std::string)"expected end of file at " + c);
  
  return e;
}

static PTR(Expr) parse_expr(std::istream &in){
    return parse_oparg(0, in);
}


static PTR(Expr) parse_oparg(int precedence, std::istream &in){
    if(precedence>=op_tables.size()){
        return parse_oparg_n(in);
    }else{
        PTR(Expr) expr = parse_oparg(precedence+1, in);
        PTR(BinaryOp) op = parse_binary_op(precedence, in);
        
        if(op!=nullptr) {
            PTR(Expr) rhs = parse_oparg(precedence, in);
            return NEW(BinaryOpExpr)(op,expr,rhs);
        }else
            return expr;
    }
}

static PTR(BinaryOp) parse_binary_op(int precedence, std::istream &in){
    for(PTR(BinaryOp) op: op_tables[precedence]){
        std::string op_name = op->to_string();
        int i = 0;
        char next_ch = peek_after_spaces(in);
        
        while(next_ch == op_name[i]){
            consume(in, next_ch);
            if(++i == op_name.length())
                return op;
            next_ch = in.peek();
        }
        
        while (i-- >0)
            in.putback(op_name[i]);
    }
    return nullptr;
}


    static PTR(Expr) parse_oparg_n(std::istream &in){
        PTR(Expr) e = parse_inner(in);
        while(peek_after_spaces(in) == '('){
            consume(in, '(');
            PTR(Expr) actualArg = parse_expr(in);
            e = NEW(CallExpr)(e,actualArg);
            if(peek_after_spaces(in)==')'){
                consume(in, ')');
            }else{
                throw std::runtime_error("Expected close paren after call expr");
        }
        }
        return e;
    }


static PTR(Expr) parse_multicand(std::istream &in){
    PTR(Expr) e = parse_inner(in);
    while(peek_after_spaces(in) == '('){
        consume(in, '(');
        PTR(Expr) actualArg = parse_expr(in);
        e = NEW(CallExpr)(e,actualArg);
        if(peek_after_spaces(in)==')'){
            consume(in, ')');
        }else{
            throw std::runtime_error("Expected close paren after call expr");
    }
    }
    return e;
}



// Parses something with no immediate `+` or `*` from `in`.
static PTR(Expr) parse_inner(std::istream &in) {
  PTR(Expr) e;

  char c = peek_after_spaces(in);
  
  if (c == '(') {
    c = in.get();
    e = parse_expr(in);
    c = peek_after_spaces(in);
      if (c == ')'){
      c = in.get();
      }else{
          throw std::runtime_error("expected a close parenthesis");
      }
  }else if (c=='-'||isdigit(c)) {
    e = parse_number(in);
  } else if (isalpha(c)) {
    e = parse_variable(in);
  } else if (c == '_') {
    std::string keyword = parse_keyword(in);
      if (keyword == "_let"){
          e =  parse_let_expr(in);
      }
    else if (keyword == "_if"){
        e= parse_if_expr(in);
    }
    else if(keyword == "_true"){
        e = NEW(BoolExpr)(true);
    }
    else if(keyword == "_false"){
        e = NEW(BoolExpr)(false);
      }
    else if(keyword =="_fun"){
        e = parse_fun_expr(in);
    }
    else{
        throw std::runtime_error((std::string)"unexpected keyword " + keyword);
        }
    }
  else {
    throw std::runtime_error((std::string)"expected a digit or open parenthesis at " + c);
  }
  return e;
}
    //Parses fun_exprs with variables in their formal args
    static PTR(Expr) parse_fun_expr(std::istream &in){
        char funOpenParen = peek_after_spaces(in);
        if(funOpenParen!='('){
            throw std::runtime_error("expected an opening paren before the formal arg of a function");
        }else{
            in.get();
        }
        
        char formalArgStart = peek_after_spaces(in);
        if(!isalpha(formalArgStart)){
            throw std::runtime_error("expected an alphabetical start to formal arg in fun statement");
        }
        std::string formalArgName =  parse_alphabetic(in,"");
        
        char funCloseParen = peek_after_spaces(in);
        if(funCloseParen!=')'){
               throw std::runtime_error("expected an closing paren before the formal arg of a function");
        }else{
            in.get();
        }
        PTR(Expr) funBody = parse_expr(in);
        return NEW(FunExpr)(formalArgName,funBody);
    }


    

//Parses an If Expr
static PTR(Expr) parse_if_expr(std::istream &in){
    PTR(Expr) condition = parse_expr(in);
    char thenUnderscore = peek_after_spaces(in);
    if(thenUnderscore!='_'){
        throw std::runtime_error("expected an _ operator: before then in if statement");
    }
    std::string then_keyword = parse_keyword(in);
    if(then_keyword != "_then"){
        throw std::runtime_error("expected _then after condition in if statement");
    }
    PTR(Expr) trueConseq = parse_expr(in);
    
    char elseUnderscore = peek_after_spaces(in);
    if(elseUnderscore!='_'){
        throw std::runtime_error("expected an _ operator: before else in if statement");
    }
    std::string else_keyword = parse_keyword(in);
    if(else_keyword != "_else"){
        throw std::runtime_error("expected _else after condition in if statement");
    }
    PTR(Expr) falseConseq = parse_expr(in);
    return NEW(IfExpr)(condition,trueConseq,falseConseq);
}



// Parses a number, assuming that `in` starts with a digit.
static PTR(Expr) parse_number(std::istream &in) {
  int num = 0;
    if(peek_after_spaces(in)=='-'){
        in.get();
        in >> num;
        num = -num;
        return NEW(NumExpr)(num);
    }else{
        in >> num;
        return NEW(NumExpr)(num);
    }
}


    

// Parses an expression, assuming that `in` starts with a
// letter.
static PTR(Expr) parse_variable(std::istream &in) {
  return NEW(VarExpr)(parse_alphabetic(in, ""));
}

/*static PTR(Expr) parse_let_expr(std::istream &in){
    char varStart = peek_after_spaces(in);
    if(!isalpha(varStart)){
        throw std::runtime_error("expected an alphabetical start to var name in let statement");
    }
    std::string varName =  parse_alphabetic(in,"");
    
    char assignOperator = peek_after_spaces(in);
    if(assignOperator!='='){
        throw std::runtime_error("expected an assignment operator: = in let statement");
    }
    assignOperator=in.get();
    PTR(Expr) rhs = parse_expr(in);
    char keywordUnderscore = peek_after_spaces(in);
    if(keywordUnderscore!='_'){
        throw std::runtime_error("expected an _ operator: after variableValExpr in let statement");
    }
    std::string in_keyword = parse_keyword(in);
    
    PTR(Expr) body = parse_expr(in);
    return NEW(LetExpr)(varName,rhs,body);
}*/
static PTR(Expr)parse_let_expr(std::istream &in) {
  char next_ch = peek_after_spaces(in);
  if (!isalpha(next_ch))
    throw std::runtime_error("expected a name after `_let`");
  std::string var = parse_alphabetic(in, "");
  
  if (peek_after_spaces(in) != '=')
    throw std::runtime_error("expected a `=` for `_let` binding");
  consume(in, '=');

  PTR(Expr)rhs = parse_expr(in);
  
  parse_expected_keyword(in, "_in", "_let");
  
  PTR(Expr)body = parse_expr(in);
  
  return NEW(LetExpr)(var, rhs, body);
}


static std::string parse_keyword(std::istream &in) {
  consume(in, '_');
  return parse_alphabetic(in, "_");
}

// Parses an expression, assuming that `in` starts with a
// letter.
static std::string parse_alphabetic(std::istream &in, std::string prefix) {
  std::string name = prefix;
  while (1) {
    char c = in.peek();
    if (!isalpha(c))
      break;
    name += in.get();
  }
  return name;
}


// Like in.peek(), but consume an whitespace at the
// start of `in`
static char peek_after_spaces(std::istream &in) {
  char next_ch;
  while (1) {
    next_ch = in.peek();
    if (!isspace(next_ch))
      break;
    consume(in, next_ch);
  }
  return next_ch;
}

static void consume(std::istream &in, char expect) {
  char next_ch = in.get();
  assert(next_ch == expect);
}
        

/* for tests */
PTR(Expr) parse_str(std::string s) {
  std::istringstream in(s);
  return parse(in);
}

std::string optimizer(std::istream &in){
    PTR(Expr) exprVal = parse(in);
    return exprVal->to_string();
}



