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
set<string> existingGroups;

void setExistingGroups(string email);
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
void getContactInput(string* name, string* phoneNo, string* address, vector<string>* group, string email, bool check);
void getNewGroup(string* group);
bool individualGroupExist(vector<string> group, string groupName);
bool groupExist(string group);

//add user
void contact::addContact(string email) {
    // Collect contact details from the user
    getContactInput(&name, &phoneNo, &address, &group, email, false);
    setContact(name, phoneNo, address, group);
    sqlite3* db = nullptr;

    // Check if database connection is established for contacts
    if (!dbConnectionEstablishedForContact(&db)) {
        return;
    }

    // Ensure the contact table exists before inserting data
    if (!contactTableExist(db, "contact")) {
        createContactTable(db);
    }

    // Loop to validate and re-enter a valid phone number
    while (!validPhone(phoneNo)) {
        cout << "Enter the phone number : ";
        cin >> phoneNo;
    }

    // Check if the contact with the same phone number already exists
    if (contactExist(db, "contact", email, phoneNo)) {
        cout << "The contact already exists" << endl;
        sqlite3_close(db);
        return;
    }

    // Check for duplicate contact names and re-enter if necessary
    while (nameExist(db, "contact", email, name)) {
        cout << "Name already exists" << endl << "Enter the name : ";
        cin >> name;
    }

    string groupName;
    if (!group.empty()) {
        groupName += group.at(0);

        // Format group names with commas
        for (int index = 1; index < group.size(); index++) {
            groupName += ",";
            groupName += group.at(index);
        }
    }

    // Insert new contact data into the database
    const string query = "INSERT INTO contact VALUES ('" + name + "','" + phoneNo + "','" + address + "','" + groupName + "','" + email + "')";
    char* err;
    int res = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

    if (res != SQLITE_OK) {
        cout << "Error in inserting record : " << err << endl;
        sqlite3_free(err);
        sqlite3_close(db);
        return;
    }

    sqlite3_close(db);
    cout << "Contact added successfully" << endl;
    return;
}

// Search for contacts by name
void contact::search(string email) {
    sqlite3* db = nullptr;

    if (!dbConnectionEstablishedForContact(&db)) {
        return;
    }

    // Check if contact table exists
    if (!contactTableExist(db, "contact")) {
        cout << "No contacts to search" << endl;
        sqlite3_close(db);
        return;
    }

    // Retrieve and sort all contact records
    fetchRecords(db, email);
    sort(contactDetails.begin(), contactDetails.end(), compareByName);

    if (contactDetails.empty()) {
        cout << "No contacts to search" << endl;
        sqlite3_close(db);
        return;
    }

    bool first = true;
    string searchName;
    cin.ignore();
    cout << "Enter the name to search : ";
    getline(cin, searchName);
    cout << endl;

    for (auto& itr : contactDetails) {
        bool ok = compareSearch(itr.name, searchName);

        if (ok && first) {
            first = false;
            cout << left << setw(25) << "Name" << setw(15) << "Phone Number" << setw(50) << "Address" << setw(10) << "Group" << endl;
            cout << "------------------------------------------------------------------------------------------------------------------------" << endl;
        }

        if (ok) {
            cout << left << setw(25) << itr.name << setw(15) << itr.phoneNo << setw(50) << itr.address << setw(10);

            if (!itr.group.empty()) {
                cout << itr.group.at(0);

                for (int index = 1; index < itr.group.size(); index++) {
                    cout << "," << itr.group[index];
                }
            }
            cout << endl;
        }
    }

    if (first) {
        cout << "No contacts matched to this name" << endl;
    }
    sqlite3_close(db);
}

//delete the contact
void contact::deleteContact(string email) {
    // Display all contacts for the given email
    viewContacts(email);
    sqlite3* db;

    if (!dbConnectionEstablishedForContact(&db)) {
        return;
    }

    if (contactDetails.empty()) {
        sqlite3_close(db);
        return;
    }

    string delName;
    cin.ignore();
    cout << "Enter the name you want to delete : ";
    getline(cin, delName);

    // Check if the contact name exists in the database
    bool exist = nameExist(db, "contact", email, delName);

    if (!exist) {
        cout << "Please provide the correct name" << endl;
        sqlite3_close(db);
        return;
    }

    // Construct and execute the deletion query
    const string query = "DELETE FROM contact WHERE name = '" + delName + "' AND email = '" + email + "'";
    char* err;
    int res = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

    if (res != SQLITE_OK) {
        cout << "Deletion error: " << err << endl;
        sqlite3_free(err);
        sqlite3_close(db);
        return;
    }

    cout << "Contact deleted successfully.\n";
    sqlite3_close(db);
}

