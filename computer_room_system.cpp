#include <iostream>
using namespace std;


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
                break;
            case 2:
                break;
            case 3:
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
