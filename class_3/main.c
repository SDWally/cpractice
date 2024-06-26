#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>

#include "locker.h"
#include "threadpool.h"
#include "lst_timer.h"
#include "http_conn.h"
#include "log.h"
#include "sql_connection_pool.h"

#define MAX_FD 65536
#define MAX_EVENT_NUMBER 10000
#define TIMESLOT 5

#define SYNLOG

#define listenfdLT

extern int addfd(int epollfd, int fd, bool one_shot);
extern int remove(int epollfd, int fd);
extern int setnonblocking(int fd);

static int pipefd[2];
static sort_timer_lst timer_lst;
static int epollfd = 0;


void sig_handler(int sig)
{
    int save_errno = errno;
    int msg = sig;
    send(pipefd[1], (char *) &msg, 1, 0);
    errno = save_errno;
}

void addsig(int sig, void(handler)(int), bool restart = true)
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = handler;
    if (restart)
    {
        sa.sa_flags |= SA_RESTART;
    }

    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}

void timer_handler()
{
    timer_lst.tick();
    alarm(TIMESLOT);
}


void cb_func(client_data *user_data)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, user_data->sockfd, 0);
    assert(user_data);
    close(user_data->sockfd);
    http_conn::m_user_count--;
    LOG_INFO("close fd %d", user_data->sockfd);
    Log::get_instance()->flush();
}

void show_error(int connfd, const char *info)
{
    printf("%s", info);
    send(connfd, info, strlen(info), 0);
    close(connfd);
}

