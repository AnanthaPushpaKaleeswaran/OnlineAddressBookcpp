#include<iostream>
#include<string>
#include<random>
#include<ctime>
#include<vector>
#include<regex>
#include "sqlite3.h"
#include "user.h"
using namespace std;

int publicKey;
int privateKey;
int n;
struct UserDet {
    string email;
    string pass;
};
vector<UserDet> userDetails;

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
string decryptPassword(string password);
long long int decrypt(int msg);
void setkeys();
int gcd(int a, int h);
char getRandomAlphabet();
void fetchUsers(sqlite3* db);

//add user function
bool user::addUser() {
    sqlite3* db = nullptr;

    // Ensure email and password are valid
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

    // Check if the database exists and create if necessary
    if (!dbExist(&db)) {
        createDatabase(&db);
    }

    if (!dbConnectionEstablished(&db)) {
        return false;
    }

    // Check if the user table exists and create if necessary
    if (!tableExist(db, "user")) {
        createTable(db);
    }

    // Check if the email already exists in the database
    if (emailExist(db, "user", email)) {
        cout << "The user already exists." << endl << "Login to continue" << endl;
        sqlite3_close(db);
        return false;
    }

    password = encryptPassword(password);
    const string query = "INSERT INTO user VALUES ('" + email + "','" + password + "')";
    char* err;
    int res = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

    // Handle any errors during record insertion
    if (res != SQLITE_OK) {
        cout << "Error in inserting record : " << err << endl;
        sqlite3_free(err);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    cout << "User added successfully" << endl;
    return true;
}


//validate user
bool user::validateUser() {
    sqlite3* db = nullptr;

    // Check if database exists, is connected, and if the 'user' table exists
    if (!dbExist(&db) || !dbConnectionEstablished(&db) || !tableExist(db, "user")) {
        cout << "Signup to continue" << endl;
        return false;
    }
    fetchUsers(db);
    
    for (auto itr : userDetails) {
        if (itr.email == email && decryptPassword(itr.pass) == password) {
            cout << "Logging in..." << endl;
            return true;
        }
    }

    cout << "No user exist." << endl << "Check your email and password or sign up to continue" << endl;
    return false;
}


//create table function
void createTable(sqlite3* db) {
    // SQL command to create the 'user' table if it does not already exist
    char* err;
    const string sql = "CREATE TABLE IF NOT EXISTS user(email VARCHAR(50) PRIMARY KEY, password VARCHAR(50));";
    int res = sqlite3_exec(db, sql.c_str(), NULL, NULL, &err);

    if (res != SQLITE_OK) {
        cout << "There is an error in creating table." << endl;
        sqlite3_free(err);
    }
}

// Check if the database file exists
bool dbExist(sqlite3** db) {
    try {
        struct stat buffer;

        if (stat("onlineAddressBook.db", &buffer) != 0) {
            throw "There is no database here.";
        }
    }
    catch (const char* err) {
        cerr << err << endl;
        return false;
    }
    return true;
}

// Create the database file if it does not exist
void createDatabase(sqlite3** db) {
    cout << "Database was created successfully." << endl;
    int exit = sqlite3_open("onlineAddressBook.db", db);

    if (exit != SQLITE_OK) {
        cout << "Error in opening db\nPlease try again later" << endl;
    }
    sqlite3_close(*db);
}

// Establish a connection to the database file and check for success
bool dbConnectionEstablished(sqlite3** db) {
    int exit = sqlite3_open("onlineAddressBook.db", db);

    if (exit != SQLITE_OK || !isConnected(*db)) {
        cerr << "There is no database here" << endl;
        sqlite3_close(*db);
        return false;
    }
    return true;
}

// Check if the specified table exists in the database
bool tableExist(sqlite3* db, string tabName) {
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

// Verify if the database is successfully connected
bool isConnected(sqlite3* db) {
    return db != nullptr && sqlite3_errcode(db) == SQLITE_OK;
}

// Validate the format of the password
bool validPass(string password) {
    // Check password length constraints
    regex expression("^(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.*[@#$%^&-+=()]).{8,50}");
    
    if (regex_match(password, expression)) {
        return true;
    }

    cout << "The password length must between 8 and 50 characters." << endl;
    cout<<"The password must contain a lower alphabet,a upper alphabet,a number and a symbol" << endl;
    return false;
}

//checking for email duplication
bool emailExist(sqlite3* db, string tabName, string email) {
    // SQL query to count entries with the specified email
    const string query = "SELECT COUNT(*) FROM '" + tabName + "'WHERE email='" + email + "'";
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    if (res != SQLITE_OK) {
        cout << "Error checking for existing record: " << sqlite3_errmsg(db) << endl;
        return -1;
    }

    int count = 0;

    // Retrieve the count result from the query
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count > 0;
}

// Email format validation function
bool validEmail(string email) {
    regex regEx("[a-zA-Z0-9._%+-]+@[a-z.-]+\\.[a-z]{2,}$");
   
    if (regex_match(email, regEx)) {
        return true;
    }

    cout << "Please enter a valid email" << endl;
    return false;
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
        encryption += getRandomAlphabet();
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

string decryptPassword(string password) {
    setkeys();
    string decPass,decryption;
    for (char ch : password) {
        if (!isalpha(ch)) {
            decPass += ch;
        }
        else {
            decryption += decrypt(stoi(decPass));
            decPass.clear();
        }
    }
    return decryption;
}

long long int decrypt(int msg) {
    int d = privateKey;
    long long int decrypted = 1;
    while (d--) {
        decrypted *= msg;
        decrypted %= n;
    }
    return decrypted;
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

char getRandomAlphabet() {
    random_device rd;
    mt19937 gen(rd());  // Mersenne Twister pseudo-random generator
    uniform_int_distribution<int> dis(0, 25);  // Uniform distribution for 26 letters
    int randomIndex = dis(gen);
    char randomChar = 'A' + randomIndex;  // 'A' is 65 in ASCII, 'Z' is 90
    return randomChar;
}

//fetch the user details
void fetchUsers(sqlite3* db) {
    userDetails.clear();
    const string query = "SELECT * FROM user";
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    if (res != SQLITE_OK) {
        cout << "Failed to fetch records" << endl;
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string pass = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        userDetails.push_back({ email,pass });
    }
    sqlite3_finalize(stmt);
}
