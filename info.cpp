#include "info.h"

Info::Info()
{
	online_user = new list<User>;
	device =new list<Dev>;	
	
	mydatabase = new DataBase ;
	mydatabase->my_database_connect("user");	//连接user数据库，数据库存放的是设备对应的用户信息

	string dev_name[24];	//devname代表不同的设备名字
	int testname_num = mydatabase->my_username_get(dev_name);

	cout << testname_num <<endl;
	for (int i =0; i<testname_num ; i++)
	{
		Dev d;
		d.dname = dev_name[i];
		cout << dev_name[i] << endl;
		d.l = new list<DevUser>;
		
		string member;//设备对应的权限用户
		mydatabase->my_get_deviceuser(dev_name[i],member);
		cout << member << endl;
		
		//解析读回的权限用户名字符串
		
		int start = 0,end = 0;
		DevUser u;
		while(1)
		{
			//string容器的相关使用
			end = member.find('|',start);//从start开始找
			if(-1 == end)
			{
				break;
			}
			u.name = member.substr(start,end-start);//得到子串
			d.l->push_back(u);//把节点挂载到设备权限用户链表
			start = end + 1 ;
			u.name.clear();//用完的实例记得释放
		}
		u.name = member.substr(start,member.size()-start);
		d.l->push_back(u);		

		device->push_back(d);//把该设备信息挂载到设备链表上
	}
	
	for (list<Dev>::iterator it = device->begin();it != device->end();it++)
	{
		cout<< "设备名:" << it->dname <<endl;
		for (list<DevUser>::iterator i = it->l->begin();i!= it->l->end();i++)
		{
			cout << i->name <<endl;		
		}
	}
	
	mydatabase->my_database_disconnect();
	cout << "链表初始化成功"<<endl;
}

