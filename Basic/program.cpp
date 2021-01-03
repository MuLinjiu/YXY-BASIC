/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include <string>
#include "program.h"
#include "statement.h"
#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/tokenscanner.h"

using namespace std;

Program::Program() = default;

Program::~Program() {clear();}

void Program::clear() {
    for (auto it : table) {
        delete it.second.sta;
        it.second.sta = nullptr;
    }
    table.clear();
}

void Program::addSourceLine(int lineNumber, string line) {
     if (Existed(lineNumber)) removeSourceLine(lineNumber);
     table.insert(make_pair(lineNumber,Line(line)));
}

void Program::removeSourceLine(int lineNumber) {
    if (Existed(lineNumber)) {
        delete table[lineNumber].sta;
        table.erase(lineNumber);
    }
}

string Program::getSourceLine(int lineNumber) {
    if (!Existed(lineNumber)) return "";
    return table[lineNumber].source_Line;
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    if (!Existed(lineNumber)) error("[error] SYNTAX ERROR");
    delete table[lineNumber].sta;
    table[lineNumber].sta=stmt;
}

Statement *Program::getParsedStatement(int lineNumber) {
    if (!Existed(lineNumber)) error("[error] SYNTAX ERROR");
    return table[lineNumber].sta;
}

int Program::getFirstLineNumber() {
    if (table.empty()) return -1;
    return table.begin()->first;
}

int Program::getNextLineNumber(int lineNumber) {
    if (table.upper_bound(lineNumber)==table.end()) return -1;
    return table.upper_bound(lineNumber)->first;
}

bool Program::Existed(int lineNumber) {
    if (table.empty() || table.find(lineNumber)==table.end()) {
        return false;
    }
    return true;
}

void Program::List() {
    if (table.empty()) return;
    for (auto it : table) cout<<it.second.source_Line<<endl;
    return;
}

void Program::Run(EvalState &state) {
    if (table.empty()) return;
    auto it = table.begin();
    while (it!=table.end()) {
        try {
            it->second.sta->execute(state);
            it++;
        } catch (ErrorException &ex) {
            TokenScanner scanner;
            if (scanner.getTokenType(ex.getMessage()) == NUMBER) {
                int num=stringToInteger(ex.getMessage());
                if (Existed(num)) {
                    it = table.find(num);
                    continue;
                }
                else error("[error] goto");
            }
            else error(ex.getMessage());
        }
    }
}