#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "../formula/Formula.h"

Formula parseCNF(const std::string& filename) {
    Formula formula;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return formula;
    }

    int maxVar = 0;
    std::string line;
    while (std::getline(file, line)) {
        // Trim szóközök
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty() || line[0] == 'c') continue; // komment

        if (line[0] == 'p') {
            // header sor, nem számít a numVars, majd a legnagyobb literál alapján állítjuk
            continue;
        }

        // Split szóközök mentén
        Clause clause;
        std::istringstream iss(line);
        std::string token;
        while (iss >> token) {
            int lit = std::stoi(token);
            if (lit == 0) break; // klauzula vége
            clause.push_back(lit);
            maxVar = std::max(maxVar, std::abs(lit));
        }

        if (!clause.empty())
            formula.clauses.push_back(clause);
    }

    formula.numVars = maxVar;

    return formula;
}
//SMT
#include <iostream>
#include <string_view>
#include <string>
#include "../types/SMTtypes.h"
#include "../lexer.cpp"

void makeTrees(std::string_view tokenText, Lexer& lexer){
    //func-> funcname to int vmi mappal(gyökerek indexe)
    if (tokenText == "declare-fun") {
            std::cout << "making func: "<<std::string(lexer.next_token().text)<<AST.size()+1<<"\n";
            func.insert({std::string(lexer.next_token().text) ,AST.size()+1});
    } 
    //assert -> assert tömba ami az assertek gyökerének indexét tárolja
    else if (tokenText == "assert") {
        std::cout << "making assert: "<<AST.size()+1<<"\n";
        asserts.push_back(AST.size()+1);
    } 
    //check-sat -> vége
    else if (tokenText == "check-sat") {

    } 
    else {
    std::cout<<"!!Tree type not supported: "<<tokenText << "'\n";
    }
}
void buildTree(std::string_view tokenText){

        //művelet esetén csak építem az AST-t 
        //konstans esetén ha funcban van talán meg éri tárolni
}


int parseSMTLIB(const std::string& filename) {


    size_t countParen =0;
    bool endOfBlock = true;

    std::ifstream file(filename);

    

    if (!file.is_open()) {
        throw std::runtime_error("Nem sikerult megnyitni a fajlt");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string source_code = buffer.str();

    Lexer lexer(source_code.data(), source_code.size());

    Token token = lexer.next_token();
//read row
    while (token.type != TokenType::EndFocal) {
        switch (token.type)
        {
        //( countParen++
        case TokenType::LeftParen:
            if(countParen==0)endOfBlock=true;
            countParen++;
            break;
        //) countParen--
        case TokenType::RightParen:
            countParen--;
            break;
        //indentifier(row types)
        case TokenType::Identifier:
            //ha count paren 0 volt előzőleg
            if(endOfBlock){
                makeTrees(token.text,lexer);
                endOfBlock=false;
            }
            //ha nem 0
            else buildTree(token.text); 
            break;       
        default: std::cout<<"!! Token type not supported: "<< token.text << toString(token.type) << "'\n";
            break;
        }
        

        std::cout << "Token: '" << token.text << "'\n";
        token = lexer.next_token();
    }

    return 0;
}

  // --- Tesztelés / Használat ---
int main() {
    parseSMTLIB("or_monoton_n.txt");

}