// OnlineAddressBook.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include<vector>
#include "user.h"
#include "contact.h"
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
		while (1) {
			cout << "1.Login" << endl << "2.Signup" << endl << "3.Exit" << endl;
			cout << "Enter your choice : ";
			int type;
			string email;
			string password;
			bool ok = false;
			cin >> type;

			switch (type) {

			case 1:
				getUserInput(&email, &password);
				setUser(email, password);
				ok = validateUser();
				break;

			case 2:
				getUserInput(&email, &password);
				setUser(email, password);
				ok = addUser();
				break;

			case 3:
				cout << "Thank you :)"<<endl;
				return;

			default:
				cout << "Invalid option."<<endl;
				cout << "-----------------------------------------------------------------------------------------------------------------------!" << endl << endl;
				continue;
			}
			cout << "-----------------------------------------------------------------------------------------------------------------------!" << endl << endl;

			if (ok) {
				getContact();
			}
		}
	}

	//get the contact details
	void getContact() {
		while (1) {
			cout << "1.Add Contact" << endl << "2.Search Contact" << endl << "3.Delete Contact" << endl;
			cout << "4.Edit Contact" << endl << "5.See the contacts" << endl << "6.See the contacts by group" << endl << "7.Logout";
			cout << endl << "Enter your choice : ";
			int choice;
			cin >> choice;
			bool ok = false;
			string name;
			string phoneNo;
			string address;
			vector<string> group;

			switch (choice) {

			case 1:
				addContact(email);
				break;

			case 2:
				search(email);
				break;

			case 3:
				deleteContact(email);
				break;

			case 4:
				editContact(email);
				break;

			case 5:
				viewContacts(email);
				break;

			case 6:
				viewByGroup(email);
				break;

			case 7:
				cout << "Logging out ..."<<endl;
				cout << "-----------------------------------------------------------------------------------------------------------------------!" << endl << endl;
				return;

			default:
				cout << "Invalid option." << endl;
				cout << "-----------------------------------------------------------------------------------------------------------------------!" << endl << endl;
				continue;
			}
			cout << "-----------------------------------------------------------------------------------------------------------------------!" << endl << endl;
		}
	}

	//getting the input for user
	void getUserInput(string* email, string* password) {
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