// Function to edit an existing contact
void contact::editContact(string email) {
    // Display all contacts for the given email
    viewContacts(email);
    sqlite3* db;

    if (!dbConnectionEstablishedForContact(&db)) {
        return;
    }

    if (contactDetails.empty()) {
        sqlite3_close(db);
        return;
    }

    string editName;
    cin.ignore();
    cout << "Enter the name to edit : ";
    getline(cin, editName);

    // Get updated contact details from the user
    getContactInput(&name, &phoneNo, &address, &group, email, true);
    setContact(name, phoneNo, address, group);

    if (!nameExist(db, "contact", email, editName)) {
        cout << "Please provide the correct name" << endl;
        sqlite3_close(db);
        return;
    }

    while (!validPhone(phoneNo)) {
        cout << "Enter the phone number : ";
        cin >> phoneNo;
    }

    // Format group names with commas
    string groupName;
    if (!group.empty()) {
        groupName += group.at(0);

        for (int index = 1; index < group.size(); index++) {
            groupName += ",";
            groupName += group.at(index);
        }
    }

    // Construct and execute the update query
    const string query = "UPDATE contact SET name = '" + name + "', phoneNo = '" + phoneNo + "', address = '" + address + "', contactGroup = '" + groupName + "' WHERE name = '" + editName + "'";
    char* err;
    int res = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

    if (res != SQLITE_OK) {
        cerr << "Error in update : " << err << endl;
        sqlite3_free(err);
        sqlite3_close(db);
        return;
    }

    cout << "Contact modified successfully" << endl;
    sqlite3_close(db);
}

//see the contacts
void contact::viewContacts(string email) {
    sqlite3* db = nullptr;

    if (!dbConnectionEstablishedForContact(&db)) {
        return;
    }

    if (!contactTableExist(db, "contact")) {
        cout << "No contacts to display" << endl;
        sqlite3_close(db);
        return;
    }

    // Retrieve all contact records associated with the email
    fetchRecords(db, email);

    // If no contacts found, exit the function
    if (contactDetails.empty()) {
        cout << "No contacts to display" << endl;
        sqlite3_close(db);
        return;
    }

    // Sort contacts by name for ordered display
    sort(contactDetails.begin(), contactDetails.end(), compareByName);

    // Print contacts to console
    print();
    sqlite3_close(db);
}

// Function to view contacts by group
void contact::viewByGroup(string email) {
    sqlite3* db;

    if (!dbConnectionEstablishedForContact(&db)) {
        return;
    }

    if (!contactTableExist(db, "contact")) {
        cout << "No contacts to display" << endl;
        sqlite3_close(db);
        return;
    }

    setExistingGroups(email);
    sort(contactDetails.begin(), contactDetails.end(), compareByName);

    // If no contacts are available, exit the function
    if (contactDetails.empty() || existingGroups.empty()) {
        cout << "No contacts in group" << endl;
        sqlite3_close(db);
        return;
    }

    int selectGroup;

    // Loop to prompt user for a valid group selection
    while (true) {
        cout << "Select the group to display" << endl;
        int count = 1;

        // Display all existing groups to the user
        for (auto itr : existingGroups) {
            cout << count++ << "." << itr << endl;
        }

        cout << "Enter your choice : ";
        cin >> selectGroup;

        int size = existingGroups.size();

        // Check if the selected group is within the valid range
        if (selectGroup < 1 || selectGroup > size) {
            cout << "Please enter the correct group value" << endl << endl;
            continue;
        }
        break;
    }

    // Retrieve the selected group name
    string groupName = *next(existingGroups.begin(), selectGroup - 1);

    // Display header for the contact information
    cout << endl << left << setw(25) << "Name" << setw(15) << "Phone Number" << setw(50) << "Address" << setw(10) << "Group" << endl;
    cout << "------------------------------------------------------------------------------------------------------------------------" << endl;

    for (auto vec : contactDetails) {
        for (auto gro : vec.group) {
            if (groupName == gro) {
                cout << left << setw(25) << vec.name << setw(15) << vec.phoneNo << setw(50) << vec.address << groupName << endl;
            }
        }
    }

    sqlite3_close(db);
}

