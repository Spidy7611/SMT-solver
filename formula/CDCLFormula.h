#pragma once
#include <vector>
#include <algorithm>
#include "WatchedFormula.h"

class CDCLFormula : public WatchedFormula {
public:
    int numVars;
    // A klózokat más adatszerkeuzetként lehetne kezelni?
    //talán ha  atanult klózok külön helyen lennének h a halmaz is tömb lehessen gyorsabb lenne?
    std::vector<std::vector<int>> clauses;
    std::vector<int> assignments; // 0, 1, -1
    std::vector<int> trail;
    
   

    // 1. Döntési szintek: Melyik szinten lett beállítva a változó?
    // (Pl. level[5] = 2 azt jelenti, hogy az x5 változó a 2. döntésnél kapott értéket)
    std::vector<int> decisionLevels;

    // 2. Indoklás (Antecedent): Melyik klóz kényszerítette ezt a változót?
    // Ha döntés volt, akkor -1. Ha unit propagation, akkor a clauseID.
    std::vector<int> reasons;

    // 3. Aktivitás (VSIDS-hez): Melyik változó szerepel sok konfliktusban?
    std::vector<double> activity;

    // 4. Trail limit: Hol kezdődnek az egyes döntési szintek a trail-en?
   
    std::vector<int> trailLim;

    int qhead = 0;

    CDCLFormula(int n, const std::vector<std::vector<int>>& c)
        : WatchedFormula(n), numVars(n), clauses(c) {
        assignments.assign(numVars + 1, 0);
        decisionLevels.assign(numVars + 1, -1);
        reasons.assign(numVars + 1, -1);
        activity.assign(numVars + 1, 0.0);
        
        trail.reserve(numVars);
        trailLim.reserve(numVars);
        initTwoWatchedLiterals(clauses);
    }

    // Új klóz hozzáadása futás közben (tanuláskor)
    void addLearnedClause(const std::vector<int>& newClause) {
        int newID = clauses.size();
        clauses.push_back(newClause);
        // A tanult klózt is azonnal figyelni kell!
        if (newClause.size() >= 2) {
            addWatch(newClause[0], newID, newClause[1]);
            addWatch(newClause[1], newID, newClause[0]);
        }
    }

// --- SEGÉDFÜGGVÉNYEK A SOLVERHEZ ---

    // Jelenlegi döntési szint lekérdezése
    inline int currentLevel() const {
        return trailLim.size();
    }

    // Új döntési szint kezdése
    inline void newDecisionLevel() {
        trailLim.push_back(trail.size());
    }

    // Változó értékének lekérdezése (0, 1, vagy -1)
    inline int value(int lit) const {
        int val = assignments[std::abs(lit)];
        return (lit > 0) ? val : -val; 
    }

    // Kész vagyunk-e?
    inline bool allAssigned() const {
        return trail.size() == numVars;
    }

};
