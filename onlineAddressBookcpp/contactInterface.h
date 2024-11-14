#pragma once
#include<iostream>
using namespace std;

class contactInterface {
public:
	virtual void addContact(string email) = 0;
	virtual void search(string email) = 0;
	virtual void deleteContact(string email) = 0;
	virtual void editContact(string email) = 0;
	virtual void viewContacts(string email) = 0;
	virtual void viewByGroup(string email) = 0;
	virtual void viewGroupNone(string email) = 0;
	virtual void setContact(string name, string phoneNo, string address, vector<string> group) = 0;
};