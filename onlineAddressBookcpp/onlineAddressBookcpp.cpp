//

#include <iostream>
#include "user.h"
#include "contact.h"
using namespace std;
void getInput(string* email, string* password);
void getContact(string* name, string* phoneNo, string* address);
class onlineAddressBook : public user, public contact {
public:
	void setUser(string email, string password) {
		this->email = email;
		this->password = password;
	}
	void setContact(string name, string phoneNo, string address) {
		this->name = name;
		this->phoneNo = phoneNo;
		this->address = address;
	}
};

int main()
{
	onlineAddressBook bookObj;
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
		switch (choice) {

		case 1:
			getContact(&name, &phoneNo, &address);
			bookObj.setContact(name, phoneNo, address);
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
void getInput(string* email, string* password) {
	cout << "Enter your name : ";
	cin >> *email;
	cout << "Enter your password : ";
	cin >> *password;
}
void getContact(string* name, string* phoneNo, string* address) {
	cout << "Enter the name : ";
	cin >> *name;
	cout << "Enter the phone number : ";
	cin >> *phoneNo;
	cout << "Enter the address : ";
	cin >> *address;
}