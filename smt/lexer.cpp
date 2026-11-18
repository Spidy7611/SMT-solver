#include <string_view>
#include <string>

//lexerhez
enum class TokenType {
    EndFocal,     // Fájl vége (EOF)
    Identifier,   // pl. változónevek, kulcsszavak (pl. x, declare, assert) 
    Value,       // start with #    
    LeftParen,    // (
    RightParen,    // )
    c //_
};

struct Token {
    TokenType type;
    std::string_view text; // Mutató a pufferben lévő eredeti szövegrészletre (nincs másolás!)
};

std::string toString(TokenType type) {
    switch (type) {
        case TokenType::EndFocal:   return "EndFocal";
        case TokenType::Identifier: return "Identifier";
        case TokenType::Value:      return "Value";
        case TokenType::LeftParen:  return "LeftParen";
        case TokenType::RightParen: return "RightParen";
    }
    return "Unknown";
}


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
        if (c == '_') { ptr++; return {TokenType::c, "_"}; }

        //több karakteres
        if(c=='#')return read_it(TokenType::Value);
        return read_it(TokenType::Identifier);




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