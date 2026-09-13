#pragma once
#include <vector>

struct Watcher {
    int clauseID;
    int blockingLiteral;
};

class WatchedFormula {
public:
    std::vector<std::vector<Watcher>> watches;

    WatchedFormula(int numVars) {
        watches.resize(2 * numVars + 2);
    }

    inline int litToIdx(int lit) const {
        return (lit > 0) ? (lit << 1) : ((-lit) << 1 | 1);
    }

    void addWatch(int lit, int clauseID, int blockLit) {
        watches[litToIdx(lit)].push_back({clauseID, blockLit});
    }

    void initTwoWatchedLiterals(const std::vector<std::vector<int>>& clauses) {
        for (int i = 0; i < (int)clauses.size(); ++i) {
            if (clauses[i].size() >= 2) {
                addWatch(clauses[i][0], i, clauses[i][1]);
                addWatch(clauses[i][1], i, clauses[i][0]);
            }
        }
    }
};
