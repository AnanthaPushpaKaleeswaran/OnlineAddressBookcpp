#include<iostream>
#include<string>
#include "sqlite3.h"
#include "contact.h"
using namespace std;

void createContactTable(sqlite3* db);
bool dbConnectionEstablishedForContact(sqlite3** db);
bool contactTableExist(sqlite3* db, string tabName);
bool isConnectedForContact(sqlite3* db);
bool validPhone(string phone);
bool contactExist(sqlite3* db, string tableName, string email, string phoneNo);

void contact::addContact(string email) {
    sqlite3* db = nullptr;

    if (!dbConnectionEstablishedForContact(&db)) {
        return;
    }

    if (!contactTableExist(db, "contact")) {
        createContactTable(db);
    }

    while (!validPhone(phoneNo)) {
        cout << "Enter the phone number : ";
        cin >> phoneNo;
    }

    if (contactExist(db, "contact", email, phoneNo)) {
        cout << "The contact already exist" << endl;
        return;
    }

    string query = "INSERT INTO contact VALUES ('" + name + "','" + phoneNo + "','" + address + "','" + group + "','" + email + "')";
    char* err;
    int res = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

    if (res != SQLITE_OK) {
        cout << "Error in inserting record : " << err << endl;
        sqlite3_free(err);
        sqlite3_close(db);
        return;
    }

    sqlite3_close(db);
    cout << "Contact addedd successfully" << endl;
    return;
}

void contact::search() {}

void contact::deleteContact() {}

void contact::editContact() {}

void contact::viewContacts() {}

void contact::viewByGroup() {}

//create table function
void createContactTable(sqlite3* db) {
    char* err;
    const char* sql = "CREATE TABLE IF NOT EXISTS contact(name VARCHAR(40),phoneNo VARCHAR(20),address VARCHAR(100),contactGroup VARCHAR(30),email VARCHAR(50),PRIMARY KEY(phoneNo,email),FOREIGN KEY(email) REFERENCES userDetails(email) ON DELETE CASCADE);";
    int res = sqlite3_exec(db, sql, NULL, NULL, &err);

    if (res != SQLITE_OK) {
        cout << "There is an error in creating table." << endl;
        sqlite3_free(err);
    }
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

bool validPhone(string phone) {
    bool notDigit = false;
    for (int index = 0; index < phone.length(); index++) {
        if (!isdigit(phone[index])) {
            notDigit = true;
        }
    }
    if (notDigit) {
        cout << "The phone number must contains numbers only"<<endl;
        return false;
    }
    return true;
}

bool contactExist(sqlite3* db, string tableName, string email, string phoneNo) {
    string query = "SELECT COUNT(*) FROM '" + tableName + "' WHERE email = '" + email + "' AND phoneNo = '" + phoneNo + "'";
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    if (res != SQLITE_OK) {
        cout << "Error checking for existing record: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    int count = 0;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count > 0;
}