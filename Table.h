// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#ifndef TABLE_H
#define TABLE_H

#include "silly.h"
#include "TableEntry.h"
#include <vector>
#include <string>

struct greaterThan {
    size_t col;
    TableEntry compareTo;

    bool operator()(const std::vector<TableEntry> &row) {
        if(row[col] > compareTo) return true;
        return false;
    }

    greaterThan(size_t colIn, TableEntry compIn) : col(colIn), compareTo(compIn) { }
};

struct equalTo {
    size_t col;
    TableEntry compareTo;

    bool operator()(const std::vector<TableEntry> &row) {
        if(row[col] == compareTo) return true;
        return false;
    }
    
    equalTo(size_t colIn, TableEntry compIn) : col(colIn), compareTo(compIn) { }
};

struct lessThan {
    size_t col;
    TableEntry compareTo;

    bool operator()(const std::vector<TableEntry> &row) {
        if(row[col] < compareTo) return true;
        return false;
    }

    lessThan(size_t colIn, TableEntry compIn) : col(colIn), compareTo(compIn) { }
};

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
        double cmdDouble; int cmdInt; bool cmdBool;
        for(size_t i = 0; i < numRowsInsert; i++) {
            std::vector<TableEntry> row; row.reserve(numCols);
            for(size_t j = 0; j < numCols; j++) {
                switch(dataTypes[j]) {
                    case EntryType::Double: {
                        std::cin >> cmdDouble;
                        row.emplace_back(TableEntry(cmdDouble));
                        break;
                    }
                    case EntryType::Int: {
                        std::cin >> cmdInt;
                        row.emplace_back(TableEntry(cmdInt));
                        break;
                    }
                    case EntryType::Bool: {
                        std::cin >> cmdBool;
                        row.emplace_back(TableEntry(cmdBool));
                        break;
                    }  
                    case EntryType::String: {
                        std::cin >> cmd; //cmd already a string, don't need new variable
                        row.emplace_back(TableEntry(cmd));
                        break;
                    }
                }
            }
            data.push_back(row);
        }

        std::cout << "Added " << numRowsInsert << " rows to " << tableName << 
        " from position " << initialRows << " to " << initialRows + numRowsInsert - 1 
        << "\n";
    }

    // used by processPrint when user wants to print ALL
    void printAll(std::vector<size_t> &colIndices, bool quietMode) {
        if(!quietMode) {

            // iterate through data and corresponding indices and print them
            for(size_t i = 0; i < data.size(); i++) {
                for(uint32_t j = 0; j < colIndices.size(); j++) {
                    std::cout << data[i][colIndices[j]] << ' ';
                }
                std::cout << '\n';
            }
            //
        }
    
        std::cout << "Printed " << data.size() << 
        " matching rows from " << tableName << '\n';
    }
    //

    // used by processPrint when user wants to print WHERE // TODO
    void printWhere(std::vector<size_t> &colInputs, bool quietMode) {
        std::string colName; std::cin >> colName; size_t colIndex = 0;

        // find index of inputted column name, if it exists
        // ouput error message and return if it does not
        for(size_t i = 0; i < colNames.size(); i++) {
            if(colName == colNames[i]) { colIndex = i; break; }
            else if(i == colNames.size() - 1) {
                std::cout << "Error during PRINT: " << colName << 
                " does not name a column in " << tableName << '\n';
                getline(std::cin, colName);
                return;
            }
        }
        //

        char op; std::cin >> op;

        // get data type of inputted column, call corresponding helper function
        int numPrinted = 0;
        EntryType type = dataTypes[colIndex];
        switch(type) {
            case EntryType::Double: {
                double doubleData; std::cin >> doubleData;
                numPrinted = colCompare(op, TableEntry(doubleData), colInputs, colIndex, quietMode);
                break;
            }
            case EntryType::Int: {
                int intData; std::cin >> intData;
                numPrinted = colCompare(op, TableEntry(intData), colInputs, colIndex, quietMode);
                break;
            }
            case EntryType::Bool: {
                bool boolData; std::cin >> boolData;
                numPrinted = colCompare(op, TableEntry(boolData), colInputs, colIndex, quietMode);
                break;
            }
            case EntryType::String: {
                std::string stringData; std::cin >> stringData;
                TableEntry tableString(stringData);
                numPrinted = colCompare(op, TableEntry(stringData), colInputs, colIndex, quietMode);
                break;
            }
        }

        std::cout << "Printed " << numPrinted << " matching rows from " << tableName << '\n';
    }
    //

    int colCompare(char oper, const TableEntry& val, std::vector<size_t>& colIndices, size_t colIndex, bool quietMode) {
        switch(oper) {
            case '>':
                return printCols(colIndices, greaterThan(colIndex, val), quietMode);
                break;
            case '=':
                return printCols(colIndices, equalTo(colIndex, val), quietMode);
                break;
            case '<':
                return printCols(colIndices, lessThan(colIndex, val), quietMode);
                break;
        }
        return 0;
    }

    template <typename FuncType>
    int printCols(std::vector<size_t>& colIndices, FuncType pred, bool quietMode) {
        int numPrinted = 0;
        for(size_t i = 0; i < data.size(); i++) {
            // if data passes the functor, print all necessary columns
            if(pred(data[i])) {
                numPrinted++;
                if(!quietMode) {
                    for(size_t j = 0; j < colIndices.size(); j++) {
                        std::cout << data[i][j] << ' ';
                    }
                    std::cout << '\n';
                }
            }
        }
        return numPrinted;
    }

    int deleteRows(std::string cmd) {
        std::string colName; std::cin >> colName;
        int colIndex;
        for(size_t i = 0; i < colNames.size(); i++) {
            if(colName == colNames[i]) { colIndex = i; break; }
            else if(i == colNames.size() - 1) {
                std::cout << "Error during DELETE: " << colName << 
                " does not name a column in " << tableName << '\n';
                std::getline(std::cin, cmd);
                return -1;
            }
        }
        char oper; std::cin >> oper;
        
    }
};



#endif