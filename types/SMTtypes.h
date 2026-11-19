#include <variant>
#include <vector>
#include <unordered_map>
#include <optional>
#include <cstdint>
#include <iostream>
#include <stack>
//#include "../memory/MemoryPool.h"
   


struct BVLeaf {
    std::vector<bool> constats;
    std::vector<bool> type; // 0 ha konstans 1 ha szabad
    int size;
    BVLeaf(){};
    BVLeaf(int s) : size(s) 
    {
        type.reserve(s);
    }
    void add_c(bool v)
    {
        type.push_back(0);
        constats.push_back(v);
        ++size;
    }
    void add_f(int db)
    {
        type.insert(type.end(), db, 1);
        size += db;
    }
    std::string toString() const { return "BVLeaf"; }
};

struct UnaryOp {
    std::string_view relation;
    int child;
    UnaryOp(int c, std::string_view relation) : child(c), relation(relation) {}
    std::string toString() const { return std::string(relation) + " c= " + std::to_string(child); }
};

struct BinaryOp {
    std::string_view relation;
    int child1;
    int child2;
    BinaryOp(int c1, int c2, std::string_view relation) : child1(c1), child2(c2), relation(relation) {}
    std::string toString() const { return std::string(relation) + " c1= " + std::to_string(child1) + " c2= " + std::to_string(child2); }
};

struct PlaceHolder {
    std::string toString() const { return "PlaceHolder"; }
};

struct Pointer {
    int p;
    Pointer(int p) : p(p) {}
    std::string toString() const { return "Pointer(" + std::to_string(p) + ")"; }
};





std::unordered_map<std::string_view, int> func;
std::vector<int> asserts;
std::vector<std::variant<BVLeaf, UnaryOp, BinaryOp,PlaceHolder,Pointer>> AST;
std::stack<int,std::vector<int>> writePointer;//Az ast nem levéllel lezárt ágait tárolja
std::unordered_map<std::string_view, bool> OpType={
    {"bvor",1},
    {"bvuge",1}
};//0 ha unary 1 ha binary





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