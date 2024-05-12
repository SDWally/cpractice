#include "student.hxx"
#include "globalFile.hxx"
#include <istream>
#include <ostream>


Student::Student()
{

};

Student::Student(int id, string name, string pwd)
{
    this->m_Id = id;
    this->m_Name = name;
    this->m_Pwd = pwd;

    ifstream ifs;
    ifs.open(COMPUTER_FILE, ios::in);

    computerRoom c;

    while (ifs >> c.m_ComId && ifs >> c.m_MaxNum)
    {
        vCom.push_back(c);
    }

    ifs.close();
    
};
void Student::operMenu()
{
    cout << "welcome student: " << this->m_Name << "  login !" << endl;
    cout << "\t\t ---------------------------\n";
    cout << "\t\t |                          |\n";
    cout << "\t\t |      1. 申请预约          |\n";
    cout << "\t\t |                         |\n";
    cout << "\t\t |      2. 查看我的预约          |\n";
    cout << "\t\t |                          |\n";
    cout << "\t\t |      3. 查看所有预约          |\n";
    cout << "\t\t |                          |\n";
    cout << "\t\t |      4. 取消预约             |\n";
    cout << "\t\t |                          |\n";
    cout << "\t\t |      0. 注销登录          |\n";
    cout << "\t\t ---------------------------\n";
    cout << "please enter your choice." << endl;   
};

void Student::applyOrder()
{
    cout << "机房开放时间为周一到周五！" << endl;
    cout << "请输入申请预约的时间： " << endl;
    cout << "1 周一" << endl;
    cout << "2 周二" << endl;
    cout << "3 周三" << endl;
    cout << "4 周四" << endl;
    cout << "5 周五" << endl;

    int date = 0;
    int interval = 0;
    int room = 0;

    while (true)
    {
        cin >> date;
        if (date >= 1 && date <= 5)
        {
            break;
        }
        cout << "输入有误，请重新输入" << endl;
    }

    cout << "请重新输入申请预约的时间段： " << endl;
    cout << "1 上午" << endl;
    cout << "2 下午" << endl;

    while (true)
    {
        cin >> interval;
        if (interval >= 1 && interval <= 2)
        {
            break;
        }
        cout << "输入有误，请重新输入" << endl;
    }

    cout << "请重新输入申请预约的时间段： " << endl;
    cout << "1号机房容量：" << vCom[0].m_MaxNum << endl;
    cout << "2号机房容量：" << vCom[1].m_MaxNum << endl;
    cout << "3号机房容量：" << vCom[2].m_MaxNum << endl;

    while (true)
    {
        cin >> room;
        if (room >= 1 && room <= 3)
        {
            break;
        }
        cout << "输入有误，请重新输入" << endl;
    }

    cout << "预约成功！审核中" << endl;

    ofstream ofs(ORDER_FILE, ios::app);
    ofs << "date: " << date << " ";
    ofs << "interval: " << interval << " ";
    ofs << "stuId: " << this->m_Id << " ";
    ofs << "stuName: " << this->m_Name << " ";
    ofs << "roomId: " << room << " ";
    ofs << "status: " << 1 << endl;
    
    ofs.close();
    system("pause");
    system("cls");

};


void Student::showMyOrder()
{
    
};
void Student::showAllOrder()
{
    
};

void Student::cancelOrder()
{
    
};


void studentMenu(Identity * &student)
{
    while (true)
    {
        student->operMenu();
        Student* stu = (Student*)student;
        int select = 0;
        
        cin >> select;

        if (select == 1)
        {
            stu->applyOrder();
        }
        else if (select == 2)
        {
            stu->showMyOrder();
        }
        else if (select == 3)
        {
            stu->showAllOrder();
        }
        else if (select == 4)
        {
            stu->cancelOrder();
        }
        else
        {
           delete student;
           cout << "logout sucess" << endl;
           system("pause");
           system("cls");
           return;
        }
    }
}