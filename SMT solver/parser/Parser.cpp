#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "../formula/Formula.h"

Formula parseCNF(const std::string& filename) {
    Formula formula;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return formula;
    }

    int maxVar = 0;
    std::string line;
    while (std::getline(file, line)) {
        // Trim szóközök
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty() || line[0] == 'c') continue; // komment

        if (line[0] == 'p') {
            // header sor, nem számít a numVars, majd a legnagyobb literál alapján állítjuk
            continue;
        }

        // Split szóközök mentén
        Clause clause;
        std::istringstream iss(line);
        std::string token;
        while (iss >> token) {
            int lit = std::stoi(token);
            if (lit == 0) break; // klauzula vége
            clause.push_back(lit);
            maxVar = std::max(maxVar, std::abs(lit));
        }

        if (!clause.empty())
            formula.clauses.push_back(clause);
    }

    formula.numVars = maxVar;

    // Debug print
   /* std::cout << "Parsed " << formula.numVars << " vars, "
              << formula.clauses.size() << " clauses:\n";
    for (size_t i = 0; i < formula.clauses.size(); ++i) {
        std::cout << "Clause " << i+1 << ": ";
        for (Literal lit : formula.clauses[i]) std::cout << lit << " ";
        std::cout << "\n";
    }*/

    return formula;
}