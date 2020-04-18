#include <iostream>
#include <sstream>
//#include "catch.hpp"



static int calc_expr(std::istream &in, std::ostream &err, bool &ok);
static int calc_addend(std::istream &in, std::ostream &err, bool &ok);
static int calc_number_or_paren(std::istream &in, std::ostream &err, bool &ok);
static int calc_number(std::istream &in);
static char peek_next(std::istream &in);

/* The grammar that we're trying to parse is
     <expr> = <addend>
            | <expr> + <expr>
     <addend> = <number>
              | (<expr>)
              | <addend> * <addend>
   On further reflection, we can adjust this one step closer to a
   parser, since we're happy to have left-associative `+` and `*`:
     <expr> = <addend>
            | <addend> + <expr>
     <addend> = <number-or-paren>
              | <number-or-paren> * <addend>
     <number-or-paren> = <number>
                       | (<expr>)
   This revised deifnition of <expr> matches the same strings as the
   original, but it more directly suggests a parsing strategy:
   * An <expr> always has to start with an <addend>. We may immediate
     see a `+`, in which case there's another <expr>, and that's all.
     If we don't see a `+`, then we must be done with this <expr>.
   * An <addend> always has to start with an <number-or-paren>. We may
     see a `*`, in which case there's another <addend> and that's all.
     If we don't see a `*`, then we must be done with this <addend>.
   * For a <number-or-paren>, we can tell whether it's a <number> or
     <paren> by looking at one character.
   The parser can work by having a function for every <....> name, and
   the functions call each other to reflect the way the <....>
   grammars refer to each other. For example, the <expr> function will
   start by calling the <addend> function, and then it will check for
   a `+` and call the <expr> function (itself) if it finds a `+`.
*/

int calc(std::istream &in, std::ostream &err) {
    bool ok = true;

    int num = calc_expr(in, err, ok);
    if (!ok) return -1;

    char c = peek_next(in);
    if ((c != '\n') && !in.eof()) {
        err << "expected an end of line\n";
        return -1;
    }
    return num;
}



// Take an input stream that starts with an expression,
// and returns the value of that expression. Consume the
// largest initial expression possible.
// Returns -1 and sets `ok` to false on error printed to `err`
static int calc_expr(std::istream &in, std::ostream &err, bool &ok) {
    // Since an <expr> starts with an <addend>, call `calc_addend` here
    int addendInt =calc_addend(in, err, ok);
    int expressionInt=0;

    char c = peek_next(in);
    if (c == '+') {
        in >> c;
        // Must have another <expr> here, so call `calc_expr`
        expressionInt =calc_expr(in,err,ok);
    }

    return addendInt+expressionInt;
}

static int calc_addend(std::istream &in, std::ostream &err, bool &ok) {
    int numOrParenValue;
    // Since an <expr> starts with an <number-or-paren>, call `calc_number_or_paren` here
    numOrParenValue = calc_number_or_paren(in,err,ok);

    char c = peek_next(in);
    int multiplierVal=1;
    if (c == '*') {
        in >> c;
        // Must have another <addend> here, so call `calc_addend`
        multiplierVal=calc_addend(in,err,ok);
    }
    return numOrParenValue*multiplierVal;
}

static int calc_number_or_paren(std::istream &in, std::ostream &err, bool &ok) {
    int num;
    char c = peek_next(in);
    if (c == '(') {
        in >> c;
        num = calc_expr(in, err, ok);
        // After that, there should be a `)`, and we're done
        c = peek_next(in);
        if(c !=')'){
            err << "expected a close parenthesis\n";
            ok = false;
        }
        else{
            in >> c;
        }
    } else if (isdigit(c)) {
        num = calc_number(in);
    }
    else {
        err << "expected a digit, variable or open parenthesis at " << c << "\n";
        ok = false;
        return -1;
    }

    return num;
}

//Reads a number from the input stream as an int.
static int calc_number(std::istream &in) {
    int num;
    in >> num;
    return num;
}

// This function peeks the next character.
// If you want to allow whitespace, this is a good place
// to skip over it
static char peek_next(std::istream &in) {
    char c = in.peek();
    while(c==' '||c=='\n'){
        in.get();
        c= in.peek();
    }
    return c;
}

/* for tests */
static int calc_str(std::string s) {
    std::istringstream in(s);
    return calc(in, std::cerr);
}

/* for tests */
static std::string calc_str_error(std::string s) {
    std::istringstream in(s);
    std::ostringstream err(std::ios_base::out);
    (void)calc(in, err);
    return err.str();
}

static char peek_next_str(std::string s){
    std::istringstream in(s);
    return peek_next(in);


}

/*
TEST_CASE( "See if this works" ) {

CHECK ( calc_str_error(" ( 1 ") == "expected a close parenthesis\n" );

CHECK( calc_str("10") == 10 );
CHECK( calc_str("(10)") == 10 );
CHECK( calc_str("10+1") == 11 );
CHECK( calc_str("(10+1)") == 11 );
CHECK( calc_str("(10)+1") == 11 );
CHECK( calc_str("10+(1)") == 11 );
CHECK( calc_str("1+2*3") == 7 );
CHECK( calc_str("1*2+3") == 5 );
CHECK( calc_str("4*2*3") == 24 );
CHECK( calc_str("4+2+3") == 9 );
CHECK( calc_str("4*(2+3)") == 20 );
CHECK( calc_str("(2+3)*4") == 20 );

CHECK ( calc_str_error("x") == "expected a digit, variable or open parenthesis at x\n" );
CHECK ( calc_str_error("(1") == "expected a close parenthesis\n" );


CHECK( peek_next_str(" 15 ") == '1');
CHECK( calc_str(" 10 ") == 10 );
CHECK( calc_str(" (  10 ) ") == 10 );
CHECK( calc_str(" 10  + 1") == 11 );
CHECK( calc_str(" ( 10 + 1 ) ") == 11 );
CHECK( calc_str(" 11 * ( 10 + 1 ) ") == 121 );
CHECK( calc_str(" ( 11 * 10 ) + 1 ") == 111 );
CHECK( calc_str(" 1 + 2 * 3 ") == 7 );

CHECK ( calc_str_error(" x ") == "expected a digit, variable or open parenthesis at x\n" );
CHECK ( calc_str_error(" ( 1 ") == "expected a close parenthesis\n" );

CHECK ( calc_str("(10+10*2+1)*(4+(2+1*2))+1") == 249 );
}
*/

