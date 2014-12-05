/* 
 * File:   Client.cpp
 * Author: fahmi
 * 
 * Created on December 4, 2014, 8:39 AM
 */

#include "Client.h"

Client::Client() {
    port=9000;
    // buka socket TCP (SOCK_STREAM) dengan alamat IPv4 dan protocol IP
    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0){
	close(sock);
	printf("Cannot open socket\n");
	exit(1);
    }
    else{
        printf("Success open socket\n");
    }
    // buat address server
    server = gethostbyname("localhost");
    if (server == NULL) {
	fprintf(stderr,"Host not found\n");
	exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);
}

Client::Client(const Client& orig) {
}

Client::~Client() {
}
void Client::ConnectToServer(){
    // connect ke server, jika error keluar
    if (connect(sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        exit(1);
    }
    else{
        cout <<"Berhasil terhubung ke surver"<<endl;
    }
    // copy n ke buffer
    bcopy("lalalalal", message, 50);
    //cin >> message;
    // tulis ke server
    len = write(sock,message,50);
    cout << "Message terkirim" << endl;
    if (len >= 0){
	// baca balasan server
	len = read(sock,message,50);
	if (len >= 0){
            printf("%s\n", message);
	}
    }
    //close(sock);
}
void Client::Disconnect(){
    close(sock);
}