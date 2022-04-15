#include "server.h"
DataBase *Server::rgdb = new DataBase;
//！因为在静态回调函数中操作数据库实例rgdb来访问数据库，所以rgdb也应该声明为静态的
static int data_n = 0;	//用户数量

Server::Server(const char *ip,int port)
{
	userlist = new Info;	
	rgdb = new DataBase;	//rgdb是数据库

	//创建事件集合base
	base = event_base_new();

	//服务器socket绑定
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	//server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//创建监听对象
	listener = evconnlistener_new_bind(base, listener_cb, NULL, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 10, (struct sockaddr *)&server_addr, sizeof(server_addr));
	//解释传入的参数：断开连接的时候释放对象，而且同一个客户端地址可以重复绑定
	//监听队列长度为10
	if (NULL == listener)
	{
		cout<< "bind error" << endl;
	}

	cout << "服务器初始化成功 开始监听客户端" << endl;
	event_base_dispatch(base);//开始监听
	event_base_free(base);
	
}

//客户端连接时的处理回调函数
void Server::listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg)
{
	cout << "接受客户端 fd = " << fd << endl;

	//创建线程来工作
	thread client_thread(client_handler,fd);
	client_thread.detach();	//线程分离 资源自动回收
}

void Server::client_handler(int fd)
{
	//创建集合
	struct event_base *base = event_base_new();

	//创建bufferevent对象
	struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (NULL == bev)
	{
		cout << "bufferevent_socket_new error" << endl;
	}

	//给bufferevent设置回调函数
	bufferevent_setcb(bev, read_cb,NULL,event_cb, NULL);

	//使能回调函数
	bufferevent_enable(bev, EV_READ);
	
	event_base_dispatch(base);	//监听集合(监听客户端有无数据发送)

	event_base_free(base);	//释放掉监听集合
}

void Server::read_cb(struct bufferevent *bev,void *ctx)
{
	char buf[1024] = {0};
	int size = bufferevent_read(bev, buf, sizeof(buf));
	if (size < 0)
	{
		cout << "buffer read error" <<endl;
	}
	
	cout << buf << endl;

	Json::Reader reader;	//解析json对象(解码客户端发来的数据)
	Json::FastWriter writer; //封装json对象(打包即将要发给客户端的数据)
	Json::Value val;

	if(!reader.parse(buf,val)) 	//把未解码的数据转换成json对象
	{
		cout << "服务器解析数据失败" <<endl;
	}

	string cmd = val["cmd"].asString();//把json对象转换成可以操作的字符串
	
	if (cmd == "register")
	{
		server_register(bev,val);
	}
	else if (cmd == "login")
	{
		server_login(bev,val);
	}	

}


void Server::event_cb(struct bufferevent *bev,short
     what, void *ctx)
{
	
}


Server::~Server()
{
	event_base_free(base);
}

void Server::server_register(struct bufferevent *bev,Json::Value val)
{
	rgdb->my_database_connect("userdata");
	
	if(rgdb->my_database_user_exist(val["user"].asString()))
	{
		Json::Value val;
		val["cmd"]= "register_reply";
		val["result"]="failure";

		cout << "注册失败，用户已存在" << endl; 

		string b = Json::FastWriter().write(val);
		if(bufferevent_write(bev,b.c_str(),strlen(b.c_str())) < 0)
		{
			cout << "bufferevent_write" <<endl;
		}
	}
	else
	{
		rgdb->my_database_user_password(val["user"].asString(),val["password"].asString());
		Json::Value val;
		val["cmd"]="register_reply";
		val["result"]="success";
		string b = Json::FastWriter().write(val);
        if(bufferevent_write(bev,b.c_str(),strlen(b.c_str())) < 0)//write第二个参数是指针，所以用c_str()返回指针
        {
           cout << "bufferevent_write" <<endl;
        }
	}
	rgdb->my_database_disconnect();//用完记得关
}

void Server::server_login(struct bufferevent *bev,Json::Value val)
{

	rgdb->my_database_connect("userdata");
	if(!rgdb->my_database_user_exist(val["user"].asString()))
	{
		Json::Value val;
         val["cmd"]= "login_reply";
         val["result"]="user_not_exist"; 
         cout << "登录失败，用户不存在" << endl;
 
         string b = Json::FastWriter().write(val);
         if(bufferevent_write(bev,b.c_str(),strlen(b.c_str())) < 0)
         {
             cout << "bufferevent_write" <<endl;
	     }
		 return;
	}
	//此处作密码的判断
	else
	{
	if(!rgdb->my_database_password_check(val["user"].asString(),val["password"].asString()))
	{
		Json::Value val;
         val["cmd"]= "login_reply";
         val["result"]="password effort"; 
         cout << "登录失败，密码错误" << endl;
string b = Json::FastWriter().write(val);
          if(bufferevent_write(bev,b.c_str(),strlen(b.c_str())) < 0)
          {
              cout << "bufferevent_write" <<endl;
          }   
	return;
	}

	if(val["user"].asString()=="admin")
	{
		{
		Json::Value val;
          val["cmd"]= "login_reply";
          val["result"]="login_main_success";
          cout << "主控登录成功" << endl;
          
          string b = Json::FastWriter().write(val);
          if(bufferevent_write(bev,b.c_str(),strlen(b.c_str())) < 0)
          {
              cout << "bufferevent_write" <<endl;
          }   
          return;
		}
	}
	else
		{
		Json::Value val;
          val["cmd"]= "login_reply";
          val["result"]="login_success";
          cout << "分控登录成功" << endl;
          
          string b = Json::FastWriter().write(val);
          if(bufferevent_write(bev,b.c_str(),strlen(b.c_str())) < 0)
          {
              cout << "bufferevent_write" <<endl;
          }   
          return;
		}
		}

}


