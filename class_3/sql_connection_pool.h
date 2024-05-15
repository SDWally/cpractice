#pragma once

#include <mysql/mysql.h>
#include "locker.h"
using namespace std;

class connection_pool
{
public:
    MYSQL *GetConnection();
    bool ReleaseConnection(MYSQL *conn);
    int GetFreeConn();
    void DestroyPool();

    static connection_pool *GetInstance();

    void init(string url, string User, string PassWord, string DataBaseName, int Port, unsigned int MaxConn);

    connection_pool();
    ~connection_pool();

private:
    unsigned int MaxConn;
    unsigned int CurConn;
    unsigned int FreeConn;

private:
    locker lock;
    list<MYSQL *> connList;
    sem reserve;
}



