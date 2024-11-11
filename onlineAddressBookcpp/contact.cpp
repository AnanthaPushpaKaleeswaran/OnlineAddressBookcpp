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

void contact::viewByGroup() {}

//create table function
void createContactTable() {
    sqlite3* db = nullptr;

    if (!dbConnectionEstablishedForContact(&db)) { // Assuming dbConnectionEstablished() is used here
        sqlite3_close(db); // Ensure database is closed even if connection fails
        return;
    }

    if (contactTableExist(db, "contactDetails")) {
        sqlite3_close(db);
        return;
    }
    char* err;
    const char* sql = "CREATE TABLE IF NOT EXISTS contactDetails(name VARCHAR(40),phoneNo VARCHAR(20),address VARCHAR(100),contactGroup VARCHAR(30),email VARCHAR(50),PRIMARY KEY(phoneNo,email),FOREIGN KEY(email) REFERENCES userDetails(email) ON DELETE CASCADE);";
    int res = sqlite3_exec(db, sql, NULL, NULL, &err);

    if (res != SQLITE_OK) {
        cout << "There is an error in creating table." << endl;
        sqlite3_free(err);
    }
    sqlite3_close(db);
}

//connection established or not
bool dbConnectionEstablishedForContact(sqlite3** db) {
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
    string query = "SELECT * FROM sqlite_master WHERE type='table' AND name='" + tabName + "'";

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "OOPS error occurred " << sqlite3_errmsg(db) << endl;
        return false;
    }

    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return exists;
}

//check if the db is connected or not
bool isConnectedForContact(sqlite3* db) {
    return db != nullptr && sqlite3_errcode(db) == SQLITE_OK;
}