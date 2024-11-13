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

	//set the contact
	void setContact(string name, string phoneNo, string address, vector<string> group) {
		this->name = name;
		this->phoneNo = phoneNo;
		this->address = address;
		this->group = group;
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
				cout << "-----------------------------------------------------------------------------------------------!" << endl << endl;
				continue;
			}
			cout << "-----------------------------------------------------------------------------------------------!" << endl << endl;

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
				getContactInput(&name, &phoneNo, &address, &group);
				setContact(name, phoneNo, address, group);
				addContact(email);
				break;

			case 2:
				search(email);
				break;

			case 3:
				//deleteContact(email);
				break;

			case 4:
				editContact();
				break;

			case 5:
				viewContacts(email);
				break;

			case 6:
				viewByGroup(email);
				break;

			case 7:
				cout << "Logging out ..."<<endl;
				cout << "-----------------------------------------------------------------------------------------------!" << endl << endl;
				return;

			default:
				cout << "Invalid option." << endl;
				cout << "-----------------------------------------------------------------------------------------------!" << endl << endl;
				continue;
			}
			cout << "-----------------------------------------------------------------------------------------------!" << endl << endl;
		}
	}

	//getting the input for user
	void getUserInput(string* email, string* password) {
		cout << "Enter your email : ";
		cin >> *email;

		cout << "Enter your password : ";
		cin >> *password;
	}

	//getting the input for contact
	void getContactInput(string* name, string* phoneNo, string* address, vector<string>* group) {
		cin.ignore();
		cout << "Enter the name : ";
		getline(cin, *name);

		cout << "Enter the phone number : ";
		cin >> *phoneNo;

		cin.ignore();
		cout << "Enter the address : ";
		getline(cin, *address);

		while (1) {
			bool ok = false;
			setExistingGroups(email);
			int count = 1;
			cout << endl<<"Select a group"<<endl;
			
			for (auto itr : existingGroups) {
				itr[0] = toupper(itr[0]);
				cout << count++ << "." << itr << endl;
			}

			cout << count++ << ".New Group" << endl;
			cout << count << ".Exit" << endl;
			int groupChoice;
			cout << "Enter your choice : ";
			cin >> groupChoice;
			string groupName;
			auto it = existingGroups.begin();

			if (groupChoice>=1 && groupChoice < count - 1) {
				advance(it, groupChoice-1);
				groupName = *it;
				
				if (individualGroupExist(*group,groupName)) {
					cout << "The group was already added." << endl;
				}
				else {
					group->push_back(groupName);
					cout << "Group added was successfully!" << endl;
				}
			}
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
			else if (groupChoice == count) {
				cout << "Exiting group selection" << endl;
				return;
			}
			else{
				cout << "Invalid choice."<<endl<<"Please try again later" << endl;
			}
		}
	}

	//get the new group
	void getNewGroup(string* group) {
		cout << "Enter the group name : ";
		cin >> *group;
	}

	//check whether the group already exist in exisiting groups or not
	bool groupExist(string group) {
		int groupLen = group.length();
		for (auto& itr : existingGroups) {
			bool individual = true;

			if (itr.length() != groupLen) {
				individual = false;
				continue;
			}

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
		return false;
	}

	//check the group was added to the contact or not
	bool individualGroupExist(vector<string> group,string groupName) {
		int groupLen = groupName.length();
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
		return false;
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