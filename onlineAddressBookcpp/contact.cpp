#include<iostream>
#include<string>
#include<algorithm>
#include<vector>
#include<iomanip>
#include "sqlite3.h"
#include "contact.h"
using namespace std;

struct ContactDet {
    string name;
    string phoneNo;
    string address;
    vector<string> group;
};

vector<ContactDet> contactDetails;

void createContactTable(sqlite3* db);
bool dbConnectionEstablishedForContact(sqlite3** db);
bool contactTableExist(sqlite3* db, string tabName);
bool isConnectedForContact(sqlite3* db);
bool validPhone(string phone);
bool contactExist(sqlite3* db, string tableName, string email, string phoneNo);
void fetchRecords(sqlite3* db, string email);
bool compareByName(ContactDet first, ContactDet second);
void print();
bool nameExist(sqlite3* db, string tableName, string email, string name);
bool compareSearch(string name, string subName);
bool compareByGroup(ContactDet first, ContactDet second);

//add user
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
        sqlite3_close(db);
        return;
    }

    while (nameExist(db, "contact", email, name)) {
        cout << "Name already exist" << endl << "Enter the name : ";
        cin >> name;
    }
    string groupName;
    
    if (!group.empty()) {
        groupName += group.at(0);
        for (int index = 1; index < group.size(); index++) {
            groupName += ",";
            groupName += group.at(index);
        }
    }
    
    string query = "INSERT INTO contact VALUES ('" + name + "','" + phoneNo + "','" + address + "','" + groupName + "','" + email + "')";
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

//search function
void contact::search(string email){
    sqlite3* db = nullptr;

    if (!dbConnectionEstablishedForContact(&db)) {
        return;
    }

    if (!contactTableExist(db, "contact")) {
        cout << "No contacts" << endl;
        sqlite3_close(db);
        return;
    }

    bool first = true;
    string searchName;
    cin.ignore();
    cout << "Enter the name to serach : ";
    getline(cin, searchName);
    fetchRecords(db, email);
    sort(contactDetails.begin(), contactDetails.end(), compareByName);

    for (auto& itr : contactDetails) {
        bool ok = compareSearch(itr.name, searchName);
      
        if (ok && first) {
            first = false;
            cout << left << setw(30) << "Name" << setw(15) << "Phone Number" << setw(60) << "Address" << setw(10) << "Group" << endl;
            cout << "------------------------------------------------------------------------------------------------------------------------" << endl;
        }
       
        if (ok) {
            cout << left << setw(30) << itr.name << setw(15) << itr.phoneNo << setw(60) << itr.address << setw(10);
            if (!itr.group.empty()) {
                cout << itr.group.at(0);
                for (int index = 1; index < itr.group.size(); index++) {
                    cout << "," << itr.group[index];
                }
            }
            cout<<endl;
        }
    }

    if (first) {
        cout << "No contacts matched to this name" << endl;
    }
    sqlite3_close(db);
}

void contact::deleteContact(string email) {
    viewContacts(email);
    sqlite3* db;

    if (!dbConnectionEstablishedForContact(&db)) {
        return;
    }

    string delName;
    cin.ignore();
    cout << "Enter the name you want to delete : ";
    getline(cin, delName);
    bool exist = nameExist(db, "contact", email, delName);

    if (!exist) {
        cout << "Please give the correct name"<<endl;
        sqlite3_close(db);
        return;
    }

    string query = "DELETE FROM contact WHERE name = '" + delName + "'AND email = '" + email + "'";
    char* err;
    int  res = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
    
    if (res != SQLITE_OK) {
        cout << "Deletion error: " << err << endl;
        sqlite3_free(err);
        sqlite3_close(db);
        return;
    }

    cout << "Contact deleted successfully.\n";
    sqlite3_close(db);
}

void contact::editContact() {}

//see the contacts
void contact::viewContacts(string email) {
    sqlite3* db = nullptr;

    if (!dbConnectionEstablishedForContact(&db)) {
        return;
    }

    if (!contactTableExist(db, "contact")) {
        cout << "No contacts" << endl;
        sqlite3_close(db);
        return;
    }
    fetchRecords(db, email);
   
    if (contactDetails.empty()) {
        cout << "No contacts" << endl;
        sqlite3_close(db);
        return;
    }

    sort(contactDetails.begin(),contactDetails.end(),compareByName);
    print();
    sqlite3_close(db);
}

