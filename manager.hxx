#pragma once
#include <iostream>
#include <vector>
using namespace std;
#include "identity.hxx"
#include "globalFile.hxx"
# include "student.hxx"
# include "teacher.hxx"






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

    void initVector();

    bool checkRepeat(int id, int type);

    vector<Student> vStu;
    vector<Teacher> vTea;
    vector<computerRoom> vCom;

};

void managerMenu(Identity * &manager);