//see the contacts those who are not in group
void contact::viewGroupNone(string email) {
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

    // If no contacts are available, exit the function
    if (contactDetails.empty()) {
        cout << "No contacts" << endl;
        sqlite3_close(db);
        return;
    }
    bool check = false;

    // Loop through the contacts to check if any contact doesn't belong to any group
    for (auto vec : contactDetails) {
        if (vec.group.at(0) == "") {
            check = true;
            break;
        }
    }

    // If no contacts without a group, display a message and return
    if (!check) {
        cout << "No contacts without group" << endl;
        return;
    }

    // Display the header for contacts with no group
    cout << endl << left << setw(25) << "Name" << setw(15) << "Phone Number" << setw(50) << "Address" << setw(10) << "Group" << endl;
    cout << "------------------------------------------------------------------------------------------------------------------------" << endl;

    // Loop through and display contacts that don't belong to any group
    for (auto vec : contactDetails) {
        if (vec.group.at(0) == "") {
            cout << left << setw(25) << vec.name << setw(15) << vec.phoneNo << setw(50) << vec.address << endl;
        }
    }

    sqlite3_close(db);
}

//set the contact
void contact::setContact(string name, string phoneNo, string address, vector<string> group) {
    this->name = name;
    this->phoneNo = phoneNo;
    this->address = address;
    this->group = group;
}

