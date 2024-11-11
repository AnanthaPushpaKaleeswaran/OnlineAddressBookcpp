#pragma once
#include<iostream>
using namespace std;
class userInterface {
public:
	virtual bool addUser() = 0;
	virtual bool validateUser() = 0;
	virtual string encryption() = 0;
};