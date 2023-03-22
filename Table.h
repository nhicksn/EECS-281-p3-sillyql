// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#ifndef TABLE_H
#define TABLE_H

#include "silly.h"
#include "TableEntry.h"
#include <vector>

class Table {
friend class Silly;
private:
    std::vector<std::vector<TableEntry>> data;
    std::vector<std::string> colNames;

public:
    Table(int numCols) {
        std::vector<TableEntry> initial; initial.reserve(numCols);
        data.emplace_back(initial);
    }

};



#endif