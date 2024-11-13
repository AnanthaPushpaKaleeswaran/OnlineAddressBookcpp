#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<set>
#include "contactInterface.h"
using namespace std;
class contact : public contactInterface {
protected:
	string name;
	string phoneNo;
	string address;
	vector<string> group;

	void addContact(string email) override;
	void search(string email) override;
	void deleteContact(string email) override;
	void editContact(string email) override;
	void viewContacts(string email) override;
	void viewByGroup(string email) override;
	void setContact(string name, string phoneNo, string address, vector<string> group) override;
};