//see the contacts by group
void contact::viewByGroup(string email){
    sqlite3* db;

    if (!dbConnectionEstablishedForContact(&db)) {
        return;
    }

    if (!contactTableExist(db, "contact")) {
        cout << "No contacts" << endl;
        sqlite3_close(db);
        return;
    }
    setExistingGroups(email);
    sort(contactDetails.begin(), contactDetails.end(), compareByName);

    if (contactDetails.empty()) {
        cout << "No contacts" << endl;
        sqlite3_close(db);
        return;
    }
    cout << endl << left << setw(30) << "Name" << setw(15) << "Phone Number" << setw(60) << "Address" << setw(10) << "Group" << endl;
    cout << "------------------------------------------------------------------------------------------------------------------------" << endl;

    for (auto itr : existingGroups) {
        for (auto vec : contactDetails) {
            for (auto gro : vec.group) {
                if (itr == gro) {
                    cout << left << setw(30) << vec.name << setw(15) << vec.phoneNo << setw(60) << vec.address<<itr<<endl;
                }
            }
        }
        cout << endl;
    }

    for (auto vec : contactDetails) {
        if (vec.group.at(0) == "") {
            cout << left << setw(30) << vec.name << setw(15) << vec.phoneNo << setw(60) << vec.address << endl;
        }
    }
    sqlite3_close(db);
}

//set the groups
void contact::setExistingGroups(string email) {
    sqlite3* db;

    if (!dbConnectionEstablishedForContact(&db)) {
        return;
    }
    existingGroups.clear();
    fetchRecords(db, email);

    for (auto& itr : contactDetails) {
        for (int index = 0; index < itr.group.size(); index++) {
            if (itr.group.at(index).length()>0) {
                existingGroups.insert(itr.group[index]);
            }
        }
    }
    sqlite3_close(db);
}

//create table function
void createContactTable(sqlite3* db) {
    char* err;
    const char* sql = "CREATE TABLE IF NOT EXISTS contact(name VARCHAR(40),phoneNo VARCHAR(20),address VARCHAR(100),contactGroup VARCHAR(100),email VARCHAR(50),PRIMARY KEY(phoneNo,email),FOREIGN KEY(email) REFERENCES userDetails(email) ON DELETE CASCADE);";
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

//phone number validation
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

//check if the contact exist or not
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

//fetch the contacts
void fetchRecords(sqlite3* db,string email) {
    for (auto& itr : contactDetails) {
        itr.group.clear();
    }

    contactDetails.clear();
    string query = "SELECT * FROM contact WHERE email = '" + email + "'";
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    if (!contactTableExist(db, "contact")) {
        return;
    }
   
    if (res != SQLITE_OK) {
        cout << "Failed to fetch records" << endl;
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string phoneNo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        string group = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        vector<string> groupVector;
        string groupName;
        for (int index = 0; index < group.length(); index++) {
            if (group[index] == ',') {
                groupVector.push_back(groupName);
                groupName.erase();
            }
            else {
                groupName += group[index];
            }
        }

        groupVector.push_back(groupName);
        contactDetails.push_back({ name,phoneNo,address,groupVector });
    }
    sqlite3_finalize(stmt);
}

//comparing the name
bool compareByName(ContactDet first, ContactDet second) {
    return first.name < second.name;
}

//display function
void print() {
    cout <<endl<< left << setw(30) << "Name" << setw(15) << "Phone Number" << setw(60) << "Address" << setw(10) << "Group" << endl;
    cout << "------------------------------------------------------------------------------------------------------------------------" << endl;

    for (auto& itr : contactDetails) {
        cout << left << setw(30) << itr.name << setw(15) << itr.phoneNo << setw(60) << itr.address;
       
        if (!itr.group.empty()) {
            cout << itr.group.at(0);
        
            for (int index = 1; index < itr.group.size(); index++) {
                cout << "," << itr.group[index];
            }
        }
        cout << endl;
    }
    cout << endl;
}

//check name exist in this email or not
bool nameExist(sqlite3* db, string tableName, string email, string name) {
    string query = "SELECT COUNT(*) FROM '" + tableName + "' WHERE email = '" + email + "' AND name = '" + name + "'";
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

//compare the names and search
bool compareSearch(string name, string subName) {
    if (name.length() < subName.length()) {
        return false;
    }

    for (int index = 0; index < name.length()-subName.length()+1; index++) {
        if (name[index] == subName[0]) {
            bool ok = true;
    
            for (int first = index, second = 0; second < subName.length(); first++, second++) {
                if (name[first] != subName[second]){
                    ok = false;
                }
            }
           
            if (ok) {
                return true;
            }
        }
    }
    return false;
}

//compare the strings thorugh group and name
bool compareByGroup(ContactDet first, ContactDet second) {
    if (first.group == second.group) {
        return first.name < second.name;
    }
    return first.group < second.group;
}