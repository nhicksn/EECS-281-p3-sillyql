// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#ifndef TABLE_H
#define TABLE_H

#include "silly.h"
#include "TableEntry.h"
#include <vector>
#include <string>

struct Table {
    // all data held by Table data structure
    uint32_t numCols;
    std::string tableName;
    std::vector<std::vector<TableEntry>> data;
    std::vector<std::string> colNames;
    std::vector<EntryType> dataTypes;
    //

    // constructor used by create
    Table(int numCol, std::string nameTable) : numCols(numCol), tableName(nameTable) {}
    //
    
    //////////////////////////////////////////////////////////////////////////////////////
    //------------------------------TABLE-MEMBER-FUNCTIONS------------------------------//
    //////////////////////////////////////////////////////////////////////////////////////

    void createTable(std::string cmd) {
        // add the correct data type to the dataTypes vector in the table
        for(uint32_t i = 0; i < numCols; i++) {
            std::cin >> cmd;
            if(cmd == "double") {
                dataTypes.emplace_back(EntryType::Double);
            }
            else if(cmd == "int") {
                dataTypes.emplace_back(EntryType::Int);
            }
            else if(cmd == "bool") {
                dataTypes.emplace_back(EntryType::Bool);
            }
            else if(cmd == "string") {
                dataTypes.emplace_back(EntryType::String);
            }
        }
        //

        // get the column names and add them to the column names vector in the table
        for(uint32_t i = 0; i < numCols; i++) {
            std::cin >> cmd;
            colNames.emplace_back(cmd);
            std::cout << cmd << " ";
        }
        //

        std::cout << "created\n";
    }
    
    // used by processInsert
    void insert(uint32_t numRowsInsert, std::string cmd) {
        size_t initialRows = data.size();
        data.reserve(initialRows + numRowsInsert);
        for(size_t i = 0; i < numRowsInsert; i++) {
            std::vector<TableEntry> row; row.reserve(numCols);
            double cmdDouble; int cmdInt; bool cmdBool;
            for(size_t j = 0; j < numCols; j++) {
                switch(dataTypes[j]) {
                    case EntryType::Double:
                        std::cin >> cmdDouble;
                        row.emplace_back(TableEntry(cmdDouble));
                        break;
                    case EntryType::Int:
                        std::cin >> cmdInt;
                        row.emplace_back(TableEntry(cmdInt));
                        break;
                    case EntryType::Bool:
                        std::cin >> cmdBool;
                        row.emplace_back(TableEntry(cmdBool));
                        break;
                    case EntryType::String:
                            std::cin >> cmd; //cmd already a string, don't need new variable
                        row.emplace_back(TableEntry(cmd));
                        break;
                }
            }
            data.push_back(row);
        }

        std::cout << "Added " << numRowsInsert << " rows to " << tableName << 
        " from position " << initialRows << " to " << initialRows + numRowsInsert - 1 
        << "\n";
    }

    // used by processPrint when user wants to print ALL
    void printAll(std::unordered_set<std::string> &colInputs) {
        for(size_t i = 0; i < data.size(); i++) {
            for(uint32_t j = 0; j < numCols; j++) {
                if(colInputs.find(colNames[j]) != colInputs.end()) {
                    std::cout << data[i][j] << ' ';
                }
            }
            std::cout << '\n';
        }
        std::cout << "Printed " << data.size() << 
        " matching rows from " << tableName << '\n';
    }
    //

    // used by processPrint when user wants to print WHERE
    void printWhere() {
        return;
    }
    //
};


#endif