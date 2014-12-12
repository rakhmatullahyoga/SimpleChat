#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <signal.h>
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
bool newNotif;
bool interrupted;

void UpdateMessageLog() {
	ofstream clientFile;
	clientFile.open("logmessage.txt", ios::trunc);
	while(!messageQueue.empty()) {
		clientFile << messageQueue.front().GetSender() << endl;
		clientFile << messageQueue.front().GetReceiver() << endl;
		clientFile << messageQueue.front().GetTime() << endl;
		clientFile << messageQueue.front().GetMessage() << endl;
		clientFile << messageQueue.front().GetStatus() << endl;
		messageQueue.pop();
	}
	clientFile.close();
}

void ShowOldMessage(string _sender) {
	ifstream clientFile;
	clientFile.open("logmessage.txt", ios::in);
	string sender;
	string receiver;
	string timestamp;
	string message;
	string status;
	while(!clientFile.eof()) {
		getline(clientFile,sender);
		if(strcmp(sender.c_str(), "")==0)
			continue;
		getline(clientFile,receiver);
		getline(clientFile,timestamp);
		getline(clientFile,message);
		getline(clientFile,status);
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
	clientFile.close();
}

void ShowNewMessage(string _sender) {
	Message readMsg;
	ifstream clientFile;
	
	clientFile.open("logmessage.txt", ios::in);
	string sender;
	string receiver;
	string timestamp;
	string message;
	string status;
	cout << "-------------- New message(s) --------------" << endl;
	while(!clientFile.eof()) {
		getline(clientFile,sender);
		if(strcmp(sender.c_str(), "")==0)
			continue;
		getline(clientFile,receiver);
		getline(clientFile,timestamp);
		getline(clientFile,message);
		getline(clientFile,status);
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
	clientFile.close();
	UpdateMessageLog();
}

void ReadNotification() {
	ifstream clientFile;
	std::vector<string> newSender;
	string sender;
	string receiver;
	string timestamp;
	string message;
	string status;

	newNotif = false;
	clientFile.open("logmessage.txt", ios::in);
	while(!clientFile.eof()) {
		getline(clientFile,sender);
		if(strcmp(sender.c_str(), "")==0)
			continue;
		getline(clientFile,receiver);
		getline(clientFile,timestamp);
		getline(clientFile,message);
		getline(clientFile,status);
		if(strcmp(receiver.c_str(), u.GetName().c_str())==0) {
			if(strcmp(status.c_str(), "read")!=0) {
				newNotif = true;
				if(std::find(newSender.begin(), newSender.end(), sender)==newSender.end()) {
					newSender.push_back(sender);
				}
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
	}
	clientFile.close();
}

void *manage_reading(void* param){
	int n, i;
	char inbuf[BUFF_LENGTH];
	char *var;
	ifstream inputclientFile;
	ofstream outputclientFile;
	Message readMsg;
	string sender;
	string receiver;
	string timestamp;
	string message;
	string status;
	bool pendingMsg = false;

	while(served == 0){

		for(i = 0; i < BUFF_LENGTH; i++){
			inbuf[i] = 0;
		}

		n = read(u.GetSocket(), inbuf, sizeof(inbuf));
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
			inputclientFile.open("logmessage.txt", ios::in);
		    while(!inputclientFile.eof()) {
				getline(inputclientFile,sender);
				if(strcmp(sender.c_str(), "")==0)
					continue;
				getline(inputclientFile,receiver);
				getline(inputclientFile,timestamp);
				getline(inputclientFile,message);
				getline(inputclientFile,status);
				readMsg.SetSender(sender);
				readMsg.SetReceiver(receiver);
				readMsg.SetTime(timestamp);
				readMsg.SetMessage(message);
				readMsg.SetStatus(status);
				if ((strcmp(m.GetSender().c_str(), readMsg.GetSender().c_str())==0)&&(strcmp(m.GetReceiver().c_str(), readMsg.GetReceiver().c_str())==0)&&
					(strcmp(m.GetTime().c_str(), readMsg.GetTime().c_str())==0)&&(strcmp(m.GetMessage().c_str(), readMsg.GetMessage().c_str())==0)&&
					(strcmp(readMsg.GetStatus().c_str(), "pending")==0))
				{
					readMsg.SetStatus("sent");
					pendingMsg = true;
				}
				messageQueue.push(readMsg);
			}
			inputclientFile.close();
			if (pendingMsg)
			{
				UpdateMessageLog();
			}
			else {
				while(!messageQueue.empty()) {
					messageQueue.pop();
				}
				outputclientFile.open("logmessage.txt", ios::app);
			    outputclientFile << m.GetSender() << endl;
			    outputclientFile << m.GetReceiver() << endl;
			    outputclientFile << m.GetTime() << endl;
			    outputclientFile << m.GetMessage() << endl;
			    outputclientFile << m.GetStatus() << endl;
			    outputclientFile.close();
			}
		}
	}	
}

void interrupt_handler(int sig){
	char outbuf[BUFF_LENGTH];
	interrupted = true;
	sprintf(outbuf, "logout");
	write(u.GetSocket(), outbuf, sizeof(outbuf));
	close(u.GetSocket());
	printf("Interrupt recieved: shutting down client!\n");
	return;
}

int main(int argc, char** argv){
	interrupted = false;
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
	signal(SIGINT, interrupt_handler);
	do {
		do {
			system("clear");
			cout << "============== SIMPLE-CHAT ==============\n" << endl;
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
		} while(!u.isOnline()&&!interrupted);

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
			while(served == 0&&!interrupted){
				for(i = 0; i < BUFF_LENGTH; i++){
					outbuf[i] = 0;
				}
				while(getchar() != '\n');
				sleep(1);
				system("clear");
				cout << "============== SIMPLE-CHAT ==============\n" << endl;
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
					cout << "Press enter to continue...";
					getchar();
				}
				else if (strcmp(outbuf, "logout")==0) {
					u.Logout();
					close(u.GetSocket());
					served = 1;
					sleep(1);
				}
			}
		}
		else {
			cout << "\nServer Busy, closing connection" << endl;
			close(u.GetSocket());
			u.SetSocket(-1);
		}
		out:;
	} while(command.compare("exit")!=0&&!interrupted);
	cout << "Bye!" << endl;
	close(u.GetSocket());
	return 0;
}
