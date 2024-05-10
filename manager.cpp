#include "manager.hxx"
#include "computerRoom.h"
#include <istream>
#include <algorithm>


Manager::Manager()
{

};

Manager::Manager(string name, string pwd)
{
    this->m_Name = name;
    this->m_Pwd = pwd;

    this->initVector();

    ifstream ifs;
    ifs.open(COMPUTER_FILE, ios::in);

    computerRoom c;
    while (ifs >> c.m_ComId && ifs >> c.m_MaxNum)
    {
        vCom.push_back(c);
    }
    cout << "now the computer room number is: " << vCom.size() << endl;

    ifs.close();
};

void Manager::operMenu()
{
    cout << "welcome  " << this->m_Name << "  login !" << endl;
    cout << "please enter your identity." << endl;
    cout << "\t\t ---------------------------\n";
    cout << "\t\t |                          |\n";
    cout << "\t\t |      1. 添加账号          |\n";
    cout << "\t\t |                         |\n";
    cout << "\t\t |      2. 查看账号          |\n";
    cout << "\t\t |                          |\n";
    cout << "\t\t |      3. 查看机房          |\n";
    cout << "\t\t |                          |\n";
    cout << "\t\t |      4. 清空预约             |\n";
    cout << "\t\t |                          |\n";
    cout << "\t\t |      0. 注销登录          |\n";
    cout << "\t\t ---------------------------\n";
    cout << "please enter your choice." << endl;   
};

void Manager::addPerson()
{
    cout << "please input the account type: " << endl;
    cout << "1. add student " << endl;
    cout << "2. add teacher " << endl;

    string fileName;
    string tip;
    ofstream ofs;

    int select = 0;
    cin >> select;

    string errorTip;

    if (select == 1)
    {
        fileName = STUDENT_FILE;
        tip = "please input the student id: ";
        errorTip = "student id dupliacate, please re input: ";
    }
    else 
    {
        fileName = TEACHER_FILE;
        tip = "please input the teacher id: ";
        errorTip = "teacher id dupliacate, please re input: ";
    }

    ofs.open(fileName, ios::out|ios::app);
    int id;
    string name;
    string pwd;
    cout << tip << endl;

    while (true)
    {
        cin >> id;
        bool ret = this->checkRepeat(id, select);
        if (ret)
        {
            cout << errorTip << endl;
        }
        else
        {
            break;
        }

    }

    cout << "please input the name: " << endl;
    cin >> name;

    cout << "please input the pwd: " << endl;
    cin >> pwd;
    
    ofs << id << " " << name << " " << pwd << endl;
    cout << "add successfully." << endl;

    system("pause");
    system("cls");
    
    ofs.close();
    this->initVector();

};


void printStudent(Student &s)
{
    cout << "student id: " << s.m_Id << "name: " << s.m_Name << "pwd: " << s.m_Pwd << endl;
};

void printTeacher(Teacher &t)
{
    cout << "teacher id: " << t.m_EmpId << "name: " << t.m_Name << "pwd: " << t.m_Pwd << endl;
};


void Manager::showPerson()
{
    cout << "please select the show content: " << endl;
    cout << "1. student " << endl;
    cout << "2. teacher " << endl;

    int select = 0;

    cin >> select;

    if (select == 1)
    {
        cout << "all student info: " << endl;
        for_each(vStu.begin(), vStu.end(), printStudent);
    }
    else
    {
        cout << "all teacher info: " << endl;
        for_each(vTea.begin(), vTea.end(), printTeacher);
    }

};

void Manager::showComputer()
{

};

void Manager::cleanFile()
{
    ofstream ofs(ORDER_FILE, ios::trunc);
    ofs.close();

    cout << "clean successfully." << endl;
    system("pause");
    system("cls");
};

void Manager::initVector()
{
    ifstream ifs;
    ifs.open(STUDENT_FILE, ios::in);
    if (!ifs.is_open())
    {
        cout << "file read failed" << endl;
        return;
    }

    vStu.clear();
    vTea.clear();

    Student s;
    while (ifs >> s.m_Id && ifs >> s.m_Name && ifs >> s.m_Pwd)
    {
        vStu.push_back(s);
    }
    cout << "now the student number is: " << vStu.size() << endl;
    ifs.close();

    ifs.open(TEACHER_FILE, ios::in);

    Teacher t;
    while (ifs >> t.m_EmpId && ifs >> t.m_Name && ifs >> t.m_Pwd)
    {
        vTea.push_back(t);
    }
    cout << "now the teacher number is: " << vTea.size() << endl;

    ifs.close();

};


bool Manager::checkRepeat(int id, int type)
{
    if (type == 1)
    {
        for (vector<Student>::iterator it = vStu.begin(); it != vStu.end(); it ++ )
        {
            if (id == it->m_Id)
            {
                return true;
            }
        }
    }
    else
    {
        for (vector<Teacher>::iterator it = vTea.begin(); it != vTea.end(); it ++ )
        {
            if (id == it->m_EmpId)
            {
                return true;
            }
        }
    }
    return false;
};


void managerMenu(Identity * &manager)
{
    while (true)
    {
        manager->operMenu();
        Manager* man = (Manager*)manager;
        int select = 0;
        cin >> select;
        if (select == 1)
        {
            cout << "add account" << endl;
            man->addPerson();
        }
        else if (select == 2)
        {
            cout << "view account" << endl;
            man->showPerson();

        }
        else if (select == 3)
        {
            cout << "view computer" << endl;
            man->showComputer();
            
        }
        else if (select == 4)
        {
            cout << "clean book" << endl;
            man->cleanFile();
        }
        else 
        {
            delete manager;
            cout << "log out" << endl;
            system("pause");
            system("cls");
            return;
        }
    }
};