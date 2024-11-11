#pragma once
#include<iostream>
using namespace std;

class contactInterface {
public:
	virtual void addContact() = 0;
	virtual void search() = 0;
	virtual void deleteContact() = 0;
	virtual void editContact() = 0;
	virtual void viewContacts() = 0;
	virtual void viewByGroup() = 0;
};