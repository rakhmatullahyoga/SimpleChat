#include "user.h"
using namespace std;

User::User(){
    name = " ";
    password = " ";
}

User::~User()
{
	
}

bool User::ReadLogin()
{
    string a,b;
    ifstream myfile;
    myfile.open ("loguser.txt");
    while(!myfile.eof()) // To get you all the lines.
      {
	        getline(myfile,a); // Saves the line in a.
                getline(myfile,b); // Saves the line in b.
                if(name.compare(a)==0){
                    if(password.compare(b)==0){
                        return true;
                    }
                    else{return false;}
                }
                else{return false;}
      }
    
}

bool User::ReadSignUp()
{
    string a,b;
    ifstream myfile;
    myfile.open ("loggroup.txt");
    while(!myfile.eof()) // To get you all the lines.
      {
	        getline(myfile,a); // Saves the line in a.
                getline(myfile,b); // Saves the line in b.
                if((name.compare(a)==0)||(password.compare(b)==0)){
                    return false;
                }
                else{return true;}
      }
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
    myfile.open ("loguser.txt");
    myfile << name<<endl;
    myfile << password;
    myfile.close();
}

void User::WriteGroup()
{
    ofstream myfile;
    myfile.open ("loggroup.txt");
    myfile << group;
    myfile.close();
}

void User::WriteMember()
{
    ofstream myfile;
    myfile.open ("loggroup.txt");
    myfile << member;
    myfile.close();
}

bool User::Login()
{
    cin>>name;
    cin>>password;
    if(ReadLogin()){
       cout << "Login success" <<endl;
       return true;
    }
    else{
        cout << "Invalid username or password"<<endl;
        return false;
    }
}

bool User::Signup()
{
    cin>>name;
    cin>>password;
    if(ReadSignUp()){
        WriteUser();
        cout << "Signup seccess" <<endl;  
        return true;
    }
    else{
        cout << "The account is has been existed"<<endl;
        return false;
    }
    
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
        cout<<member<<" has joined"<<endl;
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
