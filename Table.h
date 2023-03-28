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

        dataTypes.reserve(numCols);
        // add the correct data type to the dataTypes vector in the table
        for(uint32_t i = 0; i < numCols; i++) {
            std::cin >> cmd;
            if(cmd == "double") {
                dataTypes.push_back(EntryType::Double);
            }
            else if(cmd == "int") {
                dataTypes.push_back(EntryType::Int);
            }
            else if(cmd == "bool") {
                dataTypes.push_back(EntryType::Bool);
            }
            else if(cmd == "string") {
                dataTypes.push_back(EntryType::String);
            }
        }
        //

        // get the column names and add them to the column names map in the table
        colNames.reserve(numCols);
        for(uint32_t i = 0; i < numCols; i++) {
            std::cin >> cmd; std::cout << cmd << " ";
            colNames.insert({cmd, i});
        }
        //

        std::cout << "created\n";
    }
    
    // used by processInsert
    void insert(uint32_t numRowsInsert, std::string& cmd, std::string tableName) {
        std::cin >> cmd; // get rid of ROWS
        uint32_t initialRows = static_cast<uint32_t>(data.size());
        data.reserve(initialRows + numRowsInsert);
        double cmdDouble; int cmdInt; bool cmdBool;
        std::vector<TableEntry> row; row.reserve(colNames.size());
        for(uint32_t i = initialRows; i < numRowsInsert + initialRows; i++) {
            for(uint32_t j = 0; j < colNames.size(); j++) {
                switch(dataTypes[j]) {
                    case EntryType::Double: {
                        std::cin >> cmdDouble;
                        row.emplace_back(TableEntry(cmdDouble));
                        if(indexCol == j && status == tableStatus::Hash) {
                            hash[TableEntry(cmdDouble)].push_back(i);
                        }
                        else if(indexCol == j && status == tableStatus::BST) {
                            bst[TableEntry(cmdDouble)].push_back(i);
                        }
                        break;
                    }
                    case EntryType::Int: {
                        std::cin >> cmdInt;
                        row.emplace_back(TableEntry(cmdInt));
                        if(indexCol == j && status == tableStatus::Hash) {
                            hash[TableEntry(cmdInt)].push_back(i);
                        }
                        else if(indexCol == j && status == tableStatus::BST) {
                            bst[TableEntry(cmdInt)].push_back(i);
                        }
                        break;
                    }
                    case EntryType::Bool: {
                        std::cin >> cmdBool;
                        row.emplace_back(TableEntry(cmdBool));
                        if(indexCol == j && status == tableStatus::Hash) {
                            hash[TableEntry(cmdBool)].push_back(i);
                        }
                        else if(indexCol == j && status == tableStatus::BST) {
                            bst[TableEntry(cmdBool)].push_back(i);
                        }
                        break;
                    }  
                    case EntryType::String: {
                        std::cin >> cmd; //cmd already a string, don't need new variable
                        row.emplace_back(TableEntry(cmd));
                        if(indexCol == j && status == tableStatus::Hash) {
                            hash[TableEntry(cmd)].push_back(i);
                        }
                        else if(indexCol == j && status == tableStatus::BST) {
                            bst[TableEntry(cmd)].push_back(i);
                        }
                        break;
                    }
                }
            }
            data.push_back(row); row.clear();
        }

        std::cout << "Added " << numRowsInsert << " rows to " << tableName << 
        " from position " << initialRows << " to " << initialRows + numRowsInsert - 1 
        << "\n";
    }

    void print(std::string& cmd, std::string& tableName, bool quietMode) {

        // get the inputted column names, find the corresponding indices, and store them in a vector
        uint32_t numCols; std::cin >> numCols; std::string colName;
        std::vector<uint32_t> colIndices; colIndices.reserve(numCols);
        std::vector<std::string> nameCols; nameCols.reserve(numCols);
        std::unordered_map<std::string, uint32_t>::iterator iter;
        for(uint32_t i = 0; i < numCols; i++) {
            std::cin >> colName;
            iter = colNames.find(colName);
            if(iter != colNames.end()) {
                colIndices.push_back(iter->second);
                nameCols.push_back(iter->first);
            }
            else {
                std::cout << "Error during PRINT: " << colName << " does not name a column in " << tableName << '\n';
                std::getline(std::cin, cmd); return;
            }
        }

        std::cin >> cmd;
        if(cmd == "ALL") {
            if(!quietMode) {
                for(size_t i = 0; i < nameCols.size(); i++) {
                    std::cout << nameCols[i] << ' ';
                }
                std::cout << '\n';
            }
            printAll(colIndices, tableName, quietMode);
        }
        else {
            std::cin >> colName; uint32_t colIndex = 0;
            // find index of inputted column name, if it exists
            // ouput error message and return if it does not
            iter = colNames.find(colName);
            if(iter != colNames.end()) {
                colIndex = iter->second;
            }
            else {
                std::cout << "Error during PRINT: " << colName << 
                " does not name a column in " << tableName << '\n';
                getline(std::cin, colName); return;
            }
            //
            if(!quietMode) {
                for(size_t i = 0; i < nameCols.size(); i++) {
                    std::cout << nameCols[i] << ' ';
                }
                std::cout << '\n';
            }
            printWhere(colIndices, tableName, quietMode, colIndex);
        }

    }

    // used by processPrint when user wants to print ALL
    void printAll(std::vector<uint32_t>& colIndices, std::string& tableName, bool quietMode) {

        if(!quietMode) {
            
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
    void printWhere(std::vector<uint32_t>& colIndices, std::string& tableName, bool quietMode, uint32_t colIndex) {

        char op; std::cin >> op;

        bool useHash = ((indexCol == colIndex) && (status == tableStatus::Hash) && (op == '='));
        bool useBST = ((indexCol == colIndex) && (status == tableStatus::BST));
        
        // get data type of inputted column, call corresponding helper function
        uint32_t numPrinted = 0;
        EntryType type = dataTypes[colIndex];
        switch(type) {
            case EntryType::Double: {
                double doubleData; std::cin >> doubleData;
                if(useHash) numPrinted = printHash(TableEntry(doubleData), colIndices, quietMode);
                else if(useBST) numPrinted = printBST(op, TableEntry(doubleData), colIndices, quietMode);
                else numPrinted = colCompare(op, TableEntry(doubleData), colIndices, colIndex, quietMode);
                break;
            }
            case EntryType::Int: {
                int intData; std::cin >> intData;
                if(useHash) numPrinted = printHash(TableEntry(intData), colIndices, quietMode);
                else if(useBST) numPrinted = printBST(op, TableEntry(intData), colIndices, quietMode);
                else numPrinted = colCompare(op, TableEntry(intData), colIndices, colIndex, quietMode);
                break;
            }
            case EntryType::Bool: {
                bool boolData; std::cin >> boolData;
                if(useHash) numPrinted = printHash(TableEntry(boolData), colIndices, quietMode);
                else if(useBST) numPrinted = printBST(op, TableEntry(boolData), colIndices, quietMode);
                else numPrinted = colCompare(op, TableEntry(boolData), colIndices, colIndex, quietMode);
                break;
            }
            case EntryType::String: {
                std::string stringData; std::cin >> stringData;
                if(useHash) numPrinted = printHash(TableEntry(stringData), colIndices, quietMode);
                else if(useBST) numPrinted = printBST(op, TableEntry(stringData), colIndices, quietMode);
                else numPrinted = colCompare(op, TableEntry(stringData), colIndices, colIndex, quietMode);
                break;
            }
        }
        std::cout << "Printed " << numPrinted << " matching rows from " << tableName << '\n';
    }
    //

    uint32_t printHash(const TableEntry& val, std::vector<uint32_t>& colIndices, bool quietMode) {
        uint32_t numPrinted = 0;
        auto iter = hash.equal_range(val);
        while(iter.first != iter.second) {
            numPrinted += static_cast<uint32_t>(iter.first->second.size());
            if(!quietMode) {
                for(size_t i = 0; i < iter.first->second.size(); i++) {
                    for(size_t j = 0; j < colIndices.size(); j++) {
                        std::cout << data[iter.first->second[i]][colIndices[j]] << ' ';
                    }
                    std::cout << '\n';
                }
            }
            iter.first++;
        }
        return numPrinted;
    }

    uint32_t printBST(char oper, const TableEntry& val, std::vector<uint32_t>& colIndices, bool quietMode) {
        uint32_t numPrinted = 0;
        std::map<TableEntry, std::vector<uint32_t>>::iterator left;
        std::map<TableEntry, std::vector<uint32_t>>::iterator right;
        if(oper == '<') {
            left = bst.begin();
            right = bst.lower_bound(val);
        }
        else if(oper == '=') {
            std::tie(left, right) = bst.equal_range(val);
        }
        else {
            left = bst.upper_bound(val);
            right = bst.end();
        }
        while(left != right) {
            numPrinted += static_cast<uint32_t>(left->second.size());
            if(!quietMode) {
                for(size_t i = 0; i < left->second.size(); i++) {
                    for(size_t j = 0; j < colIndices.size(); j++) {
                        std::cout << data[left->second[i]][colIndices[j]] << ' ';
                    }
                    std::cout << '\n';
                }
            }
            left++;
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
        std::cin >> cmd; std::string colName; std::cin >> colName;
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

        if(status != tableStatus::None) {
            generate(indexCol, status);
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
        size_t originalSize = data.size();
        data.erase(std::remove_if(begin(data), end(data), pred), end(data));
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
        std::vector<std::pair<std::string, uint32_t>> colNameIndex; colNameIndex.reserve(numCols);
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

        uint32_t table1Col = colNames[colName1]; uint32_t table2Col = other.colNames[colName2];
        // all data from command collected, now compare the columns of the two tables, print out necesssary information

        uint32_t numPrinted = 0;

        if(other.indexCol != table2Col) {
            std::unordered_map<TableEntry, std::vector<uint32_t>> tempTable;
            for(uint32_t i = 0; i < other.data.size(); i++) {
                tempTable[other.data[i][table2Col]].push_back(i);
            }
            for(size_t i = 0; i < data.size(); i++) {
                auto iter = tempTable.equal_range(data[i][table1Col]);
                while(iter.first != iter.second) {
                    numPrinted += static_cast<uint32_t>(iter.first->second.size());
                    if(!quietMode) {
                        for(size_t j = 0; j < iter.first->second.size(); j++) {
                            for(size_t k = 0; k < colNameIndex.size(); k++) {
                                if(colNameIndex[k].second == 1) {
                                    std::cout << data[i][colNames[colNameIndex[k].first]] << ' ';
                                }
                                else {
                                    std::cout << other.data[iter.first->second[j]][other.colNames[colNameIndex[k].first]] << ' ';
                                }
                            }
                            std::cout << '\n';
                        }
                    }
                    iter.first++;
                }
            }
        }

        else if(other.status == tableStatus::Hash) {
            for(size_t i = 0; i < data.size(); i++) {
                auto iter = other.hash.equal_range(data[i][table1Col]);
                while(iter.first != iter.second) {
                    numPrinted += static_cast<uint32_t>(iter.first->second.size());
                    if(!quietMode) {
                        for(size_t j = 0; j < iter.first->second.size(); j++) {
                            for(size_t k = 0; k < colNameIndex.size(); k++) {
                                if(colNameIndex[k].second == 1) {
                                    std::cout << data[i][colNames[colNameIndex[k].first]] << ' ';
                                }
                                else {
                                    std::cout << other.data[iter.first->second[j]][other.colNames[colNameIndex[k].first]] << ' ';
                                }
                            }
                            std::cout << '\n';
                        }
                    }
                    iter.first++;
                }
            }
        }

        else {
            for(size_t i = 0; i < data.size(); i++) {
                auto iter = other.bst.equal_range(data[i][table1Col]);
                while(iter.first != iter.second) {
                    numPrinted += static_cast<uint32_t>(iter.first->second.size());
                    if(!quietMode) {
                        for(size_t j = 0; j < iter.first->second.size(); j++) {
                            for(size_t k = 0; k < colNameIndex.size(); k++) {
                                if(colNameIndex[k].second == 1) {
                                    std::cout << data[i][colNames[colNameIndex[k].first]] << ' ';
                                }
                                else {
                                    std::cout << other.data[iter.first->second[j]][other.colNames[colNameIndex[k].first]] << ' ';
                                }
                            }
                            std::cout << '\n';
                        }
                    }
                    iter.first++;
                }
            }
        }
        
        std::cout << "Printed " << numPrinted << " rows from joining " << tableName1 << 
        " to " << tableName2 << '\n';
    }

    uint32_t generateIndex(std::string& tableName, std::string& indexType, std::string& colName) {
        auto iter = colNames.find(colName);
        if(iter == colNames.end()) {
            std::cout << "Error during GENERATE: " << colName << 
            " does not name a column in " << tableName << '\n';
            return 1;
        }
        uint32_t colIndex = iter->second;
        tableStatus tableStat;
        if(indexType == "hash") tableStat = tableStatus::Hash;
        else tableStat = tableStatus::BST;
        generate(colIndex, tableStat);

        return 0;
    }

    void generate(uint32_t colIndex, tableStatus indexType) {

        indexCol = colIndex;
        
        if(indexType == tableStatus::Hash) {
            hash.clear();
            status = tableStatus::Hash;
            for(uint32_t i = 0; i < data.size(); i++) {
                hash[data[i][colIndex]].push_back(i);
            }
        }

        else {
            bst.clear();
            status = tableStatus::BST;
            for(uint32_t i = 0; i < data.size(); i++) {
                bst[data[i][colIndex]].push_back(i);
            }
        }
    }

};

#endif