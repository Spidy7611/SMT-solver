#pragma once
#include <vector>
//dpll miatt vector nem tömb
using Literal = int;
using Clause = std::vector<Literal>;

struct Formula
{
    int numVars;
    std::vector<Clause> clauses;

    Formula() : numVars(0) {}
};