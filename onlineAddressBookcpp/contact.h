#pragma once
#pragma once
#include<iostream>
#include<string>
using namespace std;
class contact {
public:
	string name;
	string phoneNo;
	string address;
	void addContact();
	void search();
	void deleteContact();
	void editContact();
	void viewContacts();
};