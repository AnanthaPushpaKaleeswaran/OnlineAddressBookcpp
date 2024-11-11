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

	void addContact(string email) override;
	void search() override;
	void deleteContact() override;
	void editContact() override;
	void viewContacts() override;
	void viewByGroup() override;
};