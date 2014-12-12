#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

#include <signal.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#include <unistd.h>

#include <ctime>
#include <vector>

#include "user.h"
#include "message.h"

using namespace std;

#define MAXTHREADS 5

#define KILO 1024
#define BUFF_LENGTH 1000
#define PROTO_PORT 60000
#define QLEN 1
#define MAX_CONTACTS 5


int contacts = 0;
pthread_t tid[MAXTHREADS];	
int active_socket[MAXTHREADS];
int thread_retval = 0;
int sd;
int endloop;
char* serverlog;

User clients[MAX_CONTACTS];
Message m;

typedef struct contact{
	char contactname[256];
	int contactsd;
}contact;



void WriteLog(string activity){
    //Get current time
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    char output[30];
    strftime(output, 30, "[%Y-%m-%d %H:%M]", timeinfo);
    
    ofstream logserver;
    logserver.open ("logserver.txt",ios::app);
    logserver << output << " " << activity <<endl;
    logserver.close();
}

void chat(int sd2){
	time_t rawtime;
    struct tm * timeinfo;

	int n,i, served = 0;	
	
	char clientname[BUFF_LENGTH];
	char *var;
	char message[BUFF_LENGTH];
	char inbuf[BUFF_LENGTH];
	char outbuf[BUFF_LENGTH];

	ofstream serverFile;
	
	while(served == 0){

		for(i = 0; i < BUFF_LENGTH; i++){
			inbuf[i] = 0;
			outbuf[i] = 0;
		}

		
		
		n = read(sd2, inbuf, sizeof(inbuf));
		
		if(!strcmp(inbuf, "logout")) {
			sprintf(outbuf, "logout");
			write(sd2, outbuf, sizeof(outbuf));
			for(i = 0; clients[i].GetSocket() != sd2; i++);
			char* name = new char[clients[i].GetName().size()+1];
			strcpy(name, clients[i].GetName().c_str());
			serverlog = new char[50];
			sprintf(serverlog, "%s logged out", name);
			WriteLog(serverlog);
			served = 1;
		}
		else {
			if (strncmp(inbuf, "message ", 8)==0) {
				i = 0;

				var = strstr(inbuf, "m");
				for(i = 0; i<8; i++){
					var++;
				}
				i=0;
				while(*var != '\0'){
					clientname[i] = *var;
					printf("%c", *var);				
					var++;
					i++;
				}
				clientname[i] = '\0';

				read(sd2, inbuf, sizeof(inbuf));
				cout << inbuf << endl;
				strcpy(message, inbuf);
				for(i = 0; clients[i].GetSocket() != sd2; i++);

				char* name = new char[clients[i].GetName().size()+1];
				strcpy(name, clients[i].GetName().c_str());


				time ( &rawtime );
    			timeinfo = localtime ( &rawtime );
				char output[30];
    			strftime(output, 30, "%Y-%m-%d %H:%M", timeinfo);
				
				sprintf(outbuf, "[%s] <%s> wrote: [%s]", output, name, message);

				i = 0;
				
				while(strcmp(clients[i].GetName().c_str(), clientname)&&(i<contacts)){
					i++;
				}
				if(i<contacts) {	// kalo receiver sedang online
					write(clients[i].GetSocket(), outbuf, sizeof(outbuf));
					cout << name << " messages " << clients[i].GetName() << endl;
					serverlog = new char[50];
					sprintf(serverlog, "%s messages %s", name, clients[i].GetName().c_str());
					WriteLog(serverlog);
				}
				else {	// pending
					m.SetSender(name);
					m.SetReceiver(clientname);
					m.SetTime(output);
					m.SetMessage(message);
					m.SetStatus("pending");
					serverFile.open("logmessage.txt", ios::app);
				    serverFile << m.GetSender() << endl;
				    serverFile << m.GetReceiver() << endl;
				    serverFile << m.GetTime() << endl;
				    serverFile << m.GetMessage() << endl;
				    serverFile << m.GetStatus() << endl;
				    serverFile.close();
				}
			}
			else if (strncmp(outbuf, "join ", 5)==0) {
				for(i = 0; clients[i].GetSocket() != sd2; i++);
				char* name = new char[clients[i].GetName().size()+1];
				strcpy(name, clients[i].GetName().c_str());
				serverlog = new char[50];
				sprintf(serverlog, "%s %s", name, outbuf);
				WriteLog(serverlog);
			}
			else if (strncmp(outbuf, "create ", 7)==0) {
				for(i = 0; clients[i].GetSocket() != sd2; i++);
				char* name = new char[clients[i].GetName().size()+1];
				strcpy(name, clients[i].GetName().c_str());
				serverlog = new char[50];
				sprintf(serverlog, "%s %s", name, outbuf);
				WriteLog(serverlog);
			}
			else if (strncmp(outbuf, "leave ", 6)==0) {
				for(i = 0; clients[i].GetSocket() != sd2; i++);
				char* name = new char[clients[i].GetName().size()+1];
				strcpy(name, clients[i].GetName().c_str());
				serverlog = new char[50];
				sprintf(serverlog, "%s %s", name, outbuf);
				WriteLog(serverlog);
			}
			else {
				// do nothing
			}
			
		}
	}
	printf("\nchat has finished\n");
	
}

void interrupt_handler(int sig){
	endloop = 1;
	close(sd);
	printf("Interrupt recieved: shutting down server!\n");
	return;
}

