#include "database.h"

DataBase::DataBase()
{
	mysql = mysql_init(NULL);
}

DataBase::~DataBase()
{
	mysql_close(mysql);
}

void DataBase::my_database_connect(const char *databasename)
{
	mysql = mysql_init(NULL);//一定要加这行，不然的话重复链接会报错
	mysql = mysql_real_connect(mysql,"localhost","root","dna8697",databasename,0,NULL,0);
	
	if (NULL == mysql)
	{
		cout << "connect database failure" << endl;
	}

	if (mysql_query(mysql, "set names utf8;") != 0)
	{
		cout << "mysql_query error" << endl;
	}
}

int DataBase::my_username_get(string *s)
{
	if(mysql_query(mysql,"set names utf8;")!=0)
	{
		cout << "mysql_query error" << endl;
	}
	if(mysql_query(mysql,"show tables;")!=0)
	{
		cout << "mysql_query error" << endl;
	}

	MYSQL_RES *res = mysql_store_result(mysql);//读取数据库内容
	if(NULL == res)
	{
		cout << "mysql_store_result error" << endl;
	}

	int count = 0;
	MYSQL_ROW row;
	while (row = mysql_fetch_row(res))//读取设备名，从而存到row字符串中
	{
		s[count] += row[0];
		count ++;
	}

	return count;
}

void DataBase::my_get_deviceuser(string name,string &s)
{
	char sql[1024] = {0};
	sprintf(sql,"select ouname from %s;",name.c_str());//.c_str()的作用是转为char *
	if(mysql_query(mysql,sql)!=0)
	{
		cout << "mysql_query error" << endl;
	}
	
	MYSQL_RES *res = mysql_store_result(mysql);
	if(NULL == res)
    {
        cout << "mysql_store_result error" << endl;
    }

	MYSQL_ROW row = mysql_fetch_row(res);
	s += row[0];
	
}

void DataBase::my_database_disconnect()
{
	mysql_close(mysql);
}

bool DataBase::my_database_user_exist(string name)
{
	char sql[128] = {0};
	sprintf(sql,"show tables like '%s';",name.c_str());//注意这个指令是要加''的
	if(mysql_query(mysql, sql)!=0)
	{
		cout << "mysql_query error" <<endl;
	}
	
	MYSQL_RES *res = mysql_store_result(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);

	if (NULL == row)		//用户不存在
	{
		return false;
	}
	else					//用户存在
	{
		return true;
	}
	
}


void DataBase::my_database_user_password(string name,string password)
{
	char sql[128]={0};
	sprintf(sql,"create table %s (password varchar(128)) character set utf8;",name.c_str());	//sprintf()函数构造sql语句

	if(mysql_query(mysql,sql)!=0)
	{
		cout << "query error" << endl;
	}

	memset(sql , 0 ,sizeof(sql));
	sprintf(sql,"insert into %s (password) values ('%s');",name.c_str(),password.c_str());

	cout << "注册成功 "<<endl;
 	cout << "密码："<< password.c_str() << endl;
	cout << "用户名："<< name.c_str() <<endl ;

	if(mysql_query(mysql,sql)!=0)
	{
		cout<< "query error" <<endl;
	}

}

//要操作mysql数据库的话首先要连接已经建立的database，然后选择table，通过 mysql_store_result( ) 把 table 的内容选出来放到句柄，通过 mysql_num_fields( ) 统计 table 的字段，通过 mysql_fetch_field( ) 获得字段。最后通过strcmp来匹配密码。

bool DataBase::my_database_password_check(string name,string password)
{
	//检查登录密码
	MYSQL_RES *result;
	MYSQL_FIELD *fields;
	unsigned int num_fields;
	MYSQL_ROW row;  //记录结构体
	char sql[128]={0};

    sprintf(sql, "select password  from %s  ",name.c_str());
    bool ret = mysql_query(mysql, sql); //成功返回0

    if (ret) {
        printf("数据库查询出错，%s 错误原因： %s\n", sql, mysql_error(mysql));
        mysql_close(mysql);
        return false;
    }

	result = mysql_store_result(mysql);
	fields = mysql_fetch_fields(result);
	//num_fields = mysql_num_fields(result);
	row = mysql_fetch_row(result);

    if (row == NULL) {	//没有查找到记录
        mysql_free_result(result);
        mysql_close(mysql);
        return false;
    }


	if( strcmp((row[0]),password.c_str() ) !=0 )
	 {return false;}
	else {return true;}
		
	return true;
}


