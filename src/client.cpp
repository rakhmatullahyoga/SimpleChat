#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <pthread.h>

#include "user.h"
#include "message.h"

using namespace std;

#define KILO 1024
#define BUFF_LENGTH 1000
#define PROTO_PORT 60000
#define MAX_CONTACTS 5

User u;
Message m;
queue<Message> messageQueue;
int sd;
int served = 0;
fstream myfile;
bool newNotif = false;

void UpdateMessageLog() {
	myfile.open("logmessage.txt", ios::trunc);
	while(!messageQueue.empty()) {
		myfile << messageQueue.front().GetSender() << endl;
		myfile << messageQueue.front().GetReceiver() << endl;
		myfile << messageQueue.front().GetTime() << endl;
		myfile << messageQueue.front().GetMessage() << endl;
		myfile << messageQueue.front().GetStatus() << endl;
		messageQueue.pop();
	}
	myfile.close();
}

void ShowOldMessage(string _sender) {
	myfile.open("logmessage.txt");
	string sender;
	string receiver;
	string timestamp;
	string message;
	string status;
	while(!myfile.eof()) {
		getline(myfile,sender);
		getline(myfile,receiver);
		getline(myfile,timestamp);
		getline(myfile,message);
		getline(myfile,status);
		if(((strcmp(receiver.c_str(), u.GetName().c_str())==0)&&(strcmp(sender.c_str(), _sender.c_str())==0))||((strcmp(receiver.c_str(), _sender.c_str())==0)&&(strcmp(sender.c_str(), u.GetName().c_str())==0))) {
			if(((strcmp(receiver.c_str(), u.GetName().c_str())==0)&&(strcmp(sender.c_str(), _sender.c_str())==0))&&(strcmp(status.c_str(), "sent")==0)) {
				// untuk message baru, do nothing
			}
			else {
				if(((strcmp(receiver.c_str(), u.GetName().c_str())==0)&&(strcmp(sender.c_str(), _sender.c_str())==0))&&(strcmp(status.c_str(), "pending")==0)) {
					// supaya tidak baca message yg pending
				}
				else {
					cout << "[" << timestamp << "] " << sender << " : "<< message << " [" << status << "]" << endl;
				}
			}
		}
	}
	myfile.close();
}

void ShowNewMessage(string _sender) {
	Message readMsg;
	
	myfile.open("logmessage.txt");
	string sender;
	string receiver;
	string timestamp;
	string message;
	string status;
	cout << "-------------- New message(s) --------------" << endl;
	while(!myfile.eof()) {
		getline(myfile,sender);
		getline(myfile,receiver);
		getline(myfile,timestamp);
		getline(myfile,message);
		getline(myfile,status);
		readMsg.SetSender(sender);
		readMsg.SetReceiver(receiver);
		readMsg.SetTime(timestamp);
		readMsg.SetMessage(message);
		readMsg.SetStatus(status);
		if(((strcmp(receiver.c_str(), u.GetName().c_str())==0)&&(strcmp(sender.c_str(), _sender.c_str())==0))&&(strcmp(status.c_str(), "pending")==0)) {

		}
		else {
			if(((strcmp(receiver.c_str(), u.GetName().c_str())==0)&&(strcmp(sender.c_str(), _sender.c_str())==0))&&(strcmp(status.c_str(), "sent")==0)) {
				cout << "[" << timestamp << "] " << sender << " : "<< message << endl;
				readMsg.SetStatus("read");
			}
			messageQueue.push(readMsg);
		}
	}
	myfile.close();
	UpdateMessageLog();
}

void ReadNotification() {
	cout << "baca notif" << endl;
	std::vector<string> newSender;
	string sender;
	string receiver;
	string timestamp;
	string message;
	string status;
	myfile.open("logmessage.txt");
	while(!myfile.eof()) {
		getline(myfile,sender);
		getline(myfile,receiver);
		getline(myfile,timestamp);
		getline(myfile,message);
		getline(myfile,status);
		if(strcmp(receiver.c_str(), u.GetName().c_str())==0) {
			if(strcmp(status.c_str(), "read")!=0) {
				newNotif = true;
				newSender.push_back(sender);
			}
		}
	}
	if(newNotif) {
		cout << "New message(s) from ";
		cout << newSender.back();
		newSender.pop_back();
		while(!newSender.empty()) {
			cout << ", " << newSender.back();
			newSender.pop_back();
		}
		cout << "." << endl;
		myfile.close();
		newNotif = false;
	}
}

