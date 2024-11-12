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
	set<string> existingGroups;

	void addContact(string email) override;
	void search(string email) override;
	void deleteContact(string email) override;
	void editContact() override;
	void viewContacts(string email) override;
	void viewByGroup(string email) override;
	void setExistingGroups(string email) override;
};