// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#ifndef SILLY_H
#define SILLY_H

#include <iostream>
#include <vector>
#include <getopt.h>
#include <unordered_map>

using TableName = std::string;

class Silly {
private:

    bool quietMode = false;

    // hash table of all the tables
    std::unordered_map<TableName, std::vector<std::vector<TableEntry>>> tables;
    
    // hash table containing the column names of each table
    std::unordered_map<TableName, std::vector<std::string>> colNames;

    void printHelp() {
        std::cout << "Usage: ./silly [-q] [-h]\nYou do not need to input any command ";
        std::cout << "line arguments.\n-q will enable quiet mode\n";
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
        
    }

    void processCreate(std::string cmd) {
        std::string tableName; std::cin >> tableName;
        int numCols; std::cin >> numCols;

        // TODO: not sure how or if you want to store each columns data type??

        // create a 'base' row which has the correct number of columns
        std::vector<TableEntry> newRow; newRow.reserve(numCols);
        // make a new table whose first row is the 'base' row
        std::vector<std::vector<TableEntry>> newTable{newRow};
        // add the new table to the unordered map
        tables[tableName] = newTable;

        // get rid of the data types given (assuming you don't need them)
        for(int i = 0; i < numCols; i++) {
            std::cin >> cmd;
        }
        //

        // get the column names and add them to the column names unordered map
        for(int i = 0; i < numCols; i++) {
            std::cin >> cmd;
            colNames[tableName].push_back(cmd);
        }
        
    }

    void processRemove(std::string cmd) {

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
            if(cin.fail()) {
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

    }
};

#endif