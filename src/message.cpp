#include <iostream>
#include <string>
#include "message.h"
using namespace std;

Message::Message() {
	sender = "";
	receiver = "";
	timestamp = "";
	message = "";
	status = "";
}

Message::~Message() {

}

string Message::GetSender() {
	return sender;
}

string Message::GetReceiver() {
	return receiver;
}

string Message::GetTime() {
	return timestamp;
}

string Message::GetMessage() {
	return message;
}

string Message::GetStatus() {
	return status;
}

void Message::SetSender(string _sender) {
	sender = _sender;
}

void Message::SetReceiver(string _receiver) {
	receiver = _receiver;
}

void Message::SetTime(string _time) {
	timestamp = _time;
}

void Message::SetMessage(string msg) {
	message = msg;
}

void Message::SetStatus(string _status) {
	status = _status;
}