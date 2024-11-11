#include<iostream>
#include<string>
#include "sqlite3.h"
#include "user.h"
using namespace std;
bool flag = false;
void createTable();
bool dbExist(sqlite3** db);
void createDatabase(sqlite3** db);
bool dbConnectionEstablished(sqlite3** db);
bool tableExist(sqlite3* db, string tabName);
bool isConnected(sqlite3* db);
bool user::addUser() {
    sqlite3* db = nullptr;

    if (!dbExist(&db)) {
        return false;
    }
    if (!dbConnectionEstablished(&db)) {
        return false;
    }
    if (!tableExist(db, "userDetails")) {
        return false;
    }
    return true;
}
bool user::validateUser() {
    return true;
}
string user::encryption() {
    return " ";
}
//create table function
void createTable() {
    //initialize the db as nullptr
    sqlite3* db = nullptr;
    bool databaseExist = dbExist(&db);
    if (!databaseExist) {
        return;
    }
    //check the db connection
    bool dbConnection = dbConnectionEstablished(&db);
    if (!dbConnection) {
        return;
    }
    //check if table exist or not
    if (tableExist(db, "userDetails")) {
        return;
    }
    char* err;
    //table creation query
    const char* sql = "CREATE TABLE IF NOT EXISTS userDetails(email VARCHAR(50) PRIMARY KEY, password VARCHAR(20));";
    int res = sqlite3_exec(db, sql, NULL, NULL, &err);
    if (res != SQLITE_OK) {
        cout << "There is an error in creating table." << endl;
        sqlite3_free(err);
    }
    //close the db
    sqlite3_close(db);
}
bool dbExist(sqlite3** db) {
    try {
        struct stat buffer;
        if (stat("onlineAddressBook.db", &buffer) != 0) {
            if (!flag) {
                //if user first entered into the application then database is created
                createDatabase(db);
                createTable();
                flag = true;
            }
            else {
                throw "There is no database here";
            }
        }
    }
    catch (const char* err) {
        cerr << err << endl;
        return false;
    }
    return true;
}
void createDatabase(sqlite3** db) {
    cout << "There is no database here." << endl << "Database was created successfully." << endl;
    //db creation
    int exit = sqlite3_open("onlineAddressBook.db", db);
    if (exit != SQLITE_OK) {
        cout << "Somewaht rror in opening db\nPlease try again few minutes later";
    }
    sqlite3_close(*db);
}
bool dbConnectionEstablished(sqlite3** db) {
    // Open database
    try {
        int exit = sqlite3_open("onlineAddressBook.db", db);
        if (exit != SQLITE_OK || !isConnected(*db)) {
            throw "There is no database here";
        }
    }
    catch (const char* err) {
        cerr << err << endl;
        sqlite3_close(*db); // Ensure database is closed if it was opened
        return false;
    }
    return true;
}
//table exist function
bool tableExist(sqlite3* db, string tabName) {
    sqlite3_stmt* stmt;
    //query for checking table exist or not
    string query = "SELECT * FROM sqlite_master WHERE type='table' AND name='" + tabName + "'";
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "OOPS error occurred " << sqlite3_errmsg(db) << endl;
        return false;
    }
    // Check if the table exists by stepping through the statement
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    // Finalize the statement to release resources
    sqlite3_finalize(stmt);
    return exists;
}
//check if the db is connected or not
bool isConnected(sqlite3* db) {
    return db != nullptr && sqlite3_errcode(db) == SQLITE_OK;
}