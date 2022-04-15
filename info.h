#ifndef INFO_H
#define INFO_H

#include <event.h>
#include <list>
#include "database.h"
using namespace std;

//用户节点
struct User
{
	string name;
	struct bufferevent *bev;
};
typedef struct User User;

struct DevUser	//设备对应的权限用户的节点
{
	string name;
};
typedef struct DevUser DevUser;

//设备信息节点
struct Dev
{	
	//这里有个细节要注意就是Dev类要放在DevUser后去声明
     list<DevUser> *l;//用来管理设备拥有的用户名的链表
     string dname;//设备名
};
typedef struct Dev Dev;


class Info
{
private:
	list<User> *online_user;	//用户信息链表
	list<Dev> *device;			//设备权限信息链表

	DataBase *mydatabase;		//数据库对象
public:
	Info();
	~Info();

};

#endif

