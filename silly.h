// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#ifndef SILLY_H
#define SILLY_H

#include <iostream>
#include <vector>
#include <getopt.h>
#include <unordered_map>
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
        if(cmd == "CREATE") processCreate();
        else if(cmd == "REMOVE") processRemove();
        else if(cmd == "INSERT") processInsert();
        else if(cmd == "PRINT") processPrint();
        else if(cmd == "DELETE") processDelete();
        else if(cmd == "JOIN") processJoin();
        else if(cmd == "GENERATE") processGenerate();
        else {
            std::cout << "Error: unrecognized command\n";
            getline(std::cin, cmd);
        }
    }

    void processCreate() {
        std::string cmd; std::string tableName; std::cin >> tableName;
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

    void processRemove() {
        std::string tableName; std::cin >> tableName;
        if(tables.find(tableName) != tables.end()) {
            tables.erase(tableName);
            std::cout << "Table " << tableName << " deleted\n";
        }
        else {
            std::cout << "Error during REMOVE: " << tableName << 
                    " does not name a table in the database\n";
        }
    }

    void processInsert() {

    }

    void processPrint() {

    }

    void processDelete() {

    }

    void processJoin() {

    }

    void processGenerate() {
        
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