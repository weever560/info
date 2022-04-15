#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
       #include <netinet/in.h>
       #include <arpa/inet.h>
#include <event.h>
#include <string.h>
#include <thread>
#include <iostream>
#include <event2/listener.h>
#include "info.h"
#include <jsoncpp/json/json.h>

using namespace std;

#define IP "10.0.12.6"

#define PORT 80

class Server
{
private:
	struct event_base *base;	//监听集合
	struct evconnlistener *listener;  	//监听事件
	Info *userlist;				//链表对象	
	static DataBase *rgdb;			//数据库对象

private:
	static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg);//回调函数需要声明成静态的，方便调用
	static void client_handler(int fd);
	static void read_cb(struct bufferevent *bev,void *ctx);
	static void event_cb(struct bufferevent *bev,short what, void *ctx);
	
	//静态成员函数中调用的也要是静态的才行
	static void server_register(struct bufferevent *bev,Json::Value val); 
	static void server_login(struct bufferevent *bev,Json::Value val);
public:
	Server(const char *ip = "10.0.12.6", int port = 80);
	~Server();
	
};

#endif

