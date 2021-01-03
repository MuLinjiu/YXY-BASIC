/*
 * File: Basic.cpp
 * ---------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the BASIC interpreter from
 * Assignment #6.
 * [TODO: extend and correct the documentation]
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.h"
#include "parser.h"
#include "program.h"
#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/tokenscanner.h"

#include "../StanfordCPPLib/simpio.h"
#include "../StanfordCPPLib/strlib.h"

using namespace std;

/* Function prototypes */

void processLine(string line, Program &program, EvalState &state);

/* Main program */

int main() {
    EvalState state;
    Program program;
//   cout << "Stub implementation of BASIC" << endl;
    while (true) {
        try {
            processLine(getLine(), program, state);
        } catch (ErrorException &ex) {
            cerr << "Error: " << ex.getMessage() << endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version,
 * the implementation does exactly what the interpreter program
 * does in Chapter 19: read a line, parse it as an expression,
 * and then print the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */
bool isWORD(const string& token);
void processLine(string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);
    if (!scanner.hasMoreTokens()) return;
    string token = scanner.nextToken();
    TokenType type = scanner.getTokenType(token);
    Statement *sta = nullptr;
    switch (type) {
        case NUMBER: {
            int lineNumber = stringToInteger(token);
            if (!scanner.hasMoreTokens()) {
                program.removeSourceLine(lineNumber);
                return;
            }
            try {
                sta=SetSta(scanner,line);
            }
            catch (...) {
                cout << "SYNTAX ERROR" << endl;
                return;
            }
            program.addSourceLine(lineNumber,line);
            program.setParsedStatement(lineNumber,sta);
            return;
        }
        case WORD: {
            if (!isWORD(token)) return;
            switch (token[0]) {
                case 'R': {
                    if(scanner.hasMoreTokens()) {cout << "SYNTAX ERROR" << endl; return;}
                    try {
                        program.Run(state);
                    } catch (ErrorException &ex) {
                        if(ex.getMessage() == "[error] end") {
                            return;
                        }else if(ex.getMessage() == "[error] zero") {
                            cout << "DIVIDE BY ZERO" << endl;
                            return;
                        }else if(ex.getMessage() == "[error] goto"){
                            cout << "LINE NUMBER ERROR" << endl;
                            return;
                        }else{
                            cout << "VARIABLE NOT DEFINED" << endl;
                            return;
                        }
                    }
                    return;
                }
                case 'C': {
                    program.clear();
                    state.clear();
                    return;
                }
                case 'Q': {
                    exit(0);
                }
                case 'H': {
                    return;
                }
                case 'L': {
                    if (token=="LIST") {
                        if(scanner.hasMoreTokens()) {cout << "SYNTAX ERROR" << endl; return;}
                        program.List();
                        return;
                    }
                }
                case 'P': case 'I': {
                    scanner.setInput(line);
                    try {
                        sta = SetSta(scanner,line);
                    } catch (...) {
                        cout << "SYNTAX ERROR" << endl;
                        return;
                    }
                    try {
                        sta->execute(state);
                        delete sta;
                    } catch (ErrorException &ex) {
                        if (ex.getMessage()=="[error] zero") {
                            cout << "DIVIDE BY ZERO" << endl;
                            return;
                        } else {
                            cout << "VARIABLE NOT DEFINED" << endl;
                            return;
                        }
                    }
                    return;
                }
                default: {
                    cout<<"SYNTAX ERROR"<<endl; return;
                }
            }
        }
        default: {
            cout<<"SYNTAX ERROR"<<endl; return;
        }
    }
}

bool isWORD(const string &token){
    if(token == "LET" || token == "REM" || token == "PRINT" || token == "END" || token == "IF" ||
       token == "THEN" || token == "GOTO" || token == "RUN" || token == "LIST" || token == "CLEAR" ||
       token == "QUIT" || token == "HELP" || token == "INPUT") return true;
    return false;
}