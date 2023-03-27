// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#ifndef TABLE_H
#define TABLE_H

#include "silly.h"
#include "TableEntry.h"
#include <vector>
#include <map>
#include <iterator>
#include <string>

enum class tableStatus {
    None,
    BST,
    Hash
};

struct greaterThan {
    uint32_t col;
    TableEntry compareTo;

    bool operator()(const std::vector<TableEntry> &row) {
        return row[col] > compareTo;
    }

    greaterThan(uint32_t colIn, const TableEntry& compIn) : col(colIn), compareTo(compIn) { }
};

struct equalTo {
    uint32_t col;
    TableEntry compareTo;

    bool operator()(const std::vector<TableEntry> &row) {
        return row[col] == compareTo;
    }
    
    equalTo(uint32_t colIn, const TableEntry& compIn) : col(colIn), compareTo(compIn) { }
};

struct lessThan {
    uint32_t col;
    TableEntry compareTo;

    bool operator()(const std::vector<TableEntry> &row) {
        return row[col] < compareTo;
    }

    lessThan(uint32_t colIn, const TableEntry& compIn) : col(colIn), compareTo(compIn) { }
};

struct joinComp {
    uint32_t col1;
    uint32_t col2;

    bool operator()(const std::vector<TableEntry> &row1, const std::vector<TableEntry>& row2) {
        return row1[col1] == row2[col2];
    }

    joinComp(uint32_t col1in, uint32_t col2in) : col1(col1in), col2(col2in) { }
};

struct Table {
    // all data held by Table data structure
    std::vector<std::vector<TableEntry>> data;
    // maps colNames to their index in the data
    std::unordered_map<std::string, uint32_t> colNames;
    std::vector<EntryType> dataTypes;

    // info for GENERATE
    tableStatus status = tableStatus::None;
    uint32_t indexCol = UINT32_MAX;
    // maps an entry in the given column to the rows index 
    std::unordered_map<TableEntry, std::vector<uint32_t>> hash; // hash index for GENERATE
    std::map<TableEntry, std::vector<uint32_t>> bst; // bst index for GENERATE
    //

    // constructor used by create
    Table() {}
    //
    
    ////////////////////////////////////////////////////////////////////////////////
    //---------------------------TABLE-MEMBER-FUNCTIONS---------------------------//
    ////////////////////////////////////////////////////////////////////////////////

    void createTable(std::string& cmd, uint32_t numCols) {

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

        // get the column names and add them to the column names map in the table
        for(uint32_t i = 0; i < numCols; i++) {
            std::cin >> cmd;
            colNames.insert({cmd, i});
            std::cout << cmd << " ";
        }
        //

        std::cout << "created\n";
    }
    
