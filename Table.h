// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#ifndef TABLE_H
#define TABLE_H

#include "silly.h"
#include "TableEntry.h"
#include <vector>
#include <string>

struct Table {
    uint32_t numCols;
    std::string tableName;
    std::vector<std::vector<TableEntry>> data;
    std::vector<std::string> colNames;
    std::vector<EntryType> dataTypes;

    Table(int numCol, std::string nameTable) : numCols(numCol), tableName(nameTable) {}
};


#endif