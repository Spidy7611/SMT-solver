#include <variant>
#include <vector>
#include <optional>
#include <cstdint>
#include <iostream>
#include "../memory/MemoryPool.h"
   


//lexerhez
enum class TokenType {
    Unknown,
    EndFocal,     // Fájl vége (EOF)
    Identifier,   // pl. változónevek, kulcsszavak (pl. x, declare, assert) 
    Value,       // start with #
    op,    
    LeftParen,    // (
    RightParen    // )
};
struct Token {
    TokenType type;
    std::string_view text; // Mutató a pufferben lévő eredeti szövegrészletre (nincs másolás!)
};

enum class RelationKind {
    BV_ADD, 
    BV_AND 
};



struct BVLeaf {
    //BitVector val;
    // Saját konstruktor
    std::optional<int> val;
    int size;
    BVLeaf(int v,int s) : val(v) , size(s) {} 
};

struct UnaryOp {
    RelationKind relation;
    int child;
    UnaryOp(int c, RelationKind relation) : child(c),relation(relation) {}

};

struct BinaryOp {
    RelationKind relation;
    int child1;
    int child2;
    BinaryOp(int c1, int c2,RelationKind relation) : child1(c1), child2(c2),relation(relation) {}
};
std::vector<std::variant<BVLeaf, UnaryOp, BinaryOp>> AST;
/*
class BitVector {
    int length_in_bits;//bit-ben
    int indx;
    MemoryPool& pool;
public:
    BitVector(int l, MemoryPool& pool) : length_in_bits(l),indx(pool.allocate(l)),pool(pool){} 
    
// Írás:
void setValue(size_t val) {
    if (length_in_bits <= 8) {
        pool.getAccess<uint8_t>(indx) = static_cast<uint8_t>(val);
    } else if (length_in_bits <= 16) {
        pool.getAccess<uint16_t>(indx) = static_cast<uint16_t>(val);
    } else if (length_in_bits <= 32) {
        pool.getAccess<uint32_t>(indx) = static_cast<uint32_t>(val);
    } else {
        pool.getAccess<uint64_t>(indx) = val;
    }
}

// Olvasás 
uint64_t getValue() const {
    if (length_in_bits <= 8)  return pool.getAccess<uint8_t>(indx);
    if (length_in_bits <= 16) return pool.getAccess<uint16_t>(indx);
    if (length_in_bits <= 32) return pool.getAccess<uint32_t>(indx);
    return pool.getAccess<uint64_t>(indx);
}
};*/