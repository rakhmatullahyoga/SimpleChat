/* 
 * File:   login.h
 * Author: linda.sekawati
 *
 * Created on December 4, 2014, 10:25 AM
 */

#ifndef USER_H
#define	USER_H
#include <string>
#include <iostream>
#include <fstream>
using namespace std;
class User{
        public :
            User();
            ~User();
            void Signup();
            void Login();
            void Logout();
            bool ReadSignUp();
            bool ReadLogin();
            bool ReadGroup();
            bool ReadMember();
            void WriteUser();
            void WriteGroup();
            void WriteMember();
            void CreateGroup(string group);
            void JoinGroup(string member);
            void LeaveGroup(string member);
            
            
        private :
            string name;
            string password;
            string member;
            string group;

};
#endif	/* LOGIN_H */

