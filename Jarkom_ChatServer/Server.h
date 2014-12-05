/* 
 * File:   Server.h
 * Author: fahmi
 *
 * Created on December 4, 2014, 8:39 AM
 */

#ifndef SERVER_H
#define	SERVER_H

using namespace std;
#include <iostream>
#include <cstdio>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <cstdlib>

class Server {
public:
    Server();
    Server(const Server& orig);
    virtual ~Server();
    void OpenConnection();
    void CloseConnection();
private:
    int sock, client_sock;
    char message[50];
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int port, len;
    int yes;
    string keluar;
};

#endif	/* SERVER_H */

