// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#ifndef SILLY_H
#define SILLY_H

#include <iostream>
#include <vector>
#include <getopt.h>
#include <unordered_map>
#include <unordered_set>
#include "Table.h"

using TableName = std::string;

class Silly {
private:

    bool quietMode = false;

    // hash table of all the tables
    std::unordered_map<TableName, Table> tables;

    void printHelp() {
        std::cout << "Usage: ./silly [-q] [-h]\n";
        std::cout << "You do not need to input any command line arguments.\n";
        std::cout << "-q will enable quiet mode\n";
        std::cout << "-h will print this message again\n";
    } // printHelp

    // gets command line args, and settings of input file
    void getMode(const int &argc, char *argv[]) {
        opterr = false;
        int choice;
        int index = 0;
        option long_options[] = {
            { "help",         no_argument, nullptr, 'h'  },
            { "quiet",        no_argument, nullptr, 'q'  },
            { nullptr,        0,           nullptr, '\0' },
        }; // long_options

        while((choice = getopt_long(argc, argv, "hq", long_options, &index)) != -1) {
            switch(choice) {
                case 'h': {
                    printHelp();
                    exit(0);
                } // case 'v'
                
                case 'q' : {
                    quietMode = true;
                    break;
                } // case 'm'

                default: {
                    std::cerr << "Error: Unknown command line option\n";
                    exit(1);
                } // default case
            } // switch choice
        } // while choice
    } //getMode

    // called by readInput if command is not quit or a comment
    // needs to handle:
    // CREATE, REMOVE, INSERT, PRINT, DELETE, JOIN, GENERATE

    void processCommand(std::string cmd) {

        if(cmd == "CREATE") processCreate(cmd);
        else if(cmd == "REMOVE") processRemove(cmd);
        else if(cmd == "INSERT") processInsert(cmd);
        else if(cmd == "PRINT") processPrint(cmd);
        else if(cmd == "DELETE") processDelete(cmd);
        else if(cmd == "JOIN") processJoin(cmd);
        else if(cmd == "GENERATE") processGenerate(cmd);
        else {
            std::cout << "Error: unrecognized command\n";
            getline(std::cin, cmd);
        }

    }

    void processCreate(std::string cmd) {
        std::string tableName; std::cin >> tableName;
        std::cout << "New table " << tableName << " with column(s) ";
        int numCols; std::cin >> numCols;

        // create a new table which has the right number of columns
        Table newTable(numCols, tableName);
        // add the new table to the unordered map
        auto iter = tables.find(tableName);
        if(iter == tables.end()) {
            tables.insert({tableName, newTable});
            iter = tables.find(tableName);
        }
        else {
            std::cout << "Error during CREATE: Cannot create already existing table " << 
            tableName << '\n';
            getline(std::cin, cmd);
            return;
        }

        // add the correct data type to the dataTypes vector in the table
        for(int i = 0; i < numCols; i++) {
            std::cin >> cmd;
            if(cmd == "double") {
                iter->second.dataTypes.emplace_back(EntryType::Double);
            }
            else if(cmd == "int") {
                iter->second.dataTypes.emplace_back(EntryType::Int);
            }
            else if(cmd == "bool") {
                iter->second.dataTypes.emplace_back(EntryType::Bool);
            }
            else if(cmd == "string") {
                iter->second.dataTypes.emplace_back(EntryType::String);
            }
        }
        //

        // get the column names and add them to the column names vector in the table
        for(int i = 0; i < numCols; i++) {
            std::cin >> cmd;
            iter->second.colNames.emplace_back(cmd);
            std::cout << cmd << " ";
        }
        //

        std::cout << "created\n";
        
    }

    void processRemove(std::string cmd) {
        std::cin >> cmd; // cmd is the tableName
        if(tables.find(cmd) != tables.end()) {
            tables.erase(cmd);
            std::cout << "Table " << cmd << " deleted\n";
        }
        else {
            std::cout << "Error during REMOVE: " << cmd << 
                    " does not name a table in the database\n";
        }
    }

    void processInsert(std::string cmd) {
        std::cin >> cmd; // get rid of INTO
        std::string tableName; std::cin >> tableName;
        uint32_t numRowsInsert; std::cin >> numRowsInsert;

        auto iter = tables.find(tableName);
        if(iter != tables.end()) {
            std::cin >> cmd; // get rid of ROWS

            size_t initialRows = iter->second.data.size();
            iter->second.data.reserve(initialRows + numRowsInsert);
            for(size_t i = 0; i < numRowsInsert; i++) {
                std::vector<TableEntry> row; row.reserve(iter->second.numCols);
                double cmdDouble; int cmdInt; bool cmdBool;
                for(size_t j = 0; j < iter->second.numCols; j++) {
                    switch(iter->second.dataTypes[j]) {
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
                iter->second.data.push_back(row);
            }

            std::cout << "Added " << numRowsInsert << " rows to " << tableName << 
            " from position " << initialRows << " to " << initialRows + numRowsInsert - 1 
            << "\n";
        }

        else {
            std::cout << "Error during INSERT: " << tableName 
                << " does not name a table in the database\n";

            // get rid of the rest of the command
            for(size_t i = 0; i < numRowsInsert; i++) {
                std::getline(std::cin, cmd);
            }

            return;
        }
    }

    void processPrint(std::string cmd) {
        std::cin >> cmd; // get from of FROM
        std::string tableName; std::cin >> tableName;

        auto iter = tables.find(tableName);
        if(iter != tables.end()) {
            std::unordered_set<std::string> colNames;
            uint32_t numCols; std::cin >> numCols; std::string colName;
            for(uint32_t i = 0; i < numCols; i++) {
                std::cin >> colName; colNames.insert(colName);
                std::cout << colName << " ";
            }
            std::cin >> cmd; std::cout << '\n';

            // print all
            if(cmd == "ALL") {
                for(size_t i = 0; i < iter->second.data.size(); i++) {
                    for(uint32_t j = 0; j < iter->second.numCols; j++) {
                        if(colNames.find(iter->second.colNames[j]) != colNames.end()) {
                            std::cout << iter->second.data[i][j] << ' ';
                        }
                    }
                    std::cout << '\n';
                }
                std::cout << "Printed " << iter->second.data.size() << 
                " matching rows from " << tableName << '\n';
            }
            //

            // print where
            else {
                return;
            }
            //
        }

        else {
            std::cout << "Error during PRINT: " << tableName 
                << " does not name a table in the database\n";
                std::getline(std::cin, cmd); return;
        }
    }

    void processDelete(std::string cmd) {
        cmd[0]; //dummy
    }

    void processJoin(std::string cmd) {
        cmd[0]; //dummy
    }

    void processGenerate(std::string cmd) {
        cmd[0]; //dummy
    }

public:

    // constructor which handles command line arguments
    Silly(const int &argc, char *argv[]) {
        getMode(argc, argv);
    }

    void readInput() {
        
        // command string
        std::string cmd;

        do {

            // check for errors from cin
            if(std::cin.fail()) {
                std::cerr << "Error: Reading from cin has failed\n";
                exit(1);
            }
            //

            // get user input
            std::cout << "% ";
            std::cin >> cmd;
            //
            
            // handle base cases (QUIT and comment '#')
            if(cmd == "QUIT") continue;
            else if(cmd[0] == '#') { getline(std::cin, cmd); continue; }
            //

            // process command if neither base case
            processCommand(cmd);

        } while(cmd != "QUIT");

        std::cout << "Thanks for being silly!\n";
    }
};

#endif