int main(int argc, char *argv[])
{
    cout << "start main func..." << endl;
#ifdef ASYNLOG
    Log::get_instance()->init("ServerLog", 2000, 800000, 8);
#endif
#ifdef SYNLOG
    Log::get_instance()->init("ServerLog", 2000, 800000, 0);
#endif
    cout << "start arg check..." << endl;
    if (argc <= 1)
    {
        printf("usage: %s ip_address port number\n"), basename(argv[0]);
        return 1;
    }
    LOG_INFO("%s", "server start");
    int port = atol(argv[1]);
    // int port = 9007;
    addsig(SIGPIPE, SIG_IGN);

    connection_pool *connPool = connection_pool::GetInstance();
    connPool->init("139.9.175.149", "tiny", "tiny", "tinyweb", 3306, 10);
    
    cout << "init mysql pool..." << endl;
    threadpool<http_conn> *pool = NULL;
    try
    {
        pool = new threadpool<http_conn>(connPool);
    }
    catch (...)
    {
        return 1;
    }

    http_conn *users = new http_conn[MAX_FD];
    assert(users);
    
    cout << "create mysqlresult ..." << endl;
    users->initmysql_result(connPool);
    cout << "init result start..." << endl;
    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    cout << "socket start..." << endl;
    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    int flag = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    ret = bind(listenfd, (struct sockaddr *)&address, sizeof(address));
    assert(ret >= 0);
    ret = listen(listenfd, 5);
    assert(ret >= 0);
    
    cout << "listen start..." << endl;
    epoll_event events[MAX_EVENT_NUMBER];
    epollfd = epoll_create(5);
    assert(epollfd != -1);

    addfd(epollfd, listenfd, false);
    http_conn::m_epollfd = epollfd;

    ret = socketpair(PF_UNIX, SOCK_STREAM, 000, pipefd);
    assert(ret != -1);
    setnonblocking(pipefd[1]);
    addfd(epollfd, pipefd[0], false);

    addsig(SIGALRM, sig_handler, false);
    addsig(SIGTERM, sig_handler, false);
    bool stop_server = false;

    client_data *users_timer = new client_data[MAX_FD];

    bool timeout = false;
    alarm(TIMESLOT);

    cout << "server start, wait connect..." << endl;
    while (!stop_server)
    {
        cout << "server enter epoll wait ..." << endl;
        int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        cout << "server enter number check ..." << endl;
        cout << number << endl;
        if (number < 0 && errno != EINTR)
        {
            LOG_ERROR("%s", "epoll failure");
            break;
        }
        cout << "server get connect..." << endl;
        

        for (int i = 0; i < number; i++)
        {
            int sockfd = events[i].data.fd;
            if (sockfd == listenfd)
            {
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof(client_address);
#ifdef listenfdLT
                int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addrlength);

                cout << "server accept..." << endl;

                if (connfd < 0)
                {
                    cout << "server accept  connfd ..." << endl;
                    LOG_ERROR("%s:errno is: %d", "accept error", errno);
                    continue;
                }
                if (http_conn::m_user_count >= MAX_FD)
                {
                    cout << "server accept http conn ..." << endl;
                    show_error(connfd, "Internal server busy");
                    LOG_ERROR("%s", "Internal server busy");
                    continue;
                }               
                cout << "server accept  1 ..." << endl;
                users[connfd].init(connfd, client_address);
                users_timer[connfd].address = client_address;
                users_timer[connfd].sockfd = connfd;
                cout << "server accept  2 ..." << endl;
                util_timer *timer = new util_timer;
                timer->user_data = &users_timer[connfd];
                timer->cb_func = cb_func;
                time_t cur = time(NULL);
                cout << "server accept  3 ..." << endl;
                timer->expire = cur + 3 * TIMESLOT;
                users_timer[connfd].timer = timer;
                timer_lst.add_timer(timer);
                cout << "server accept  4 ..." << endl;
#endif

#ifdef listenfdET

                while(1)
                {
                    int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addrlength);
                    if (connfd < 0)
                    {
                        LOG_ERROR("%s:errno is: %d", "accept error", errno);
                        break;
                    }
                    if (http_conn::m_user_count >= MAX_FD)
                    {
                        show_error(connfd, "Interval server busy");
                        LOG_ERROR("%s", "Interval server busy");
                        break;
                    }
                    users[connfd].init(connfd, client_address);

                    users_timer[connfd].address = client_address;
                    users_timer[connfd].sockfd = connfd;
                    util_timer *timer = new util_timer;
                    timer->user_data = &user_timer[connfd];
                    time->cb_func = cb_func;
                    time_t cur = time(NULL);
                    timer->expire = cur + 3 * TIMESLOT;
                    users_timer[connfd].timer = timer;
                    timer_lst.add_timer[timer];
                }
                continue;
#endif
            cout << "server finish if else ..." << endl;
            }
            else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
                cout << "server enter epoll ..." << endl;
                util_timer *timer = users_timer[sockfd].timer;
                timer->cb_func(&users_timer[sockfd]);

                if (timer)
                {
                    timer_lst.del_timer(timer);
                }
            }
            else if ((sockfd == pipefd[0]) && (events[i].events & EPOLLIN))
            {
                cout << "server enter pipefd ..." << endl;
                int sig;
                char signals[1024];
                ret = recv(pipefd[0], signals, sizeof(signals), 0);
                if (ret == -1)
                {
                    continue;
                }
                else if (ret == 0)
                {
                    continue;
                }
                else
                {
                    for (int i = 0; i < ret; ++i)
                    {
                        switch (signals[i])
                        {
                        case SIGALRM:
                        {
                            timeout = true;
                            break;
                        }
                        case SIGTERM:
                        {
                            stop_server = true;
                        }
                        }
                    }
                }
            }
            else if (events[i].events & EPOLLIN)
            {
                cout << "server enter event poll ..." << endl;
                util_timer *timer = users_timer[sockfd].timer;
                if (users[sockfd].read_once())
                {
                    cout << "server enter read once ..." << endl;
                    LOG_INFO("deal with the client(%s)", inet_ntoa(users[sockfd].get_address()->sin_addr));
                    Log::get_instance()->flush();
                    pool->append(users + sockfd);
                    if (timer)
                    {
                        cout << "server enter once timer ..." << endl;
                        time_t cur = time(NULL);
                        timer->expire = cur + 3 * TIMESLOT;
                        cout << "server enter log ..." << endl;
                        LOG_INFO("%s", "adjust timer once");
                        Log::get_instance()->flush();
                        cout << "server enter adjust timer ..." << endl;
                        timer_lst.adjust_timer(timer);
                    }
                    cout << "server finish adjust timer ..." << endl;
                }
                else
                {
                    cout << "server enter timer ..." << endl;
                    timer->cb_func(&users_timer[sockfd]);
                    if (timer)
                    {
                        timer_lst.del_timer(timer);
                    }
                }
            }
            else if (events[i].events & EPOLLOUT)
            {
                cout << "server enter epollout ..." << endl;
                util_timer *timer = users_timer[sockfd].timer;
                if (users[sockfd].write())
                {
                    LOG_INFO("send data to the client(%s)", inet_ntoa(users[sockfd].get_address()->sin_addr));
                    Log::get_instance()->flush();

                    if (timer)
                    {
                        time_t cur = time(NULL);
                        timer->expire = cur + 3 * TIMESLOT;
                        LOG_INFO("%s", "adjust timer once");
                        Log::get_instance()->flush();
                        timer_lst.adjust_timer(timer);
                    }
                }
                else
                {
                    timer->cb_func(&users_timer[sockfd]);
                    if (timer)
                    {
                        timer_lst.del_timer(timer);
                    }
                }
            }
        }
        if (timeout)
        {
            cout << "enter timeout ..." << endl;
            timer_handler();
            timeout = false;
        }
        cout << "finish connect ..." << endl;
    }
    cout << "server close epollfd ..." << endl;
    close(epollfd);
    close(listenfd);
    cout << "server close epollfd 1 ..." << endl;
    close(pipefd[1]);
    cout << "server close epollfd 2 ..." << endl;
    close(pipefd[0]);
    cout << "server close epollfd 3 ..." << endl;
    delete[] users;
    delete[] users_timer;
    cout << "server close epollfd 4 ..." << endl;
    delete pool;
    return 0;
}
