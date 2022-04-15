#ifndef DATABASE_H
#define DATABASE_H

#include <mysql/mysql.h>
#include <iostream>
#include <string.h>

using namespace std;

class DataBase
{
private:
	MYSQL *mysql;
public:
	DataBase();
	~DataBase();	

	void my_database_connect(const char * databasename);
	int my_username_get(string *);	//实际上得到的是设备名,写的时候变量名没想周全
	void my_get_deviceuser(string name,string &s);
	void my_database_disconnect();
	bool my_database_user_exist(string);
	void my_database_user_password(string,string);
	bool  my_database_password_check(string name,string password);
};

#endif

