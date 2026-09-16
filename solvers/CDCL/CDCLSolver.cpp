#include "CDCLSolver.h"
#include <cmath>
#include <algorithm>
#include <iostream>

// --- Alapfüggvények ---

bool CDCLSolver::assign(CDCLFormula& f, int lit, int reasonID) {
    int var = std::abs(lit);
    if (f.assignments[var] != 0) return false;
    
    f.assignments[var] = (lit > 0) ? 1 : -1;
    f.decisionLevels[var] = f.currentLevel();
    f.reasons[var] = reasonID;
    f.trail.push_back(lit);
    
    return true;
}

void CDCLSolver::backjump(CDCLFormula& f, int level) {
    // Kiszámítjuk, hogy a trail-en meddig kell visszavágni az értékadásokat
    int limit = (level < f.trailLim.size()) ? f.trailLim[level] : f.trail.size();
    
    while (f.trail.size() > limit) {
        int lit = f.trail.back();
        f.trail.pop_back();
        
        int var = std::abs(lit);
        f.assignments[var] = 0;
        f.decisionLevels[var] = -1;
        f.reasons[var] = -1;
    }
    
    f.trailLim.resize(level);
    f.qhead = f.trail.size(); // A propagációs mutatót is vissza kell állítani
}

int CDCLSolver::pickVariable(CDCLFormula& f) {
    int bestVar = -1;
    double maxActivity = -1.0;
    
    // VSIDS: A legmagasabb aktivitású, még beállítatlan változó kiválasztása
    for (int i = 1; i <= f.numVars; i++) {
        if (f.assignments[i] == 0) {
            if (bestVar == -1 || f.activity[i] > maxActivity) {
                bestVar = i;
                maxActivity = f.activity[i];
            }
        }
    }
    
    // Alapértelmezett polaritás: a heurisztika alapján próbálkozunk először a negatívval
    return -bestVar; 
}


// --- 1. Kényszerítés (BCP - Boolean Constraint Propagation) ---

int CDCLSolver::propagate(CDCLFormula& f) {
    while (f.qhead < (int)f.trail.size()) {
        int p = f.trail[f.qhead++];
        int falseLit = -p;
        int watchIdx = f.litToIdx(falseLit);
        
        auto& ws = f.watches[watchIdx];
        //Two-Pointer Technique
        int i = 0, j = 0;
        
        while (i < ws.size()) {
            Watcher w = ws[i];
            int blocker = w.blockingLiteral;
            
            // Ha a blocking literal igaz, a klóz már biztosan igaz, ugorhatunk
            if (f.value(blocker) > 0) {
                ws[j++] = ws[i++];
                continue;
            }
            
            int cr = w.clauseID;
            auto& c = f.clauses[cr];
            
            // Megkeressük, melyik a falseLit a két megfigyelt közül (c[0] vagy c[1])
            int falseIdx = (c[0] == falseLit) ? 0 : 1;
            int otherIdx = 1 - falseIdx;
            
            bool foundNewWatch = false;
            // Próbálunk egy új, nem-hamis literált találni a klózban
            for (size_t k = 2; k < c.size(); ++k) {
                if (f.value(c[k]) >= 0) {
                    std::swap(c[falseIdx], c[k]);
                    //swap  miatt a c[falseIdx] most már egy nem-hamis literál, így új watch-ot adunk hozzá
                    f.addWatch(c[falseIdx], cr, c[otherIdx]);
                    foundNewWatch = true;
                    break;
                }
            }
            
            if (foundNewWatch) {
                i++; // Sikeres csere: a régi watch elveszik (nem másoljuk j-be)
            } else {
                // Ha nem találtunk új figyelőt, marad a régi
                ws[j++] = ws[i++];
                int otherLit = c[otherIdx];
                int val = f.value(otherLit);
                
                // Gyorsítás: frissítjük a blokkoló literált
                ws[j-1].blockingLiteral = otherLit;
                
                if (val < 0) {
                    // Konfliktus történt! A maradék megfigyelőket visszamásoljuk a tömbbe,
                    // majd visszaadjuk a konfliktust okozó klóz ID-ját.
                    while (i < ws.size()) {
                        ws[j++] = ws[i++];
                    }
                    ws.resize(j);
                    return cr; 
                } else if (val == 0) {
                    // Unit propagáció: az utolsó reményünk unassigned, tehát be kell állítanunk igazra.
                    assign(f, otherLit, cr);
                }
            }
        }
        ws.resize(j); // szellem értékek levágása, ami egy hamis elem hanem egy jó elem másolata
    }
    
    return -1; // Nincs konfliktus
}


