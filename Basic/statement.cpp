/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include <string>
#include "statement.h"
#include "../StanfordCPPLib/tokenscanner.h"
#include "../StanfordCPPLib/simpio.h"

using namespace std;

/* Implementation of the Statement class */

Statement::Statement() {
    /* Empty */
}

Statement::~Statement() {
    /* Empty */
}

REMSta::REMSta() {}
REMSta::~REMSta() {}
void REMSta::execute(EvalState &state) {}

LETSta::LETSta(Expression *expression) : exp(expression) {}
LETSta::~LETSta() { delete exp; }
void LETSta::execute(EvalState &state) {
    exp->eval(state);
}

PRINTSta::PRINTSta(Expression *expression):exp(expression) {}
PRINTSta::~PRINTSta() { delete exp; }
void PRINTSta::execute(EvalState &state) { cout<<exp->eval(state)<<endl; }

INPUTSta::INPUTSta(string &x):name(x) {}
INPUTSta::~INPUTSta() {}
void INPUTSta::execute(EvalState &state) {
    cout<< " ? ";
    string token; TokenType type; TokenScanner scanner;
    scanner.ignoreWhitespace(); scanner.scanNumbers();
    int value,flag=1;
    while (true) {
        scanner.setInput(getLine());
        if (!scanner.hasMoreTokens()) continue;
        token = scanner.nextToken();
        if (token=="-") {
            if (!scanner.hasMoreTokens()) continue;
            token = scanner.nextToken();
            flag=-1;
        }
        type = scanner.getTokenType(token);
        if (type != NUMBER || scanner.hasMoreTokens()) {cout << "INVALID NUMBER" << endl << " ? "; continue;}
        try {
            value = stringToInteger(token);
        } catch (...) {
            cout << "INVALID NUMBER" << endl << " ? ";continue;
        }
        state.setValue(name,value*flag);
        break;
    }
}

ENDSta::ENDSta() {}
ENDSta::~ENDSta() {}
void ENDSta::execute(EvalState &state) {
    error("[error] end");
}

GOTOSta::GOTOSta(int to): to(to){}
GOTOSta::~GOTOSta() {}
void GOTOSta::execute(EvalState &state) {
    error(integerToString(to));
}

IFSta::IFSta(string op,Expression *l,Expression *r,GOTOSta *go):op(op),l(l),r(r),go(go) {}
IFSta::~IFSta() {
    delete l; delete r; delete go;
}
void IFSta::execute(EvalState &state) {
    bool flag;
    int l_ans=l->eval(state),r_ans=r->eval(state);
    switch (op[0]) {
        case '=': { flag = (l_ans==r_ans); break; }
        case '<': { flag = (l_ans<r_ans); break; }
        case '>': { flag = (l_ans>r_ans); break; }
    }
    if (flag) { go->execute(state); }
}