    // used by processInsert
    void insert(uint32_t numRowsInsert, std::string& cmd, std::string tableName) {
        std::cin >> cmd; // get rid of ROWS
        size_t initialRows = data.size();
        data.reserve(initialRows + numRowsInsert);
        double cmdDouble; int cmdInt; bool cmdBool;
        std::vector<TableEntry> row; row.reserve(colNames.size());
        for(uint32_t i = 0; i < numRowsInsert; i++) {
            for(uint32_t j = 0; j < colNames.size(); j++) {
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
            data.push_back(row); row.clear();
        }

        if(status != tableStatus::None) {
            regenerate(indexCol, status);
        }

        std::cout << "Added " << numRowsInsert << " rows to " << tableName << 
        " from position " << initialRows << " to " << initialRows + numRowsInsert - 1 
        << "\n";
    }

    void print(std::string& cmd, std::string& tableName, bool quietMode) {

        // get the inputted column names, find the corresponding indices, and store them in a vector
        std::vector<uint32_t> colIndices;
        uint32_t numCols; std::cin >> numCols; std::string colName;
        for(uint32_t i = 0; i < numCols; i++) {
            std::cin >> colName;
            auto iter = colNames.find(colName);
            if(iter != colNames.end()) {
                colIndices.push_back(iter->second);
                if(!quietMode) {
                    std::cout << colName << ' ';
                }
            }
            else {
                std::cout << "Error during PRINT: " << colName << " does not name a column in " << tableName << '\n';
                std::getline(std::cin, cmd); return;
            }
        }
        //

        std::cin >> cmd;
        if(cmd == "ALL") printAll(colIndices, tableName, quietMode);
        else printWhere(colIndices, tableName, quietMode);

    }

    // used by processPrint when user wants to print ALL
    void printAll(std::vector<uint32_t>& colIndices, std::string& tableName, bool quietMode) {

        if(!quietMode) {

            std::cout << '\n';
            
            // iterate through data and corresponding indices and print them
            for(uint32_t i = 0; i < data.size(); i++) {
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

    // used by processPrint when user wants to print WHERE
    void printWhere(std::vector<uint32_t>& colIndices, std::string& tableName, bool quietMode) {

        std::string colName; std::cin >> colName; uint32_t colIndex = 0;
        // find index of inputted column name, if it exists
        // ouput error message and return if it does not
        auto iter = colNames.find(colName);
        if(iter != colNames.end()) {
            colIndex = iter->second;
        }
        else {
            std::cout << "Error during PRINT: " << colName << 
            " does not name a column in " << tableName << '\n';
            getline(std::cin, colName); return;
        }
        //

        bool useBST = ((indexCol == colIndex) && (status == tableStatus::BST));
        
        // get data type of inputted column, call corresponding helper function
        char op; std::cin >> op;
        uint32_t numPrinted = 0;
        EntryType type = dataTypes[colIndex];
        switch(type) {
            case EntryType::Double: {
                double doubleData; std::cin >> doubleData;
                if(useBST) numPrinted = printBST(op, TableEntry(doubleData), colIndices, quietMode);
                else numPrinted = colCompare(op, TableEntry(doubleData), colIndices, colIndex, quietMode);
                break;
            }
            case EntryType::Int: {
                int intData; std::cin >> intData;
                if(useBST) numPrinted = printBST(op, TableEntry(intData), colIndices, quietMode);
                else numPrinted = colCompare(op, TableEntry(intData), colIndices, colIndex, quietMode);
                break;
            }
            case EntryType::Bool: {
                bool boolData; std::cin >> boolData;
                if(useBST) numPrinted = printBST(op, TableEntry(boolData), colIndices, quietMode);
                else numPrinted = colCompare(op, TableEntry(boolData), colIndices, colIndex, quietMode);
                break;
            }
            case EntryType::String: {
                std::string stringData; std::cin >> stringData;
                if(useBST) numPrinted = printBST(op, TableEntry(stringData), colIndices, quietMode);
                else numPrinted = colCompare(op, TableEntry(stringData), colIndices, colIndex, quietMode);
                break;
            }
        }
        std::cout << "Printed " << numPrinted << " matching rows from " << tableName << '\n';
    }
    //

    uint32_t printBST(char oper, const TableEntry& val, std::vector<uint32_t>& colIndices, bool quietMode) {\
        if(!quietMode) std::cout << '\n';
        uint32_t numPrinted = 0;
        if(oper == '<') {
            auto iter = bst.lower_bound(val);
            if(iter == bst.end()) return 0;
            auto left = bst.begin();
            while(left != iter) {
                numPrinted++;
                if(!quietMode) {
                    for(size_t i = 0; i < colIndices.size(); i++) {
                        for(size_t j = 0; j < left->second.size(); j++) {
                            std::cout << data[left->second[j]][colIndices[i]] << ' ';
                        }
                    }
                    std::cout << '\n';
                }
                left++;
            }
        }
        else if(oper == '=') {
            auto iter = bst.equal_range(val);
            if(iter.first == bst.begin()) return 0;
            while(iter.first != iter.second) {
                numPrinted++;
                if(!quietMode) {
                    for(size_t i = 0; i < colIndices.size(); i++) {
                        for(size_t j = 0; j < iter.first->second.size(); j++) {
                            std::cout << data[iter.first->second[j]][colIndices[i]] << ' ';
                        }
                    }
                    std::cout << '\n';
                }
                iter.first++;
            }
        }
        else {
            auto iter = bst.upper_bound(val);
            auto right = bst.end();
            while(iter != right) {
                numPrinted++;
                if(!quietMode) {
                    for(size_t i = 0; i < colIndices.size(); i++) {
                        for(size_t j = 0; j < iter->second.size(); j++) {
                            std::cout << data[iter->second[j]][colIndices[i]] << ' ';
                        }
                    }
                    std::cout << '\n';
                }
                iter++;
            }
        }
        return numPrinted;
    }

    uint32_t colCompare(char oper, const TableEntry& val, std::vector<uint32_t>& colIndices, uint32_t colIndex, bool quietMode) {
        switch(oper) {
            case '>':
                return printRows(colIndices, greaterThan(colIndex, val), quietMode);
                break;
            case '=':
                return printRows(colIndices, equalTo(colIndex, val), quietMode);
                break;
            case '<':
                return printRows(colIndices, lessThan(colIndex, val), quietMode);
                break;
        }
        return 0;
    }

    template <typename FuncType>
    uint32_t printRows(std::vector<uint32_t>& colIndices, FuncType pred, bool quietMode) {
        uint32_t numPrinted = 0;
        if(!quietMode) { std::cout << '\n'; }
        for(uint32_t i = 0; i < data.size(); i++) {
            // if data passes the functor, print all necessary columns
            if(pred(data[i])) {
                numPrinted++;
                if(!quietMode) {
                    for(uint32_t j = 0; j < colIndices.size(); j++) {
                        std::cout << data[i][colIndices[j]] << ' ';
                    }
                    std::cout << '\n';
                }
            }
        }
        return numPrinted;
    }

    void deleteRows(std::string& cmd, std::string tableName) {
        std::cin >> cmd;
        std::string colName; std::cin >> colName;
        uint32_t colIndex = 0;

        auto iter = colNames.find(colName);
        if(iter != colNames.end()) {
            colIndex = iter->second;
        }
        else {
            std::cout << "Error during DELETE: " << colName << 
                " does not name a column in " << tableName << '\n';
            std::getline(std::cin, cmd);
            return;
        }

        char oper; std::cin >> oper;
        
        // get data type of inputted column, call corresponding helper function
        uint32_t numDeleted = 0;
        EntryType type = dataTypes[colIndex];
        switch(type) {
            case EntryType::Double: {
                double doubleData; std::cin >> doubleData;
                numDeleted = getOper(oper, TableEntry(doubleData), colIndex);
                break;
            }
            case EntryType::Int: {
                int intData; std::cin >> intData;
                numDeleted = getOper(oper, TableEntry(intData), colIndex);
                break;
            }
            case EntryType::Bool: {
                bool boolData; std::cin >> boolData;
                numDeleted = getOper(oper, TableEntry(boolData), colIndex);
                break;
            }
            case EntryType::String: {
                std::string stringData; std::cin >> stringData;
                numDeleted = getOper(oper, TableEntry(stringData), colIndex);
                break;
            }
        }

        std::cout << "Deleted " << numDeleted << " rows from " << tableName << '\n';
    }

    uint32_t getOper(char oper, const TableEntry& val, uint32_t colIndex) {
        switch(oper) {
            case '>':
                return deleteWhere(greaterThan(colIndex, val));
                break;
            case '=':
                return deleteWhere(equalTo(colIndex, val));
                break;
            case '<':
                return deleteWhere(lessThan(colIndex, val));
                break;
        }
        return 0;
    }

    template <typename FuncType>
    uint32_t deleteWhere(FuncType pred) {
        auto iter = std::remove_if(begin(data), end(data), pred);
        size_t originalSize = data.size();
        data.erase(iter, end(data));
        uint32_t numDeleted = static_cast<uint32_t>(originalSize - data.size());
        return numDeleted;
    }

    void joinTables(Table& other, std::string& cmd, std::string& tableName1, std::string& tableName2, bool quietMode) {
        std::cin >> cmd; // get rid of WHERE
        std::string colName1; std::cin >> colName1; std::cin >> cmd;
        std::string colName2; std::cin >> colName2; std::cin >> cmd; std::cin >> cmd;
        
        // get rid of junk, keep colNames (to compare), next validate all colNames

        auto iter = colNames.find(colName1);
        if(iter == colNames.end()) {
            std::cout << "Error during JOIN: " << colName1 << 
            " does not name a column in " << tableName1 << '\n';
            std::getline(std::cin, cmd); return;
        }
        iter = other.colNames.find(colName2);
        if(iter == other.colNames.end()) {
            std::cout << "Error during JOIN: " << colName2 << 
            " does not name a column in " << tableName2 << '\n';
            std::getline(std::cin, cmd); return;
        }

        uint32_t numCols; std::cin >> numCols; std::string colName; uint32_t colIndex;
        std::vector<std::pair<std::string, uint32_t>> colNameIndex;
        for(uint32_t i = 0; i < numCols; i++) {
            std::cin >> colName; std::cin >> colIndex;
            if(colIndex == 1) {
                iter = colNames.find(colName);
                if(iter == colNames.end()) {
                    std::cout << "Error during JOIN: " << colName << 
                    " does not name a column in " << tableName1 << '\n';
                    std::getline(std::cin, cmd); return;
                }
            }
            else {
                iter = other.colNames.find(colName);
                if(iter == other.colNames.end()) {
                    std::cout << "Error during JOIN: " << colName << 
                    " does not name a column in " << tableName2 << '\n';
                    std::getline(std::cin, cmd); return;
                }
            }
            colNameIndex.push_back({colName, colIndex});
        }
        if(!quietMode) {
            for(size_t i = 0; i < colNameIndex.size(); i++) {
                std::cout << colNameIndex[i].first << ' ';
            }
            std::cout << '\n';
        }
        // all data from command collected, now compare the columns of the two tables, print out necesssary information

        // comparator, will return true if data needs to be printed
        joinComp func(colNames[colName1], other.colNames[colName2]);
        uint32_t numPrinted = 0;
        for(size_t i = 0; i < data.size(); i++) {
            for(size_t j = 0; j < other.data.size(); j++) {
                if(func(data[i], other.data[j])) {
                    numPrinted++;
                    if(!quietMode) {
                        for(size_t k = 0; k < colNameIndex.size(); k++) {
                            if(colNameIndex[k].second == 1) {
                                std::cout << data[i][colNames[colNameIndex[k].first]] << ' ';
                            }
                            else {
                                std::cout << other.data[j][other.colNames[colNameIndex[k].first]] << ' ';
                            }
                        }
                        std::cout << '\n';
                    }
                }
            }
        }
        std::cout << "Printed " << numPrinted << " rows from joining " << tableName1 << 
        " to " << tableName2 << '\n';
    }

    int generateIndex(std::string& tableName, std::string& indexType, std::string& colName) {
        auto iter = colNames.find(colName);
        if(iter == colNames.end()) {
            std::cout << "Error during GENERATE: " << colName << 
            " does not name a column in " << tableName << '\n';
            return -1;
        }
        uint32_t colIndex = iter->second;
        tableStatus tableStat;
        if(indexType == "hash") tableStat = tableStatus::Hash;
        else tableStat = tableStatus::BST;
        regenerate(colIndex, tableStat);

        return 0;
    }

    void regenerate(uint32_t colIndex, tableStatus indexType) {
        if(status == tableStatus::Hash) hash.clear();
        else if(status == tableStatus::BST) bst.clear();

        if(indexType == tableStatus::Hash) {
            indexCol = colIndex;
            status = tableStatus::Hash;
            std::vector<uint32_t> hashVec;
            for(uint32_t i = 0; i < data.size(); i++) {
                hash[data[i][colIndex]].push_back(i);
            }
        }

        else {
            indexCol = colIndex;
            status = tableStatus::BST;
            for(uint32_t i = 0; i < data.size(); i++) {
                bst[data[i][colIndex]].push_back(i);
            }
        }
    }

};

#endif