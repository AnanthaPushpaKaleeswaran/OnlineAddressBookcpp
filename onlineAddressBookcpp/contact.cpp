#include<iostream>
#include<string>
#include "sqlite3.h"
#include "contact.h"
using namespace std;
bool contactFlag = false;
void createContactTable();
bool dbConnectionEstablishedForContact(sqlite3** db);
bool contactTableExist(sqlite3* db, string tabName);
bool isConnectedForContact(sqlite3* db);
void contact::addContact() {
    sqlite3* db = nullptr;
    if (!dbConnectionEstablishedForContact(&db)) {
        return;
    }
    if (!contactTableExist(db, "contactDetails")) {
        if (!contactFlag) {
            createContactTable();
        }
        else {
            return;
        }
    }
    createContactTable();
    sqlite3_close(db);
}
void contact::search() {}
void contact::deleteContact() {}
void contact::editContact() {}
void contact::viewContacts() {}
//create table function
void createContactTable() {
    //initialize the db as nullptr
    sqlite3* db = nullptr;
    //check the db connection
    bool dbConnection = dbConnectionEstablishedForContact(&db);
    if (!dbConnection) {
        return;
    }
    //check if table exist or not
    if (contactTableExist(db, "contactDetails")) {
        return;
    }
    char* err;
    //table creation query
    const char* sql = "CREATE TABLE IF NOT EXISTS contactDetails(name VARCHAR(40),phoneNo VARCHAR(20),address VARCHAR(100),email VARCHAR(50),PRIMARY KEY(phoneNo,email),FOREIGN KEY(email) REFERENCES userDetails(email) ON DELETE CASCADE);";
    int res = sqlite3_exec(db, sql, NULL, NULL, &err);
    if (res != SQLITE_OK) {
        cout << "There is an error in creating table." << endl;
        sqlite3_free(err);
    }
    //close the db
    sqlite3_close(db);
}
bool dbConnectionEstablishedForContact(sqlite3** db) {
    // Open database
    try {
        int exit = sqlite3_open("onlineAddressBook.db", db);
        if (exit != SQLITE_OK || !isConnectedForContact(*db)) {
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
bool contactTableExist(sqlite3* db, string tabName) {
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
bool isConnectedForContact(sqlite3* db) {
    return db != nullptr && sqlite3_errcode(db) == SQLITE_OK;
}