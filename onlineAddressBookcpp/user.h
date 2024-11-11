#pragma once
#include<iostream>
#include<string>
#include "userInterface.h"
using namespace std;

class user : public userInterface {
public:
	string email;
	string password;

	bool addUser();
	bool validateUser();
};