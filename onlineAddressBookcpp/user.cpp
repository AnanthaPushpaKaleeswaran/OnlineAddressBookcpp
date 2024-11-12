#include<iostream>
#include<string>
#include<vector>
#include "sqlite3.h"
#include "user.h"
using namespace std;

int publicKey;
int privateKey;
int n;

void createTable(sqlite3* db);
bool dbExist(sqlite3** db);
void createDatabase(sqlite3** db);
bool dbConnectionEstablished(sqlite3** db);
bool tableExist(sqlite3* db, string tabName);
bool isConnected(sqlite3* db);
bool validPass(string password);
bool emailExist(sqlite3* db, string tabName, string email);
bool validEmail(string email);
string encryptPassword(string message);
long long int encrypt(double message);
void setkeys();
int gcd(int a, int h);
bool userExist(sqlite3* db, string tableName, string email, string password);

//add user function
bool user::addUser(){
    sqlite3* db = nullptr;

    while (!validEmail(email)) {
        bool ok = validPass(password);
        cout << "Enter your email : ";
        cin >> email;
        if (!ok) {
            cout << "Enter your password : ";
            cin >> password;
        }
    }

    while (!validPass(password)) {
        cout << "Enter your password : ";
        cin >> password;
    }

    if (!dbExist(&db)) {
        createDatabase(&db);
    }

    if (!dbConnectionEstablished(&db)) {
        return false;
    }

    if (!tableExist(db, "user")) {
        createTable(db);
    }

    if (emailExist(db, "user", email)) {
        cout << "The user already exists." << endl << "Login to continue" << endl;
        sqlite3_close(db);
        return false;
    }

    password = encryptPassword(password);
    string query = "INSERT INTO user VALUES ('" + email + "','" + password + "')";
    char* err;
    int res = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

    if (res != SQLITE_OK) {
        cout << "Error in inserting record : " << err << endl;
        sqlite3_free(err);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    cout << "User addedd successfully" << endl;
    return true;
}

//validate user
bool user::validateUser() {
    sqlite3* db = nullptr;

    if (!dbExist(&db) || !dbConnectionEstablished(&db) || !tableExist(db,"user")) {
        cout << "Signup to continue" << endl;
        return false;
    }
    password = encryptPassword(password);

    if (userExist(db, "user", email, password)) {
        cout << "Email and password match successfully"<<endl;
    }
    else {
        cout << "No user exist." << endl<<"Check your email and password or Signup to continue" << endl;
        return false;
    }
    return true;
}

//create table function
void createTable(sqlite3* db) {
    char* err;
    const char* sql = "CREATE TABLE IF NOT EXISTS user(email VARCHAR(50) PRIMARY KEY, password VARCHAR(50));";
    int res = sqlite3_exec(db, sql, NULL, NULL, &err);

    if (res != SQLITE_OK) {
        cout << "There is an error in creating table." << endl;
        sqlite3_free(err);
    }
}

//check db exists or not
bool dbExist(sqlite3** db) {
    try {
        struct stat buffer;

        if (stat("onlineAddressBook.db", &buffer) != 0){
            throw "There is no database here";
        }
    }
    catch (const char* err) {
        cerr << err << endl;
        return false;
    }
    return true;
}

//db creation
void createDatabase(sqlite3** db) {
    cout << "Database was created successfully." << endl;
    int exit = sqlite3_open("onlineAddressBook.db", db);

    if (exit != SQLITE_OK) {
        cout << "Error in opening db\nPlease try again later"<<endl;
    }
    sqlite3_close(*db);
}

//check the connection
bool dbConnectionEstablished(sqlite3** db) {
    int exit = sqlite3_open("onlineAddressBook.db", db);

    if (exit != SQLITE_OK || !isConnected(*db)) {
        cerr << "There is no database here" << endl;
        sqlite3_close(*db);
        return false;
    }
    return true;
}

//table exist function
bool tableExist(sqlite3* db, string tabName) {
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
bool isConnected(sqlite3* db) {
    return db != nullptr && sqlite3_errcode(db) == SQLITE_OK;
}

//password validation
bool validPass(string password) {
    if (password.length() < 8) {
        cout << "Please enter the password above 8 characters" << endl;
        return false;
    }

    if (password.length() >= 50) {
        cout << "Please enter the password below 50 characters" << endl;
        return false;
    }

    bool num = false;
    bool lower = false;
    bool upper = false;
    bool symbol = false;

    for (int i = 0; i < password.length(); i++) {
        if (isdigit(password[i])) {
            num = true;
        }
        else if (islower(password[i])) {
            lower = true;
        }
        else if (isupper(password[i])) {
            upper = true;
        }
        else if (!isalnum(password[i])) {
            symbol = true;
        }
    }

    if (!num || !lower || !upper || !symbol) {
        cout << "The password must contain a number, a lowercase alphabet, a uppercase alphabet and a symbol";
        cout << endl;
        return false;
    }
    return true;
}

//checking for email duplication
bool emailExist(sqlite3* db, string tabName, string email) {
    string query = "SELECT COUNT(*) FROM '" + tabName + "'WHERE email='" + email + "'";
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    if (res != SQLITE_OK) {
        cout << "Error checking for existing record: " << sqlite3_errmsg(db) << endl;
        return -1;
    }
    int count = 0;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count > 0;
}

//email validation
bool validEmail(string email) {
    bool at = false;
    bool dot = false;
    bool alpha = false;

    for (int i = 0; i < email.length(); i++) {
        if (email[i] == '@') {
            at = true;
        }
        else if (email[i] == '.') {
            dot = true;
        }
        else if (isalpha(email[i])) {
            alpha = true;
        }
    }

    if (!at || !dot || !alpha) {
        cout << "Please enter the valid email" << endl;
        return false;
    }
    return true;
}

//password encryption
string encryptPassword(string message) {
    setkeys();
    vector<int> form;

    for (auto& letter : message) {
        form.push_back(encrypt((int)letter));
    }
    string encryption;

    for (auto& itr : form) {
        encryption += to_string(itr);
    }
    return encryption;
}

//encrypt function
long long int encrypt(double message)
{
    int e = publicKey;
    long long int encrpyted_text = 1;

    while (e--) {
        encrpyted_text *= message;
        encrpyted_text %= n;
    }
    return encrpyted_text;
}

//key setting function
void setkeys()
{
    int prime1 = 61;
    int prime2 = 67;
    n = prime1 * prime2;
    int fi = (prime1 - 1) * (prime2 - 1);
    int e = 2;

    while (1) {
        if (gcd(e, fi) == 1)
            break;
        e++;
    } // d = (k*Φ(n) + 1) / e for some integer k
   
    publicKey = e;
    int d = 2;
    
    while (1) {
        if ((d * e) % fi == 1) {
            break;
        }
        d++;
    }
    privateKey = d;
}

//gcd of two numbers
int gcd(int a, int h)
{
    int temp;

    while (1) {
        temp = a % h;
        if (temp == 0) {
            return h;
        }
    
        a = h;
        h = temp;
    }
}

//check user exist or not
bool userExist(sqlite3* db, string tableName, string email, string password) {
    string query = "SELECT COUNT(*) FROM '" + tableName + "' WHERE email = '" + email  + "' AND password = '" + password + "'";
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