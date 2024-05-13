#include "globalFile.hxx"
#include "identity.hxx"
#include "student.hxx"
#include "teacher.hxx"
#include "manager.hxx"
#include <fstream>
#include <string>
#include <iostream>
using namespace std;


void LoginIn(string fileName, int type)
{
    Identity *person = NULL;

    ifstream ifs;
    ifs.open(fileName, ios::in);

    if (!ifs.is_open())
    {
        cout << "file is not exist." << endl;
        ifs.close();
        return;
    }
    int id = 0;
    string name;
    string pwd;
    if (type == 1)
    {
        cout << "input your student id" << endl;
        cin >> id;
    }
    else if (type == 2)
    {
        cout << "input your teacher id" << endl;
        cin >> id;
    }

    cout << "input the username: " << endl;
    cin >> name;

    cout << "input the password:" << endl;
    cin >> pwd;

    if (type == 1)
    {
      int fId;
      string fName;
      string fPwd;
      while (ifs >> fId && ifs >> fName && ifs >> fPwd)
      {
        if (id == fId && name == fName && pwd == fPwd)
        {
            cout << "student login success" << endl;
            system("pause");
            system("cls");
            person = new Student(id, name, pwd);
            studentMenu(person);
            return;
        }
      }
    }
    else if (type == 2)
    {
      int fId;
      string fName;
      string fPwd;
      while (ifs >> fId && ifs >> fName && ifs >> fPwd)
      {
        if (id == fId && name == fName && pwd == fPwd)
        {
            cout << "teacher login success" << endl;
            system("pause");
            system("cls");
            person = new Teacher(id, name, pwd);
            TeacherMenu(person);
            return;
        }
      }
    }
    else if (type == 3)
    {
      string fName;
      string fPwd;
      while ( ifs >> fName && ifs >> fPwd)
      {
        if (name == fName && pwd == fPwd)
        {
            cout << "manager login success" << endl;
            system("pause");
            system("cls");
            person = new Manager(name, pwd);
            managerMenu(person);
            return;
        }
      }
    }
    cout << "auth failed!" << endl;
    system("pause");
    system("cls");
    return;
}



int main()
{
    int select = 0;
    while (true) 
    {

        cout << "welcome to the computer roon reservation system!!!" << endl;
        cout << "please enter your identity." << endl;
        cout << "\t\t ---------------------------\n";
        cout << "\t\t |                          |\n";
        cout << "\t\t |      1. 学生代表          |\n";
        cout << "\t\t |                          |\n";
        cout << "\t\t |      2. 老   师           |\n";
        cout << "\t\t |                          |\n";
        cout << "\t\t |      3. 管理员            |\n";
        cout << "\t\t |                          |\n";
        cout << "\t\t |      0. 退   出          |\n";
        cout << "\t\t ---------------------------\n";
        cout << "please enter your choice." << endl;   
        cin >> select;
        switch (select)
        {
        case 1:
            LoginIn(STUDENT_FILE, 1);
            break;
        case 2:
            LoginIn(TEACHER_FILE, 2);
            break;
        case 3:
            LoginIn(ADMIN_FILE, 3);
            break;
        case 0:
            cout << "welcome to use next time." << endl;
            system("pause");
            return 0;
            break;
        default:
            cout << "enter error, please choose again. " << endl;
            system("pause");
            system("cls");
            break;
        }
    }

    system("pause");
    return 0;
}