void CheckPending(int sd2) {
	int i;
	char outbuf[BUFF_LENGTH];
	vector<string> newSender;
	ifstream serverFile;
	
	string sender;
	string receiver;
	string timestamp;
	string message;
	string status;

	serverFile.open("logmessage.txt", ios::in);
	for(i = 0; clients[i].GetSocket() != sd2; i++);
	while(!serverFile.eof()) {
		getline(serverFile,sender);
		if(strcmp(sender.c_str(), "")==0)
			continue;
		getline(serverFile,receiver);
		getline(serverFile,timestamp);
		getline(serverFile,message);
		getline(serverFile,status);
		if(strcmp(receiver.c_str(), clients[i].GetName().c_str())==0) {
			if(strcmp(status.c_str(), "pending")==0) {
				sprintf(outbuf, "[%s] <%s> wrote: [%s]", timestamp.c_str(), sender.c_str(), message.c_str());
				write(clients[i].GetSocket(), outbuf, sizeof(outbuf));
			}
		}
	}
	serverFile.close();
}

void *manage_connection(void *sdp) {

	char    inbuf[BUFF_LENGTH];       /* buffer for incoming data  */
	char	outbuf[BUFF_LENGTH];	/* buffer for outgoing data  */
	
	int i;			
	int sd2 = *((int *)sdp);
	int j = ((int *)sdp)-active_socket;	/* use pointer arithmetic to get this thread's index in array */
	for(i = 0; i < BUFF_LENGTH; i++){
		inbuf[i] = 0;
		outbuf[i] = 0;
	}
	sprintf(outbuf, "START");
	write(sd2, outbuf, sizeof(outbuf));

	CheckPending(sd2);
	chat(sd2);

	tid[j] = (pthread_t)-1; /* free thread array entry */
	
	close(sd2);
	contacts--;
	return &thread_retval;

}

int main(int argc, char** argv){
	
	if(argc < 2){
		printf("\nUsage: ./servertcp 'IP' ['port'] \nUse 127.0.0.1 as IP if you want to test program on localhost, port number is optional!\n\n");
		exit(1);
	}

	struct sockaddr_in sad;
	struct sockaddr_in cad;
	socklen_t alen;
	
	contact newcontact;	
	
	int sd2, port, n, i, j = 0;
	char* var;
	char clientname[256];
	char busymsg[] = "BUSY";
	char buffer[BUFF_LENGTH];

	for(i=0;i<MAXTHREADS;i++) {
		tid[i] = (pthread_t)-1;
		active_socket[i] = -1;
	}

	for(i = 0; i < BUFF_LENGTH; i++){
		buffer[i] = 0;
	}
	
	if(argc == 3){
		port = atoi(argv[2]);
		while(port < 0 || port > 64 * KILO){
			printf("Bad port number, buond limits are (0,%d)\n\nEnter a new port number: ", 64 * KILO);
			scanf("%d", &port);
		}
	}else{
		port = PROTO_PORT;
	}

	memset((char*)&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	n = inet_aton(argv[1], &sad.sin_addr);
	sad.sin_port = htons((u_short)port);

	printf("Server IP address and service port: [%s]:[%d]\n", argv[1], port);
	printf("Server IP address and Service Port [%d]:[%d]\n\n", sad.sin_addr.s_addr, sad.sin_port);

	sd = socket(PF_INET, SOCK_STREAM, 0);
	if(sd < 0) {
		perror("Socket creation failed\n");
		exit(1);
	}
	
	n = bind(sd, (struct sockaddr *)&sad, sizeof(sad));
	if(n == -1){
		perror("Error in Bind\n");
		close(sd);
		close(n);
		exit(1);
	}

	n = listen(sd, QLEN);
	if(n < 0){
		perror("Listen failed\n");
		exit(1);
	}
	
	signal(SIGINT, interrupt_handler);
	WriteLog("Server start");

	while(!endloop){

		alen = sizeof(cad);
	
		printf("Server is waiting for a Client to serve...\n");
		
		sd2 = accept(sd, (struct sockaddr *)&cad, &alen);
		if (sd2 < 0) {
			if(endloop) break;
			perror("Accept failed\n");
			exit(1);
		}	
		
		if(contacts < MAXTHREADS) {
			
			n = read(sd2, buffer, sizeof(buffer));
			
			var = strstr(buffer, "<");
			i = 0;		
			var++;	
			while(*var != '>'){
				clientname[i] = *var;
				var++;
				i++;
			}
			clientname[i] = '\0';
				
			cout << clientname << " is logged in" << endl;
			serverlog = new char[50];
			sprintf(serverlog, "%s logged in", clientname);
			WriteLog(serverlog);
	
			strcpy(newcontact.contactname, clientname);
			newcontact.contactsd = sd2;
			clients[contacts].SetName(newcontact.contactname);
			clients[contacts].SetSocket(newcontact.contactsd);

			//look for the first empty slot in thread array 
			for(i=0;tid[i]!=(pthread_t)-1;i++);
			
			active_socket[i] = clients[contacts].GetSocket();

			if(pthread_create(&tid[i], NULL, manage_connection, &active_socket[i])!=0) {
				perror("Thread creation");
				printf("test if create\n");
				tid[i] = (pthread_t)-1; // to be sure we don't have unknown values... cast
				continue;
			}
			contacts++;
		
		} else {  //too many threads 
			printf("Maximum threads active: closing connection\n");
			write(clients[contacts].GetSocket(), busymsg, strlen(busymsg)+1);
			close(clients[contacts].GetSocket());
		}

	}
	WriteLog("Server shut down");
	printf("Server finished\n");
	return 0;
}
