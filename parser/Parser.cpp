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


class Lexer {
    const char* ptr;
    const char* end;


public:
    Lexer(const char* data, size_t size) : ptr(data), end(data + size) {}

    Token next_token() {
        skip_whitespace_and_comments();

        // Ellenőrizzük, elértük-e a fájl végét
        if (ptr >= end) {
            return {TokenType::EndFocal, {}};
        }

        char c = *ptr;

        // --- Egykarakteres tokenek (Operátorok, Zárójelek) ---
        if (c == '(') { ptr++; return {TokenType::LeftParen, "("}; }
        if (c == ')') { ptr++; return {TokenType::RightParen, ")"}; }

        //több karakteres
        if(c=='#')return read_it(TokenType::Value);
        return read_it(TokenType::Identifier);




        // Ha ismeretlen karakterbe ütközünk
        ptr++;
        return {TokenType::Unknown, std::string_view(ptr - 1, 1)};
    }

private:

    // Whitespace-ek (szóköz, tab, új sor) és opcionális kommentek átugrása
    void skip_whitespace_and_comments() {
        while (ptr < end) {
            char c = *ptr;
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                ptr++;
            } 
            else if (c == ';') { // komment
                while (ptr < end && *ptr != '\n') {
                    ptr++;
                }
            } 
            else {
                
                break;
            }
        }
    }

Token read_it(TokenType t) {
    const char* start = ptr;

    while (ptr < end) {
        char c = *ptr;

        if (c == ' ' || c == '\t' || c == '\n' ||
            c == '\r' || c == '(' || c == ')') {
            break;
        }

        ptr++;
    }

    return {t, std::string_view(start, ptr - start)};
}
};


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
        //indentifier
        case TokenType::Identifier:
            //ha count paren 0 volt előzőleg
            if(endOfBlock)makeTrees(token.text);
            //ha nem 0
            else buildTree(token.text);


            
        
        default:
            break;
        }
        
        
        

            //assert -> assert tömba ami az assertek gyökerének indexét tárolja
            //func-> funcname to int vmi mappal(gyökerek indexe)
            //check-sat -> vége
        //művelet esetén csak építem az AST-t 
        //konstans esetén ha funcban van talán meg éri tárolni
        std::cout << "Token: '" << token.text << "'\n";
        token = lexer.next_token();
    }

    return 0;
}
void makeTrees(std::string_view tokenText){
    if (tokenText == "assert") {

    } 
    else if (tokenText == "check-sat") {

    } 
    else if (tokenText == "declare-fun") {

    } 
    else {
    // Egyéb azonosító
    }
}
void buildTree(std::string_view tokenText){

}
  // --- Tesztelés / Használat ---
int main() {
    parseSMTLIB("or_monoton_n.txt");

}