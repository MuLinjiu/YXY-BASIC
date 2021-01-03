/*
 * File: parser.cpp
 * ----------------
 * Implements the parser.h interface.
 */

#include <iostream>
#include <string>

#include "exp.h"
#include "parser.h"

#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/strlib.h"
#include "../StanfordCPPLib/tokenscanner.h"
using namespace std;

/*
 * Implementation notes: parseExp
 * ------------------------------
 * This code just reads an expression and then checks for extra tokens.
 */

Expression *parseExp(TokenScanner & scanner) {
   Expression *exp = readE(scanner);
   if (scanner.hasMoreTokens()) {
      error("parseExp: Found extra token: " + scanner.nextToken());
   }
   return exp;
}

/*
 * Implementation notes: readE
 * Usage: exp = readE(scanner, prec);
 * ----------------------------------
 * This version of readE uses precedence to resolve the ambiguity in
 * the grammar.  At each recursive level, the parser reads operators and
 * subexpressions until it finds an operator whose precedence is greater
 * than the prevailing one.  When a higher-precedence operator is found,
 * readE calls itself recursively to read in that subexpression as a unit.
 */

Expression *readE(TokenScanner & scanner, int prec) {
   Expression *exp = readT(scanner);
   string token;
   while (true) {
      token = scanner.nextToken();
      int newPrec = precedence(token);
      if (newPrec <= prec) break;
      Expression *rhs = readE(scanner, newPrec);
      exp = new CompoundExp(token, exp, rhs);
   }
   scanner.saveToken(token);
   return exp;
}

/*
 * Implementation notes: readT
 * ---------------------------
 * This function scans a term, which is either an integer, an identifier,
 * or a parenthesized subexpression.
 */

Expression *readT(TokenScanner & scanner) {
   string token = scanner.nextToken();
   TokenType type = scanner.getTokenType(token);
   if (type == WORD) return new IdentifierExp(token);
   if (type == NUMBER) return new ConstantExp(stringToInteger(token));
   if (token != "(") error("Illegal term in expression");
   Expression *exp = readE(scanner);
   if (scanner.nextToken() != ")") {
      error("Unbalanced parentheses in expression");
   }
   return exp;
}

/*
 * Implementation notes: precedence
 * --------------------------------
 * This function checks the token against each of the defined operators
 * and returns the appropriate precedence value.
 */

int precedence(string token) {
   if (token == "=") return 1;
   if (token == "+" || token == "-") return 2;
   if (token == "*" || token == "/") return 3;
   return 0;
}
bool isWORD(const string &token);
Statement *SetSta(TokenScanner &scanner,string line) {
    Expression *exp;
    string token = scanner.nextToken();
    if (!isWORD(token)) {
        error("[error] SYNTAX ERROR");
    }
    switch (token[0]) {
        case 'R' : return new REMSta();
        case 'L' : {
            exp = parseExp(scanner);
            if(exp->getType() != COMPOUND){error("[error] SYNTAX ERROR");}
            if(((CompoundExp *)exp)->getOp() != "=") error("[error] SYNTAX ERROR");
            if((((CompoundExp *)exp)->getLHS())->getType() != IDENTIFIER) error("[error] SYNTAX ERROR");
            if(isWORD(((IdentifierExp *)(((CompoundExp *)exp)->getLHS()))->getName())) error("[error] SYNTAX ERROR");
            return new LETSta(exp);
        }
        case 'P' : {
            exp = parseExp(scanner);
            if((exp->getType() == COMPOUND) && (((CompoundExp *)exp)->getOp() == "=")) {error("[error] SYNTAX ERROR");}
            return new PRINTSta(exp);
        }
        case 'E' : {
            if(scanner.hasMoreTokens()){error("[error] SYNTAX ERROR");}
            return new ENDSta();
        }
        case 'G' : {
            if (!scanner.hasMoreTokens()) error("[error] SYNTAX ERROR");
            token = scanner.nextToken();
            if (scanner.getTokenType(token) != NUMBER) error("[error] SYNTAX ERROR");
            if (scanner.hasMoreTokens()) error("[error] SYNTAX ERROR");
            return new GOTOSta(stringToInteger(token));
        }
        case 'I' : {
            if (token=="INPUT") {
                if (!scanner.hasMoreTokens()) error("[error] SYNTAX ERROR");
                token = scanner.nextToken();
                if (scanner.getTokenType(token) != WORD) error("[error] SYNTAX ERROR");
                if (scanner.hasMoreTokens()) error("[error] SYNTAX ERROR");
                return new INPUTSta(token);
            }
            else {
                if (!scanner.hasMoreTokens()) error("[error] SYNTAX ERROR");
                string op; Expression *l,*r; GOTOSta *go;
                if (line.find('=') != string::npos) {
                    if (line.find_first_of('=') != line.find_last_of('=')) error("[error] SYNTAX ERROR");
                    op = "=";
                    string s;
                    while (scanner.hasMoreTokens()) {
                        token=scanner.nextToken();
                        if (token=="=") break;
                        s+=token; s+=" ";
                    }
                    try {
                        r = readE(scanner);
                    } catch (...) {
                        delete r; error("[error] SYNTAX ERROR");
                    }
                    token = scanner.nextToken();
                    if (token != "THEN") {delete r;error("[error] SYNTAX ERROR");}
                    token = scanner.nextToken();
                    TokenType type = scanner.getTokenType(token);
                    if (type != NUMBER) { delete r; error("[error] SYNTAX ERROR");}
                    if (scanner.hasMoreTokens()) { delete r; error("[error] SYNTAX ERROR");}
                    int num;
                    try {
                        num = stringToInteger(token);
                    } catch (...) {
                        delete r;
                        error("[error] SYNTAX ERROR");
                    }
                    scanner.setInput(s);
                    try{
                        l = readE(scanner);
                    } catch (...) {
                        delete r;
                        error("[error] SYNTAX ERROR");
                    }
                    go = new GOTOSta(num);
                    return new IFSta(op, l, r, go);
                }
                else {
                    l = readE(scanner);
                    op = scanner.nextToken();
                    if (op != "<" && op != ">") { delete l; error("[error] SYNTAX ERROR"); }
                    try {
                        r = readE(scanner);
                    } catch (...) {
                        delete r; error("[error] SYNTAX ERROR");
                    }
                    token = scanner.nextToken();
                    if (token != "THEN") {delete l;delete r;error("[error] SYNTAX ERROR");}
                    token = scanner.nextToken();
                    TokenType type = scanner.getTokenType(token);
                    if (type != NUMBER) { delete l; delete r; error("[error] SYNTAX ERROR");}
                    if (scanner.hasMoreTokens()) {delete l; delete r; error("[error] SYNTAX ERROR");}
                    int num;
                    try {
                        num = stringToInteger(token);
                    } catch (...) {
                        delete l; delete r;
                        error("[error] SYNTAX ERROR");
                    }
                    go = new GOTOSta(num);
                    return new IFSta(op, l, r, go);
                }
            }
        }
        default: error("[error] SYNTAX ERROR");
    }
}