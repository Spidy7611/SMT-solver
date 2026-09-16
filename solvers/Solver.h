#pragma once

#include "../formula/Formula.h"
#include <vector>

enum class SolveResult
{
    SAT,
    UNSAT,
    UNKNOWN
};

class Solver
{
public:
    virtual ~Solver() = default;

    virtual SolveResult solve(const Formula& form) = 0;
};
