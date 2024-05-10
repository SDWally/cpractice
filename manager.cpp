#include "manager.hxx"


Manager::Manager()
{

};

Manager::Manager(string name, string pwd)
{
    this->m_Name = name;
    this->m_Pwd = pwd;
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
    if (select == 1)
    {
        fileName = STUDENT_FILE;
        tip = "please input the student id: ";
    }
    else 
    {
        fileName = TEACHER_FILE;
        tip = "please input the teacher id: ";
    }

    ofs.open(fileName, ios::out|ios::app);
    int id;
    string name;
    string pwd;
    cout << tip << endl;
    cin >> id;

    cout << "please input the name: " << endl;
    cin >> name;

    cout << "please input the pwd: " << endl;
    cin >> pwd;
    
    ofs << id << " " << name << " " << pwd << endl;
    cout << "add successfully." << endl;

    system("pause");
    system("cls");
    return;

};

void Manager::showPerson()
{

};

void Manager::showComputer()
{

};

void Manager::cleanFile()
{

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