//set the groups
void setExistingGroups(string email) {
    sqlite3* db;

    if (!dbConnectionEstablishedForContact(&db)) {
        return;
    }

    existingGroups.clear(); //clear the exiting group
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
    const string sql = "CREATE TABLE IF NOT EXISTS contact(name VARCHAR(40),phoneNo VARCHAR(20),address VARCHAR(100),contactGroup VARCHAR(100),email VARCHAR(50),PRIMARY KEY(phoneNo,email),FOREIGN KEY(email) REFERENCES userDetails(email) ON DELETE CASCADE);";
    int res = sqlite3_exec(db, sql.c_str(), NULL, NULL, &err);

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
    const string query = "SELECT * FROM sqlite_master WHERE type='table' AND name='" + tabName + "'";

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
// Validate if the phone number is a 10-digit number
bool validPhone(string phone) {
    if (phone.length() != 10) {
        cout << "The phone number must contain 10 numbers" << endl;
        return false; // Invalid if phone number is not 10 digits long
    }
    bool notDigit = false;

    for (int index = 0; index < phone.length(); index++) {
        if (!isdigit(phone[index])) {
            notDigit = true; // Set flag if a non-digit character is found
        }
    }

    if (notDigit) {
        cout << "The phone number must contains numbers only" << endl;
        return false;
    }
    return true;
}

// Check if the contact already exists in the database based on phone number and email
bool contactExist(sqlite3* db, string tableName, string email, string phoneNo) {
    const string query = "SELECT COUNT(*) FROM '" + tableName + "' WHERE email = '" + email + "' AND phoneNo = '" + phoneNo + "'";
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

// Fetch all contact records from the database for the specified email
void fetchRecords(sqlite3* db, string email) {
    for (auto& itr : contactDetails) {
        itr.group.clear(); // Clear any previous group data
    }

    if (!contactTableExist(db, "contact")) {
        return;
    }

    contactDetails.clear(); // Clear all contact details before fetching new data
    const string query = "SELECT * FROM contact WHERE email = '" + email + "'";
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    if (res != SQLITE_OK) {
        cout << "Failed to fetch records" << endl;
        return;
    }

    // Retrieve records and populate contactDetails
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string phoneNo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        string group = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        vector<string> groupVector;
        string groupName;

        // Split the group string into individual groups
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
        contactDetails.push_back({ name, phoneNo, address, groupVector });
    }
    sqlite3_finalize(stmt);
}

//comparing the name
bool compareByName(ContactDet first, ContactDet second) {
    return first.name < second.name;
}

//display function
void print() {
    cout << endl << left << setw(25) << "Name" << setw(15) << "Phone Number" << setw(50) << "Address" << setw(10) << "Group" << endl;
    cout << "------------------------------------------------------------------------------------------------------------------------" << endl;

    // Loop through all the contacts and display their details
    for (auto& itr : contactDetails) {
        cout << left << setw(25) << itr.name << setw(15) << itr.phoneNo << setw(50) << itr.address;

        // Print the groups for each contact, if they exist
        if (!itr.group.empty()) {
            cout << itr.group.at(0);

            // Print additional groups separated by commas
            for (int index = 1; index < itr.group.size(); index++) {
                cout << "," << itr.group[index];
            }
        }
        cout << endl;
    }
    cout << endl;
}

// Check if a contact with the given name exists for the specified email in the database
bool nameExist(sqlite3* db, string tableName, string email, string name) {
    const string query = "SELECT COUNT(*) FROM '" + tableName + "' WHERE email = '" + email + "' AND name = '" + name + "'";
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
    return count > 0; // Return true if the name exists, false otherwise
}

// Compare the given name with a substring to check if the substring is present
bool compareSearch(string name, string subName) {
    if (name.length() < subName.length()) {
        return false; // Return false if the substring is longer than the name
    }

    for (int index = 0; index < name.length() - subName.length() + 1; index++) {
        if (name[index] == subName[0]) {
            bool ok = true;

            for (int first = index, second = 0; second < subName.length(); first++, second++) {
                if (name[first] != subName[second]) {
                    ok = false;
                }
            }

            if (ok) {
                return true;
            }
        }
    }
    return false; // Return false if no match is found
}

// Get contact details input 
void getContactInput(string* name, string* phoneNo, string* address, vector<string>* group, string email, bool check) {
    if (!check) {
        cin.ignore(); // Clear any previous input from the buffer
    }

    // Prompt the user to enter contact details
    cout << "Enter the name : ";
    getline(cin, *name);

    cout << "Enter the phone number : ";
    cin >> *phoneNo;

    cin.ignore(); // Ignore the remaining newline character
    cout << "Enter the address : ";
    getline(cin, *address);

    group->clear();

    while (1) {
        bool ok = false;
        setExistingGroups(email);
        int count = 1;
        cout << endl << "Select a group" << endl;

        for (auto itr : existingGroups) {
            itr[0] = toupper(itr[0]); // Capitalize the first letter of each group name
            cout << count++ << "." << itr << endl;
        }

        cout << count++ << ".New Group" << endl;
        cout << count << ".Exit" << endl;
        int groupChoice;
        cout << "Enter your choice : ";
        cin >> groupChoice;
        string groupName;
        auto it = existingGroups.begin();

        //true when the user select the existing group
        if (groupChoice >= 1 && groupChoice < count - 1) {
            advance(it, groupChoice - 1);
            groupName = *it;

            if (individualGroupExist(*group, groupName)) {
                cout << "The group was already added." << endl;
            }
            else {
                group->push_back(groupName);
                cout << "Group added was successfully!" << endl;
            }
        }
        //true when the user select the new group
        else if (groupChoice == count - 1) {
            getNewGroup(&groupName);
            bool exist = groupExist(groupName);

            if (groupExist(groupName)) {
                cout << "The group already exists." << endl;
            }
            else if (individualGroupExist(*group, groupName)) {
                cout << "The group was already added" << endl;
            }
            else {
                group->push_back(groupName);
                cout << "Group added successfully!" << endl;
            }
        }
        //true when the user exit
        else if (groupChoice == count) {
            cout << "Exiting group selection" << endl;
            return;
        }
        else {
            cout << "Invalid choice." << endl << "Please try again later" << endl;
        }
    }
}

//get the new group
void getNewGroup(string* group) {
    cout << "Enter the group name : ";
    cin >> *group;
}

//check the group was added to the contact or not
// Check if the specified group name already exists within the given group vector (case-insensitive comparison)
bool individualGroupExist(vector<string> group, string groupName) {
    int groupLen = groupName.length();

    // Loop through the groups to check for a match
    for (auto& itr : group) {
        bool individual = true;

        if (itr.length() != groupLen) {
            individual = false;
            continue;
        }

        for (int index = 0; index < groupLen; index++) {
            if (tolower(itr[index]) != tolower(groupName[index])) {
                individual = false;
                break;
            }
        }

        if (individual) {
            return true;
        }
    }
    return false; // Return false if no match is found
}

// Check if the specified group name already exists within the existing groups (case-insensitive comparison)
bool groupExist(string group) {
    int groupLen = group.length();

    // Loop through the existing groups to check for a match
    for (auto& itr : existingGroups) {
        bool individual = true;

        // Skip if the length doesn't match
        if (itr.length() != groupLen) {
            individual = false;
            continue;
        }

        // Compare each character case-insensitively
        for (int index = 0; index < groupLen; index++) {
            if (tolower(itr[index]) != tolower(group[index])) {
                individual = false;
                break;
            }
        }

        if (individual) {
            return true;
        }
    }
    return false; // Return false if no match is found
}