// --- 2. Konfliktus Analízis (1UIP) ---

void CDCLSolver::analyzeConflict(CDCLFormula& f, int conflictClauseID, std::vector<int>& learnedClause, int& backtrackLevel) {
    //munka terület amiben rezolváliunk, a cél h csak 1 literál maradjon az aktuális szintről
    std::vector<bool> seen(f.numVars + 1, false);
    //számolja hogy hány változó van seenben az aktuális döntési szintről
    int pathC = 0;
    //legfrissebben beállított literál a trail-en, ami a konfliktushoz vezetett
    int p = 0;
    int index = f.trail.size() - 1;
    int clauseID = conflictClauseID;
    
    learnedClause.push_back(0); // Ideiglenes helyfoglaló (-p)-nek
    
    do {
        const auto& clause = f.clauses[clauseID];
        
        for (int q : clause) {
            int var = std::abs(q);
            // Ha ez egy ok-klóz (reason), 'p' volt az a literál, amit propagáltunk, ezt kihagyjuk.
            if (var != std::abs(p) && !seen[var] && f.decisionLevels[var] > 0) {
                seen[var] = true;
                
                // VSIDS aktivitás növelése a konfliktusban részt vevő változókra
                f.activity[var] += 1.0; 
                
                if (f.decisionLevels[var] == f.currentLevel()) {
                    pathC++;
                } else {
                    learnedClause.push_back(q);
                }
            }
        }
        
        // Haladás visszafelé a trail-en a következő propagált (és a konfliktus által "látott") literálig
        while (!seen[std::abs(f.trail[index])]) {
            index--;
        }
        
        p = f.trail[index];
        clauseID = f.reasons[std::abs(p)];//nem lehet -1
        seen[std::abs(p)] = false;
        pathC--;
        index--;
    } while (pathC > 0);
    
    learnedClause[0] = -p; // Az 1UIP literál negáltja lesz az asserting literal
    
    // A visszaugrási szint (backtrack level) meghatározása
    if (learnedClause.size() > 1) {
        int maxIdx = 1;
        int maxLvl = f.decisionLevels[std::abs(learnedClause[1])];
        
        // Megkeressük a második legnagyobb döntési szintet
        for (size_t i = 2; i < learnedClause.size(); i++) {
            int lvl = f.decisionLevels[std::abs(learnedClause[i])];
            if (lvl > maxLvl) {
                maxLvl = lvl;
                maxIdx = i;
            }
        }
        
        // Két-figyelős rendszerhez: biztosítjuk, hogy a 0. és 1. indexen lévők éppen a legkésőbb beállítottak
        std::swap(learnedClause[1], learnedClause[maxIdx]);
        //legnagyobb szint ahol ad kontextust az új tanult klózunk
        backtrackLevel = maxLvl;
    } else {
        // Ha csak 1 hosszú a klóz, akkor abszolút tényt tanultunk (0. szint)
        backtrackLevel = 0;
    }
}


// --- Fő Ciklus ---

