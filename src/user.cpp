#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include "user.h"
using namespace std;

User::User(){
    name = "";
    password = "";
    online = false;
}

User::~User()
{
	
}
void User::Signup()
{
    cout <<"Name    :";
    cin>>name;
    cout <<"Password:";
    cin>>password;
    if(!ReadSignUp()){
        WriteUser();
        cout << "Signup success!" <<endl;  
    }
    else{
        cout << "The account has been existed"<<endl;
    }
    
}

string User::GetName() {
    return name;
}

bool User::isOnline() {
    return online;
}

void User::Login()
{
    cout <<"Name    :";
    cin>>name;
    cout <<"Password:";
    cin>>password;
    if(ReadLogin()){
        online = true;
        cout << "Login success!" <<endl;
    }
    else{
        cout << "Invalid username or password"<<endl;
        sleep(1);
    }
}

void User::Logout(){
    if((name.compare("")==0) && (password.compare("")==0)){
        cout << "Not Login yet" << endl;
    }
    else{
        name="";
        password="";
        online = false;
        cout << "Successfully Logout" << endl;
    }
}

void User::SetSocket(int sock) {
    socket = sock;
}

int User::GetSocket() {
    return socket;
}

void User::SetName(string _name) {
    name = _name;
}

bool User::ReadSignUp()
{
    string a,b;
    ifstream myfile;
    bool userfound=false;
    myfile.open ("loguser.txt");
    while(!myfile.eof() && !userfound) // To get you all the lines.
    {
	    getline(myfile,a); // Saves the line in a.
        getline(myfile,b); // Saves the line in b.
        if(name.compare(a)==0){
            userfound=true;
        }
    }
    return userfound;
}

bool User::ReadLogin()
{
    string a,b;
    ifstream myfile;
    bool userfound=false;
    myfile.open("loguser.txt");
    while(!myfile.eof() && !userfound) // To get you all the lines.
    {
	    getline(myfile,a); // Saves the line in a.
        getline(myfile,b); // Saves the line in b.
        if((name.compare(a)==0)&&(password.compare(b)==0)){
            userfound=true;
        }
    }
    return userfound;
}

bool User::ReadGroup()
{
    string a;
    string delimiter = ";";
    ifstream myfile;
    myfile.open ("loguser.txt");
    while(!myfile.eof()) // To get you all the lines.
      {
	        getline(myfile,a); // Saves the line in a.
                a = a.substr(0 + delimiter.length());
                if(group.compare(a)==0){
                    return true;
                }
                else{return false;}
      }
}

bool User::ReadMember()
{
    string a;
    string delimiter = ";";
    ifstream myfile;
    myfile.open ("loguser.txt");
    /*while(!myfile.eof()) // To get you all the lines.
      {
	        getline(myfile,a); // Saves the line in a.
                while()   
                    a = a.substr(0 + delimiter.length());
                    a.erase(0, a.find(delimiter) + delimiter.length());
                    if(member.compare(a)==0){
                        return true;
                    }
                    else{return false;}
      }*/
}

void User::WriteUser()
{
    ofstream myfile;
    myfile.open ("loguser.txt", ios::app);
    myfile <<endl<< name<<endl;
    myfile << password;
    myfile.close();
}

void User::WriteGroup()
{
    ofstream myfile;
    myfile.open ("loggroup.txt", ios::app);
    myfile << group;
    myfile.close();
}

void User::WriteMember()
{
    ofstream myfile;
    myfile.open ("loggroup.txt", ios::app);
    myfile << member;
    myfile.close();
}

void User::CreateGroup(string group)
{
    if(ReadGroup()){
        WriteGroup();
        cout<<"Group "<<group<<" created."<<endl;
    }
    else{
        cout<<group<<" already exists."<<endl;
    }
}

void User::JoinGroup(string member)
{
    if(!ReadMember()){
        if(ReadGroup()){
            WriteMember();
            cout<<"Joining group "<< group<<endl;
        }
        else{
            cout<<group<<" doesn't exists"<<endl;
        }
    }
    else{
        cout<< member <<" has joined"<<endl;
    }

}

void User::LeaveGroup(string member)
{
    if (ReadMember()){
        cout<<"Leaving group "<<group<<endl;
        //Aksi penghapusan
    }
    else{
        cout<<"You aren't member of "<<group<<endl;
    }

}
