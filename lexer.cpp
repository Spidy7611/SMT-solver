#include <iostream>
#include <string_view>
#include <vector>
#include <cctype>

// 1. Definiáld a token típusokat a saját nyelvednek megfelelően
enum class TokenType {
    Unknown,
    EndFocal,     // Fájl vége (EOF)
    Identifier,   // pl. változónevek, kulcsszavak (pl. x, declare, assert) 
    Value,       // start with #
    op,    
    LeftParen,    // (
    RightParen    // )
};



// 2. A Token struktúra
struct Token {
    TokenType type;
    std::string_view text; // Mutató a pufferben lévő eredeti szövegrészletre (nincs másolás!)
};

// 3. A Lexer osztály vváza
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
        if (c == '(') { ptr++;if(countParen==0)wasNull=true;countParen++ return {TokenType::LeftParen, "("}; }
        if (c == ')') { ptr++;countParen-- return {TokenType::RightParen, ")"}; }

        //több karakteres
        if(c=='#')read_it(TokenType::Value)
        read_it(TokenType::Identifier)




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
            if (c != ' ' || c != '(' || c != '(') {
                ptr++;
            } else {
                ptr--;//hátha () van
                break;
            }
        }
        return {t, std::string_view(start, ptr - start)};
    }

};

// --- Tesztelés / Használat ---
int main() {
    // Minta bemenet, amit a lexernek átadunk
    const char* source_code = 
        "; Ez egy teszt komment\n"
        "(declare-fun x () (_ BitVec 32))\n"
        "(= x 100)";

    Lexer lexer(source_code, std::string_view(source_code).size());

    Token token = lexer.next_token();
    while (token.type != TokenType::EndFocal) {
        std::cout << "Típus: " << token_type_to_string(token.type) 
                  << " \t| Érték: '" << token.text << "'\n";
        token = lexer.next_token();
    }

    return 0;
}