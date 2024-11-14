// OnlineAddressBook.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include<vector>
#include "user.h"
#include "contact.h"
#include "sqlite3.h"
using namespace std;

class onlineAddressBook : public user, public contact {
public:

	//set the user
	void setUser(string email, string password) {
		this->email = email;
		this->password = password;
	}

	//get the user details
	void getUser() {
		while (1) { // Main loop for login, signup, or exit

			cout << "1.Login" << endl << "2.Signup" << endl << "3.Exit" << endl;
			struct stat buffer;

			if (stat("onlineAddressBook.db", &buffer) != 0) {
				cout << "There is no database here." << endl << "Signup to continue" << endl;
			}

			cout << "Enter your choice : ";
			int type;
			string email, password;
			bool ok = false;
			cin >> type;

			switch (type) {
			case 1: // Login option
				getUserInput(&email, &password);
				setUser(email, password);
				ok = validateUser();
				break;

			case 2: // Signup option
				getUserInput(&email, &password);
				setUser(email, password);
				ok = addUser();
				break;

			case 3: // Exit the program
				cout << "Thank you :)" << endl;
				return;

			default:
				cout << "Invalid option." << endl;
				cout << "-----------------------------------------------------------------------------------------------------------------------!" << endl << endl;
				continue;
			}
			cout << "-----------------------------------------------------------------------------------------------------------------------!" << endl << endl;

			if (ok) {
				getContact(); // Proceed to contact management if login/signup is successful
			}
		}
	}


	//get the contact details
	void getContact() {
		while (1) {
			// Display menu options
			cout << "1.Add Contact" << endl << "2.Search Contact" << endl << "3.Delete Contact" << endl;
			cout << "4.Edit Contact" << endl << "5.See the contacts" << endl << "6.See the contacts by group" << endl;
			cout << "7.See the contacts those who are not in group" << endl << "8.Log out" << endl;
			cout << endl << "Enter your choice : ";

			int choice;
			cin >> choice;

			// Initialize variables for contact details
			bool ok = false;
			string name;
			string phoneNo;
			string address;
			vector<string> group;

			// Handle user choice
			switch (choice) {
			case 1: // Add a new contact
				addContact(email);
				break;

			case 2: // Search for an existing contact
				search(email);
				break;

			case 3: // Delete a contact
				deleteContact(email);
				break;

			case 4: // Edit an existing contact
				editContact(email);
				break;

			case 5: // View all contacts
				viewContacts(email);
				break;

			case 6: // View contacts by group
				viewByGroup(email);
				break;

			case 7: // View contacts without a group
				viewGroupNone(email);
				break;

			case 8: // Log out and return to the main menu
				cout << "Logging out ..." << endl;
				cout << "-----------------------------------------------------------------------------------------------------------------------!" << endl << endl;
				return;

			default: // Handle invalid choices
				cout << "Invalid option." << endl;
				cout << "-----------------------------------------------------------------------------------------------------------------------!" << endl << endl;
				continue;
			}
			cout << "-----------------------------------------------------------------------------------------------------------------------!" << endl << endl;
		}
	}


	//getting the input for user
	virtual void getUserInput(string* email, string* password) final{
		cout << "Enter your email : ";
		cin >> *email;

		cout << "Enter your password : ";
		cin >> *password;
	}
};

int main()
{
	onlineAddressBook bookObj;
	bookObj.getUser();
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file