#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <iostream>
using namespace std;

class Message
{
public:
	Message();
	~Message();
	string GetSender();
	string GetReceiver();
	string GetTime();
	string GetMessage();
	string GetStatus();
	void SetSender(string _sender);
	void SetReceiver(string _receiver);
	void SetTime(string _time);
	void SetMessage(string msg);
	void SetStatus(string _status);
private:
	string sender;
	string receiver;
	string timestamp;
	string message;
	string status;	
};
#endif