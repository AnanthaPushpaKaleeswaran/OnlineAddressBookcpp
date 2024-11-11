#pragma once
#include<iostream>
#include<string>
#include "userInterface.h"
using namespace std;

class user : public userInterface {
protected:
	string email;
	string password;

	bool addUser() override;
	bool validateUser() override;
};