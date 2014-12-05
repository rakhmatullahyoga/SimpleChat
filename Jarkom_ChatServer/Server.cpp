/* 
 * File:   Server.cpp
 * Author: fahmi
 * 
 * Created on December 4, 2014, 8:39 AM
 */

#include "Server.h"

Server::Server() {
    port = 9000;
    yes = 1;
    // buka socket TCP (SOCK_STREAM) dengan alamat IPv4 dan protocol IP
    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0){
        close(sock);
        printf("Cannot open socket\n");
        exit(1);
    }
    else{
        printf("Success open socket\n");
    }
    // ganti opsi socket agar dapat di-reuse addressnya
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
	perror("Cannot set option\n");
	exit(1);
    
    }
    else{
        printf("Success set option\n");
    }
    // buat address untuk server
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; // semua address pada network interface
    serv_addr.sin_port = htons(9000); // port 9000
    
    // bind server ke address
    if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
	close(sock);
	printf("Cannot bind socket\n");
	exit(1);
    }
    else{
        printf("Success bind socket\n");
    }
    listen(sock,5); // listen ke maksimal 5 koneksi klien
}

Server::Server(const Server& orig) {
}

Server::~Server() {
}

void Server::OpenConnection(){
    while(1){
	// terima koneksi dari klien
	clilen = sizeof(cli_addr);
	client_sock = accept(sock, (struct sockaddr *) &cli_addr, &clilen);
        
	// kosongkan buffer
	bzero(message,50);
		
	// baca dari klien
	len = read(client_sock, message,50);
		
	if (len >= 0){ // jika pembacaan berhasil
            printf("Recv : %s\n",message);
								
            write(client_sock,message,50); // tulis ke klien
	}
        // tutup koneksi klien
        close(client_sock);
    }
}
void Server::CloseConnection(){
    close(sock);
}