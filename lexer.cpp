#include <iostream>
#include <string_view>
#include <vector>
#include <cctype>

// 1. Definiáld a token típusokat a saját nyelvednek megfelelően
enum class TokenType {
    Unknown,
    EndFocal,     // Fájl vége (EOF)
    Identifier,   // pl. változónevek, kulcsszavak (pl. x, declare, assert)
    Number,       // pl. 42, 123
    String,       // "szöveg literál"
    Plus,         // +
    Minus,        // -
    Assign,       // =
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
        if (c == '(') { ptr++; return {TokenType::LeftParen, "("}; }
        if (c == ')') { ptr++; return {TokenType::RightParen, ")"}; }
        if (c == '+') { ptr++; return {TokenType::Plus, "+"}; }
        if (c == '-') { ptr++; return {TokenType::Minus, "-"}; }
        if (c == '=') { ptr++; return {TokenType::Assign, "="}; }

        // --- String literálok (idézőjelek között) ---
        if (c == '"') {
            return read_string();
        }

        // --- Számok (számjeggyel kezdődőek) ---
        if (std::isdigit(static_cast<unsigned char>(c))) {
            return read_number();
        }

        // --- Azonosítók / Kulcsszavak (betűvel vagy megengedett karakterrel kezdődőek) ---
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            return read_identifier();
        }

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
            else if (c == ';') { // Példa kommentre (mint az SMT-LIB-ben vagy Assemblyben)
                while (ptr < end && *ptr != '\n') {
                    ptr++;
                }
            } 
            else {
                break;
            }
        }
    }

    Token read_identifier() {
        const char* start = ptr;
        while (ptr < end) {
            char c = *ptr;
            // Itt adhatod meg, mi tartozhat bele egy azonosítóba (pl. betűk, számok, kötőjel, aláhúzás)
            if (std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-') {
                ptr++;
            } else {
                break;
            }
        }
        return {TokenType::Identifier, std::string_view(start, ptr - start)};
    }

    Token read_number() {
        const char* start = ptr;
        while (ptr < end && std::isdigit(static_cast<unsigned char>(*ptr))) {
            ptr++;
        }
        return {TokenType::Number, std::string_view(start, ptr - start)};
    }

    Token read_string() {
        const char* start = ptr;
        ptr++; // Átlépjük a nyitó idézőjelet
        while (ptr < end && *ptr != '"') {
            // Itt kezelhető az escape karakter (pl. \") ha szükséges
            ptr++;
        }
        if (ptr < end) {
            ptr++; // Átlépjük a záró idézőjelet is
        }
        return {TokenType::String, std::string_view(start, ptr - start)};
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