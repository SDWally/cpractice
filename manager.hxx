#pragma once
#include <iostream>
using namespace std;
#include "identity.hxx"
#include "globalFile.hxx"




class Manager :public Identity
{
public:
    Manager();

    Manager(string name, string pwd);      

    virtual void operMenu();

    void addPerson();

    void showPerson();

    void showComputer();

    void cleanFile();

};

void managerMenu(Identity * &manager);