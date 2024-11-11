// OnlineAddressBook.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "user.h"
#include "contact.h"
using namespace std;

void getInput(string* email, string* password);
void getContact(string* name, string* phoneNo, string* address, string* group);

class onlineAddressBook : public user, public contact {
public:
	//set the user
	void setUser(string email, string password) {
		this->email = email;
		this->password = password;
	}

	//set the contact
	void setContact(string name, string phoneNo, string address, string group) {
		this->name = name;
		this->phoneNo = phoneNo;
		this->address = address;
		this->group = group;
	}
};

int main()
{
	onlineAddressBook bookObj;
	while (1) {
		cout << "1.Login" << endl << "2.Signup" << endl << "3.Exit" << endl;
		cout << "Enter your choice : ";
		//get the type
		int type;
		string email;
		string password;
		bool ok = false;
		cin >> type;
		switch (type) {

		case 1:
			getInput(&email, &password);
			bookObj.setUser(email, password);
			ok = bookObj.validateUser();
			break;

		case 2:
			getInput(&email, &password);
			bookObj.setUser(email, password);
			ok = bookObj.addUser();
			break;

		default:
			cout << "Thank you :)";
			return 0;
		}
		cout << "-------------------------------------------------------------------!" << endl << endl;

		if (ok) {
			break;
		}
	}

	while (1) {
		cout << "1.Add Contact" << endl << "2.Search Contact" << endl << "3.Delete Contact" << endl;
		cout << "4.Edit Contact" << endl << "5.See the contacts" << endl << "6.Exit" << endl;
		cout << "Enter your choice : ";
		int choice;
		cin >> choice;
		bool ok = false;
		string name;
		string phoneNo;
		string address;
		string group;
		switch (choice) {

		case 1:
			getContact(&name, &phoneNo, &address, &group);
			bookObj.setContact(name, phoneNo, address, group);
			bookObj.addContact();
			break;

		case 2:
			bookObj.search();
			break;

		case 3:
			bookObj.deleteContact();
			break;

		case 4:
			bookObj.editContact();
			break;

		case 5:
			bookObj.viewContacts();
			break;

		default:
			cout << "Thank you :)";
			return 0;
		}
		cout << "-------------------------------------------------------------------!" << endl << endl;
	}
	return 0;
}

//getting the input for user
void getInput(string* email, string* password) {
	cout << "Enter your email : ";
	cin >> *email;

	cout << "Enter your password : ";
	cin >> *password;
}

//getting the input for contact
void getContact(string* name, string* phoneNo, string* address, string* group) {
	cout << "Enter the name : ";
	cin >> *name;

	cout << "Enter the phone number : ";
	cin >> *phoneNo;

	cout << "Enter the address : ";
	cin >> *address;

	cout << "Enter the group : ";
	cin >> *group;
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