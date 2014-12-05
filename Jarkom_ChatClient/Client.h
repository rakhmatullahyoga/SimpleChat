/* 
 * File:   Client.h
 * Author: fahmi
 *
 * Created on December 4, 2014, 8:39 AM
 */

#ifndef CLIENT_H
#define	CLIENT_H

using namespace std;
#include <iostream>
#include <cstdio>
#include <sys/types.h>   // tipe data penting untuk sys/socket.h dan netinet/in.h
#include <netinet/in.h>  // fungsi dan struct internet address
#include <sys/socket.h>  // fungsi dan struct socket API
#include <netdb.h>       // lookup domain/DNS hostname
#include <unistd.h>
#include <cstdlib>
#include <errno.h>
#include <cstring>

class Client {
public:
    Client();
    Client(const Client& orig);
    virtual ~Client();
    void ConnectToServer();
    void Disconnect();
private:
    int sock, port, len;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char message[50];
};

#endif	/* CLIENT_H */

