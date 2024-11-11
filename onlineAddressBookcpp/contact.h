#pragma once
#include<iostream>
#include<string>
#include "contactInterface.h"
using namespace std;
class contact : public contactInterface {
protected:
	string name;
	string phoneNo;
	string address;
	string group;

	void addContact();
	void search();
	void deleteContact();
	void editContact();
	void viewContacts();
	void viewByGroup();
};