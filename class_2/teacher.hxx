#pragma once
# define _CRT_SECURE_NO_WARNINGS
using namespace std;
#include "identity.hxx"
#include "orderFile.hxx"


class Teacher :public Identity
{
public:
    Teacher();

    Teacher(int empId, string name, string pwd);

    virtual void operMenu();

    void showAllOrder();

    void validOrder();

    int m_EmpId;

};

void TeacherMenu(Identity* &teacher);