void *manage_reading(void* param){
	
	int n, i;
	char inbuf[BUFF_LENGTH];
	char *var;

	while(served == 0){

		for(i = 0; i < BUFF_LENGTH; i++){
			inbuf[i] = 0;
		}

		n = read(u.GetSocket(), inbuf, sizeof(inbuf));
		cout << inbuf << endl;
		if(!strcmp(inbuf, "logout")){
			served = 1;
		}
		else {
			i = 0;
			char* date = new char[16];
			char* clientname = new char[25];
			char* msg = new char[BUFF_LENGTH];
			var = strstr(inbuf, "[");
			var++;
			while(*var != ']'){
				date[i] = *var;	
				var++;
				i++;
			}
			date[i] = '\0';
			m.SetTime(date);
			i = 0;
			while(*var != '<')
				var++;
			var++;
			while(*var != '>'){
				clientname[i] = *var;
				var++;
				i++;
			}
			clientname[i] = '\0';
			m.SetSender(clientname);
			m.SetReceiver(u.GetName());
			while(*var != '[')
				var++;
			var++;
			i = 0;
			while(*var != ']'){
				msg[i] = *var;
				var++;
				i++;
			}
			msg[i] = '\0';
			m.SetMessage(msg);
			m.SetStatus("sent");
			myfile.open("logmessage.txt", ios::app);
		    myfile << m.GetSender() << endl;
		    myfile << m.GetReceiver() << endl;
		    myfile << m.GetTime() << endl;
		    myfile << m.GetMessage() << endl;
		    myfile << m.GetStatus() << endl;
		    myfile.close();
		}
	}	
}

int main(int argc, char** argv){

	if(argc < 2){
		cout << "\nUsage: ./clienttcp 'wanted client host name' 'IP'\nUse 127.0.0.1 as IP if you want to test program on localhost!\n" << endl;
		exit(1);
	}
	struct sockaddr_in sad;
	socklen_t alen;

	int i, n, port, oc = 0;
	char inbuf[BUFF_LENGTH], outbuf[BUFF_LENGTH], outchar;
	char *hostname;
	pthread_t tid;

	for(i = 0; i < BUFF_LENGTH; i++){
		inbuf[i] = 0;
		outbuf[i] = 0;
	}

	if(argc == 3){
		port = atoi(argv[2]);
		while(port < 0 || port > 64 * KILO){
			cout << "Bad port number, buond limits are (0," << KILO*64 << ")\n" << endl;
			cout << "Enter a new port number: ";
			cin >> port;
		}
	}else{
		port = PROTO_PORT;
	}
	string command;
	do {
		do {
			system("clear");
			cout << "======= SIMPLE-CHAT =======\n" << endl;
			cout << "> ";
			cin >> command;
			if (command.compare("signup")==0) {
				u.Signup();
				sleep(1);
			}
			else if (command.compare("login")==0) {
				u.Login();
			}
			else if (command.compare("exit")==0) {
				goto out;
			}
			else {
				cout << "input tidak valid!" << endl;
				sleep(1);
			}
			
		} while(!u.isOnline());

		memset((char*)&sad, 0, sizeof(sad));
		sad.sin_family = AF_INET;
		sad.sin_port = htons((u_short)port);
		n = inet_aton(argv[1], &sad.sin_addr);

		hostname = new char[u.GetName().size()+1];
		strcpy(hostname, u.GetName().c_str());

		sd = socket(PF_INET, SOCK_STREAM, 0);
		u.SetSocket(sd);
		
		connect(u.GetSocket(), (struct sockaddr*)&sad, sizeof(sad));
		
		sprintf(outbuf, "HELLO I AM <%s>", hostname);
		write(u.GetSocket(), outbuf, sizeof(outbuf));
		
		n = read(u.GetSocket(), inbuf, sizeof(inbuf));
		if(strcmp(inbuf, "BUSY")){
			if(pthread_create(&tid, NULL, manage_reading, NULL)!=0) {
				perror("Thread creation");
			}
			while(served == 0){
				for(i = 0; i < BUFF_LENGTH; i++){
					outbuf[i] = 0;
				}
				while(getchar() != '\n');
				sleep(1);
				system("clear");
				cout << "======= SIMPLE-CHAT =======\n" << endl;
				ReadNotification();
				cout << u.GetName() << " > ";
				scanf("%[^\n]s", outbuf);
				write(u.GetSocket(), outbuf, sizeof(outbuf));
				if (strncmp(outbuf, "message ", 8)==0) {
					getchar();
					cout << "Message :" << endl;
					scanf("%[^\n]s", outbuf);
					write(u.GetSocket(), outbuf, sizeof(outbuf));
					cout << "Message sent!" << endl;
				}
				else if (strncmp(outbuf, "join ", 5)==0) {
					
				}
				else if (strncmp(outbuf, "create ", 7)==0) {
					
				}
				else if (strncmp(outbuf, "leave ", 6)==0) {
					
				}
				else if (strncmp(outbuf, "show ", 5)==0) {
					char* var;
					char* show = new char[25];
					var = strstr(outbuf, "w");
					var++;
					var++;
					i=0;
					while(*var != '\0'){
						show[i] = *var;
						var++;
						i++;
					}
					show[i] = '\0';
					ShowOldMessage(show);
					if(newNotif) {
						ShowNewMessage(show);
					}
					newNotif = false;
				}
				else if (strcmp(outbuf, "logout")==0) {
					u.Logout();
					close(u.GetSocket());
					served = 1;
				}
				sleep(1);			
			}
		}
		else {
			cout << "\nServer Busy, closing connection" << endl;
			close(u.GetSocket());
			u.SetSocket(-1);
		}
		out:;
	} while(command.compare("exit")!=0);
	cout << "Bye!" << endl;
	close(u.GetSocket());
	return 0;
}
