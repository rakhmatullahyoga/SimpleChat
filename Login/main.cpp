/* 
 * File:   main.cpp
 * Author: linda.sekawati
 *
 * Created on December 4, 2014, 10:21 AM
 */

#include <cstdlib>
#include "user.h"
using namespace std;

/*
 * 
 */
int main() {
    User u;
    string input;
    bool exit=false;
    while(!exit){
        cout << "> ";
        cin >> input;
        if (input.compare("signup")==0){
            u.Signup();
        }
        else if(input.compare("login")==0){
            u.Login();
        }
        else if(input.compare("quit")==0){
            cout<<"Bye"<<endl;
            exit=true;
        }
        else{
            cout<<"Wrong input"<<endl;
        }
    }
    return 0;
}