SolveResult CDCLSolver::solve(const Formula& form) {
    CDCLFormula f(form.numVars, form.clauses);
    
    // Kezdeti unit propagáció a már eredetileg is egy hosszú klózokra
    for (size_t i = 0; i < f.clauses.size(); i++) {
        if (f.clauses[i].size() == 1) {
            int lit = f.clauses[i][0];
            if (f.value(lit) < 0) return SolveResult::UNSAT; 
            if (f.value(lit) == 0) {
                assign(f, lit, i);
            }
        }
    }

    while (true) {
        // 1. Kényszerítés (Propagate)
        int conflictClause = propagate(f);
        
        if (conflictClause != -1) {
            // Ha a nulladik szinten találunk konfliktust, a formula kielégíthetetlen
            if (f.currentLevel() == 0) {
                return SolveResult::UNSAT; 
            }
            
            std::vector<int> learnedClause;
            int backtrackLevel = 0;
            
            // 2. Konfliktus analízis
            analyzeConflict(f, conflictClause, learnedClause, backtrackLevel);
            
            // 3. Visszaugrás (Backjump) a kiszámított szintre
            backjump(f, backtrackLevel);
            
            // 4. Tanult klóz hozzáadása és az új következmény beállítása
            if (learnedClause.size() == 1) {
                assign(f, learnedClause[0], -1); // 0. szinten tanult unit klóz
            } else {
                f.addLearnedClause(learnedClause);
                assign(f, learnedClause[0], f.clauses.size() - 1); // Indoklás: az imént hozzáadott klóz ID-ja
            }
            
            // 5. VSIDS Decay: periódusonként / konfliktusonként rontjuk az aktivitást
            for (int i = 1; i <= f.numVars; i++) {
                f.activity[i] *= 0.95;
            }
            
        } else {
            // Nincs konfliktus. Készen vagyunk?
            if (f.allAssigned()) {
                //verify_and_print_every_clause(f.clauses, f.assignments);
                return SolveResult::SAT;
            }
            
            // Ha nem, akkor Döntés (Decide)
            int nextVar = pickVariable(f);
            f.newDecisionLevel();
            assign(f, nextVar, -1);
        }
    }
    
}
void CDCLSolver::verify_and_print_every_clause(const std::vector<std::vector<int>>& clauses, const std::vector<int>& assignment) {
    std::cout << "\n--- TELJES FORMULA ELLENORZESE ---" << std::endl;
    int broken_count = 0;

    for (size_t i = 0; i < clauses.size(); ++i) {
        bool clause_true = false;
        std::string debug_line = "";
        
        debug_line += "Kloz #" + std::to_string(i) + ": [ ";
        
        for (int lit : clauses[i]) {
            int var = std::abs(lit);
            
            // Hibakezeles: ha a valtozo indexe nagyobb, mint az assignment tomb
            int val = 0;
            if (var < (int)assignment.size()) {
                val = assignment[var]; 
            }

            // Meghatarozzuk a literal statuszat az ertekadas alapjan
            // val == 1 (True), val == -1 (False), val == 0 (Unassigned)
            std::string status;
            if ((lit > 0 && val == 1) || (lit < 0 && val == -1)) {
                status = "T"; // True
                clause_true = true;
            } else if ((lit > 0 && val == -1) || (lit < 0 && val == 1)) {
                status = "F"; // False
            } else {
                status = "U"; // Unassigned
            }

            debug_line += std::to_string(lit) + "(" + status + ") ";
        }
        
        debug_line += "]";

        if (clause_true) {
            std::cout << debug_line << " -> OK" << std::endl;
        } else {
            std::cout << debug_line << " -> !!! HIBA: EZ A KLOZ HAMIS !!!" << std::endl;
            broken_count++;
        }
    }

    std::cout << "\n--- OSSZESITES ---" << std::endl;
    std::cout << "Osszes kloz: " << clauses.size() << std::endl;
    std::cout << "Hibas klozok szama: " << broken_count << std::endl;
    
    if (broken_count > 0) {
        std::cout << "EREDMENY: A solver hibasan adott SAT-ot!" << std::endl;
    } else {
        std::cout << "EREDMENY: Minden kloz tenyleg IGAZ." << std::endl;
    }
}