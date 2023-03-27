// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#ifndef SILLY_H
#define SILLY_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <getopt.h>
#include <unordered_map>
#include <unordered_set>
#include "Table.h"
#include "TableEntry.h"

using TableName = std::string;

class Silly {
private:
    ////////////////////////////////
    //---SILLY-MEMBER-VARIABLES---//
    ////////////////////////////////

    bool quietMode = false;

    // hash table of all the tables
    std::unordered_map<TableName, Table> tables;


    /////////////////////////////////
    //---COMMAND-LINE-PROCESSORS---//
    /////////////////////////////////

    // PRINTHELP
    // used by getMode if -h is specified on command line
    void printHelp() {
        std::cout << "Usage: ./silly [-q] [-h]\n";
        std::cout << "You do not need to input any command line arguments.\n";
        std::cout << "-q will enable quiet mode\n";
        std::cout << "-h will print this message again\n";
    } // printHelp

    // GETMODE
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
    

    ///////////////////////////////////////////////////////////////////////////
    //-----------------------=PROCESS-MEMBER-FUNCTIONS-----------------------//
    ///////////////////////////////////////////////////////////////////////////

    // PROCESSCOMMAND
    // called by runShell if command is not quit or a comment
    void processCommand(std::string& cmd) {
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

    // PROCESSCREATE
    // used by processCommand when user command is CREATE
    void processCreate(std::string& cmd) {

        // CREATE <tablename> <N> <coltype1> <coltype2> ... <coltypeN> <colname1> <colname2> ... <colnameN>
        
        std::cin >> cmd; // cmd is tableName
        std::cout << "New table " << cmd << " with column(s) ";
        uint32_t numCols; std::cin >> numCols;

        // create a new table which has the right number of columns
        Table newTable;
        // add the new table to the unordered map
        auto iter = tables.find(cmd);
        if(iter == tables.end()) {
            tables.insert({cmd, newTable});
            iter = tables.find(cmd);
            iter->second.createTable(cmd, numCols);
        }
        else {
            std::cout << "Error during CREATE: Cannot create already existing table " << 
            cmd << '\n';
            getline(std::cin, cmd);
            return;
        }
    }

    // PROCESSREMOVE
    // used by processCommand when user command is REMOVE
    void processRemove(std::string& cmd) {

        // REMOVE <tablename>

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

    // PROCESSINSERT
    // used by processCommand when user command is INSERT
    void processInsert(std::string& cmd) {

        // INSERT INTO <tablename> <N> ROWS 
        // <value11> <value12> ... <value1M> 
        // <value21> <value22> ... <value2M> 
        // ... 
        // <valueN1> <valueN2> ... <valueNM>

        std::cin >> cmd; // get rid of INTO
        std::cin >> cmd; // cmd is now the tableName
        uint32_t numRowsInsert; std::cin >> numRowsInsert;

        auto iter = tables.find(cmd);
        if(iter != tables.end()) {
            iter->second.insert(numRowsInsert, cmd, cmd);
        }

        else {

            std::cout << "Error during INSERT: " << cmd 
                << " does not name a table in the database\n";

            // get rid of the rest of the command
            for(uint32_t i = 0; i < numRowsInsert; i++) {
                std::getline(std::cin, cmd);
            }

            return;
        }
    }

    // PROCESSPRINT
    // used by runShell when user command is PRINT
    void processPrint(std::string& cmd) {

        // PRINT FROM <tablename> <N> <print_colname1> <print_colname2> ... <print_colnameN> {WHERE <colname> <OP> <value> | ALL}        

        std::cin >> cmd; // get from of FROM
        std::string tableName; std::cin >> tableName;

        // make sure input tableName is valid
        auto iter = tables.find(tableName);
        if(iter != tables.end()) iter->second.print(cmd, tableName, quietMode);

        else {
            std::cout << "Error during PRINT: " << tableName 
                << " does not name a table in the database\n";
            std::getline(std::cin, cmd); return;
        }
    }

    // PROCESSDELETE
    // used by runShell when user command is DELETE
    void processDelete(std::string& cmd) {

        // DELETE FROM <tablename> WHERE <colname> <OP> <value>

        std::cin >> cmd; // get rid of FROM
        std::cin >> cmd; // cmd is the tableName
        auto iter = tables.find(cmd);
        if(iter != tables.end()) {
            iter->second.deleteRows(cmd, cmd);
        }
        else {
            std::cout << "Error during DELETE: " << cmd 
                << " does not name a table in the database\n";
            std::getline(std::cin, cmd);
        }
    }

    // PROCESSJOIN
    // used by runShell when user command is JOIN
    void processJoin(std::string& cmd) {

        // JOIN <tablename1> AND <tablename2> WHERE <colname1> = <colname2> AND PRINT <print_colname1> <1|2> <print_colname2> <1|2> ... <print_colnameN> <1|2>

        std::string tableName1; std::cin >> tableName1;
        std::cin >> cmd; std::string tableName2; std::cin >> tableName2;
        auto table1 = tables.find(tableName1);
        auto table2 = tables.find(tableName2);

        // validate both table names
        if(table1 == tables.end()) {
            std::cout << "Error during JOIN: " << tableName1 << " does not name a table in the database\n";
            std::getline(std::cin, cmd);
            return;
        }
        else if(table2 == tables.end()) {
            std::cout << "Error during JOIN: " << tableName2 << " does not name a table in the database\n";
            std::getline(std::cin, cmd);
            return;
        }
        //

        table1->second.joinTables(table2->second, cmd, tableName1, tableName2, quietMode);

    }

    // PROCESSGENERATE
    // used by runShell when user command is GENERATE
    void processGenerate(std::string& cmd) {
        std::cin >> cmd; // get rid of FOR
        std::string tableName; std::cin >> tableName;
        auto iter = tables.find(tableName);
        if(iter == tables.end()) {
            std::cout << "Error during GENERATE: " << tableName << " does not name a table in the database\n";
            std::getline(std::cin, cmd); return;
        }
        std::string indexType; std::cin >> indexType;
        std::cin >> cmd; std::cin >> cmd; std::string colName; std::cin >> colName;
        int returnVal = iter->second.generateIndex(tableName, indexType, colName);

        // summary print out
        if(returnVal == 0) {
            std::cout << "Created " << indexType << " index for table " << tableName 
                                            << " on column " << colName << '\n';
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////
    //----------------------END-PROCESS-MEMBER-FUNCTIONS----------------------//
    ////////////////////////////////////////////////////////////////////////////


public:

    ///////////////////////////////////////////////////////////////////////////
    //---------------------------COMMANDS-FOR-USER---------------------------//
    ///////////////////////////////////////////////////////////////////////////

    // constructor which handles command line arguments
    Silly(const int &argc, char *argv[]) {
        getMode(argc, argv);
    }

    void runShell() {
        
        // command string
        std::string cmd;

        // continue running the shell until the user quits
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
        //

        // final note from shell
        std::cout << "Thanks for being silly!\n";
    }
};

#endif