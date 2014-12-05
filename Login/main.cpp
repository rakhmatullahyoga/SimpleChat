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
    cout << "Masukkan command" << endl;
    cin >> input;
    if (input.compare("signup")==0){
        u.Signup();
    }
    else if(input.compare("login")==0){
        u.Login();
    }   
    return 0;
}

