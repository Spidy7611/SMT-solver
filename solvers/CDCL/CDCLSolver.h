#pragma once

#include "../Solver.h" 
#include "../../formula/CDCLFormula.h"
#include <vector>

class CDCLSolver : public Solver
{
public:


    SolveResult solve(const Formula& form) override;

private:


    // 1. Kényszerítés: Visszaadja a konfliktust okozó klóz indexét, vagy -1-et
    int propagate(CDCLFormula& f);

    // 2. Konfliktus Analízis: Létrehozza a tanult klózt és kiszámolja a visszaugrási szintet
    void analyzeConflict(CDCLFormula& f, int conflictClauseID, std::vector<int>& learnedClause, int& backtrackLevel);

    // 3. Változó választás: VSIDS heurisztika alapján
    int pickVariable(CDCLFormula& f);



    // Visszalépés a megadott döntési szintre
    void backjump(CDCLFormula& f, int level);

    // Új változó hozzárendelése (szint és indoklás mentésével)
    bool assign(CDCLFormula& f, int lit, int reasonID);

    void verify_and_print_every_clause(const std::vector<std::vector<int>>& clauses, const std::vector<int>& assignment);
};