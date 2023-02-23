//添加c++语言库
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <string>
//添加网络头文件
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;


char buf[1500];	//接收内容缓存区
string sent;	//发送内容缓存区
SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//全局变量服务端socket
SOCKET socketClient;	//全局变量客户端socket

class User	//	用户基类 ——定义抽象类 
{
public:
	string ID;		//账号 
	string Password;//密码 
	string Type;	//用户类型 
	float Money;	//账户余额 
	//定义纯虚函数——抽象类 
	virtual void Register() = 0;	//注册函数 
	virtual void Enter() = 0;		//登录函数 
	virtual void Input() = 0;		//用户信息从文件中读入 
	virtual void Output() = 0;		//用户信息输出到文件中 
	virtual void getUserType() = 0;		//获取用户类型 
};

class Merchant : public User// 商家子类——继承用户基类 
{
public:
	//对纯虚函数进行重写 
	void getUserType() { Type = "商家"; }
	void Register();	//注册 
	void Enter();		//登录 
	void Display(int i);	//商家用户管理 
	void Input();			//从文件中读入信息 
	void Output();			//将信息输入到文件 
};

class Consumer : public User// 消费者子类——继承用户基类 
{
public:
	//对纯虚函数进行重写 
	void getUserType() { Type = "消费者"; }
	void Register();	//注册 
	void Enter();		//登录 
	void Display(int i);	//消费者用户管理 
	void Input();			//从文件中读入信息 
	void Output();			//将信息输出到文件
};

class Commodity		//定义商品基类 
{
public:
	string Num;		//商品编号 
	string Name;	//商品名称 
	string Type;	//商品类型（图书、电器、服装、食品）
	string  Host;	//所属商家 
	float Ocost;	//原价 
	float Discount;	//折扣 
	int Amout;		//数量 
	string Describle;		//商品描述 
	void CommodityAdd(string id1);	//添加商品 
	void CommodityShow();			//展示商品 
	void CommodityCheck(string id2);//修改商品信息 
	void Input();			//从文件中读入
	void Output();			//输出到文件中
	//定义虚函数 
	virtual void getPrice() { cout << "该商品现价为：" << Ocost * Discount << endl; }	//获取商品现价 
};

class Book : public Commodity		//定义图书子类——继承商品基类 
{
public:
	void getCommodityType() { Type = "图书"; }
	void getPrice() { cout << "该图书现价为：" << Ocost * Discount << endl; }
};

class ElectricApp : public Commodity		//定义电器子类——继承商品基类 
{
public:
	void getCommodityType() { Type = "电器"; }
	void getPrice() { cout << "该电器现价为：" << Ocost * Discount << endl; }
};

class Clothing : public Commodity		//定义服装子类——继承商品基类 
{
public:
	void getCommodityType() { Type = "服装"; }
	void getPrice() { cout << "该服装现价为：" << Ocost * Discount << endl; }
};

class Food : public Commodity		//定义食品子类——继承商品基类 
{
public:
	void getCommodityType() { Type = "食品"; }
	void getPrice() { cout << "该食品现价为：" << Ocost * Discount << endl; }
};

class Order
{
public:
	string ConHost;			//所属消费者ID 
	int Ordernum;			//订单编号
	string CommodityName;	//商品名 
	int Commoditynum;		//商品数量 
	float CommodityPrice;	//商品价格
	string MerHost;			//所属店铺 
	int isExist;			//订单是否存在 == 1 表示在购物车中，== 0表示已删除或支付成功 , == 2表示在订单中但未交易成功 
	void Pay(int i);		//生成订单
	void Payment(int i);	//支付订单
	void Delete(int i);		//删除购物车内的商品 
	void Change(int i);		//修改购物车商品数量 
};


//定义四个向量存储商家信息、消费者信息、商品信息、订单信息 
//存储商家和消费者信息 
vector <Merchant> MerUser;
vector <Consumer> ConUser;
//存储商品信息 
vector <Commodity> object;
//存储订单信息 
vector <Order> orders;


void Order::Payment(int i)	//支付订单 
{
	int num;
	int money;
	int amout;
	Consumer c;
	string name;
	cout << "用户输入要支付订单的编号！" << endl;
	//接收客户端发送的编号
	buf[0] = { 0 };
	recv(socketClient, buf, sizeof(buf), 0);
	num = atoi(buf);
	int flag = 0;
	while (flag == 0)
	{
		for (int j = 0; j < orders.size() && flag == 0; j++)
		{
			if (orders.at(j).Ordernum == num && orders.at(j).isExist == 2 && orders.at(j).ConHost == ConUser.at(i).ID)
			{
				//发现该订单编号有效（属于该用户、订单未支付、订单未删除）
				flag = 1;
				sent = "1";
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				//给客户端返回1
				for (int m = 0; m < object.size(); m++)
				{
					if (orders.at(j).CommodityName == object.at(m).Name)
					{
						orders.at(j).isExist = 0;
						amout = orders.at(j).Commoditynum;
						money = amout * object.at(m).Discount * object.at(m).Ocost;
						if (money > ConUser.at(i).Money)
						{
							sent = "2";
							send(socketClient, sent.c_str(), sizeof(sent), 0);
							//若用户余额不足,订单删除
							object.at(m).Amout += amout;	//修改object中商品信息 
							cout << "用户的余额不足，支付失败！" << endl;
							return;
						}
						else
						{
							sent = "3";
							send(socketClient, sent.c_str(), sizeof(sent), 0);
							ConUser.at(i).Money -= money;
							for (int k = 0; k < MerUser.size(); k++)
							{
								if (orders.at(j).MerHost == MerUser.at(k).ID)
								{
									MerUser.at(k).Money += money;
									cout << "用户支付成功！" << endl;
									cout << "用户的余额为：" << ConUser.at(i).Money << endl;
									buf[0] = { 0 };
									gcvt(ConUser.at(i).Money, 6, buf);
									sent = buf;
									send(socketClient, sent.c_str(), sizeof(sent), 0);
									return;	//返回消费者管理界面
								}
							}
						}
					}
				}
			}
		}
		if (flag == 0)
		{
			sent = "0";
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			cout << "用户输入的订单编号不正确，用户重新输入！" << endl;
			buf[0] = { 0 };
			recv(socketClient, buf, sizeof(buf), 0);
			num = atoi(buf);
		}
	}
}

void Order::Pay(int i)			//生成订单 
{
	int num;	//订单编号
	int amount;	//订单个数
	int amout;	//订单中商品数量
	string name;
	cout << "用户输入要生成订单的个数！" << endl;
	buf[0] = { 0 };
	recv(socketClient, buf, sizeof(buf), 0);
	amount = atoi(buf);
	for (amount; amount > 0; amount--)
	{
		cout << "用户输入要生成订单的编号！" << endl;
		buf[0] = { 0 };
		recv(socketClient, buf, sizeof(buf), 0);
		num = atoi(buf);
		for (int j = 0; j < orders.size(); j++)
		{
			name = orders.at(j).CommodityName;
			if (orders.at(j).Ordernum == num && orders.at(j).isExist == 1 && orders.at(j).ConHost == ConUser.at(i).ID)
			{
				amout = orders.at(j).Commoditynum;
				for (int m = 0; m < object.size(); m++)
				{
					if (name == object.at(m).Name)
					{
							//若用户可以购买商品 ，修改商品库存，
							object.at(m).Amout -= amout;	//修改object中商品信息 
					}
				}
				orders.at(j).isExist = 2;
			}
		}
	}
	cout << "订单生成成功！" << endl << endl;
	return;
}

void Order::Delete(int i)		//删除购物车中的商品 
{
	int num;
	cout << "用户输入要删除的编号！" << endl;
	buf[0] = { 0 };
	recv(socketClient, buf, sizeof(buf), 0);
	num = atoi(buf);
	int flag = 0;
	while (flag == 0)
	{
		for (int j = 0; j < orders.size() && flag == 0; j++)
		{
			if (orders.at(j).Ordernum == num && orders.at(j).isExist == 1 && orders.at(j).ConHost == ConUser.at(i).ID)
			{
				flag = 1;
				orders.at(j).isExist = 0;
				cout << "商品删除成功！" << endl;
				sent = "1";
				send(socketClient, sent.c_str(), sizeof(sent), 0);
			}
		}
		if (flag == 0)
		{
			sent = "0";
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			cout << "用户输入的编号不正确，用户重新输入！" << endl;
			buf[0] = { 0 };
			recv(socketClient, buf, sizeof(buf), 0);
			num = atoi(buf);
		}
	}
	return;
}

void Order::Change(int i)	//修改购物车中商品信息
{
	int num;
	int amount;
	int money;
	cout << "用户输入要修改的编号！" << endl;
	buf[0] = { 0 };
	recv(socketClient, buf, sizeof(buf), 0);
	num = atoi(buf);

	int flag = 0;
	while (flag == 0)
	{
		for (int j = 0; j < orders.size() && flag == 0; j++)
		{
			if (orders.at(j).Ordernum == num && orders.at(j).isExist == 1 && orders.at(j).ConHost == ConUser.at(i).ID)
			{
				flag = 1;
				for (int k = 0; k < object.size(); k++)
				{
					if (object.at(k).Name == orders.at(j).CommodityName)
					{
						buf[0] = { 0 };
						itoa(object.at(k).Amout, buf, 10);
						sent = buf;
						//返回商品库存
						send(socketClient, sent.c_str(), sizeof(sent), 0);
						//接收修改数量
						buf[0] = { 0 };
						recv(socketClient, buf, sizeof(buf), 0);
						int amout;
						amout = atoi(buf);
						//修改后信息
						orders.at(j).Commoditynum = amout;
						cout << "商品数量已修改成功！" << endl;

						cout << "编号：" << orders.at(j).Ordernum << endl;
						buf[0] = { 0 };
						itoa(orders.at(j).Ordernum, buf, 10);
						sent = buf;
						send(socketClient, sent.c_str(), sizeof(sent), 0);
						Sleep(10);

						cout << "商品名称：" << orders.at(j).CommodityName << endl;
						sent = orders.at(j).CommodityName;
						send(socketClient, sent.c_str(), sizeof(sent), 0);
						Sleep(10);

						cout << "商品数量：" << orders.at(j).Commoditynum << endl;
						buf[0] = { 0 };
						itoa(orders.at(j).Commoditynum, buf, 10);
						sent = buf;
						send(socketClient, sent.c_str(), sizeof(sent), 0);
						Sleep(10);

						cout << "商品价格：" << orders.at(j).CommodityPrice << endl;
						buf[0] = { 0 };
						gcvt(orders.at(j).CommodityPrice, 6, buf);
						sent = buf;
						send(socketClient, sent.c_str(), sizeof(sent), 0);
						Sleep(10);

						cout << "商品店铺：" << orders.at(j).MerHost << endl << endl;
						sent = orders.at(j).MerHost;
						send(socketClient, sent.c_str(), sizeof(sent), 0);
						Sleep(10);


					}
				}
			}
		}
		if (flag == 0)
		{
			//返回商品编号不正确
			sent = "-1";
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			cout << "用户输入的订单编号不正确，用户重新输入！" << endl;
			buf[0] = { 0 };
			recv(socketClient, buf, sizeof(buf), 0);
			num = atoi(buf);
		}
	}
	return;
}

void Commodity::CommodityAdd(string id1)	//添加商品 
{
	string num;
	int flag = 0;
	cout << "用户输入商品编号！" << endl;
	//检查商品编号是否已存在 
	while (flag == 0)
	{
		buf[0] = { 0 };
		recv(socketClient, buf, sizeof(buf), 0);
		num = buf;
		flag = 1;
		for (int i = 0; i < object.size(); i++)
		{
			if (object.at(i).Num == num)
			{
				cout << "该编号已存在，请用户重新设置！" << endl;
				flag = 0;
				sent = "0";
				send(socketClient, sent.c_str(), sizeof(sent), 0);
			}
		}
		if (flag == 1)
		{
			sent = "1";
			send(socketClient, sent.c_str(), sizeof(sent), 0);
		}
	}
	string name = "";
	string describe = "";
	float price = 0;
	int amount = 0;
	buf[0] = { 0 };
	recv(socketClient, buf, sizeof(buf), 0);
	flag = atoi(buf);

	buf[0] = { 0 };
	recv(socketClient, buf, sizeof(buf), 0);
	name = buf;

	buf[0] = { 0 };
	recv(socketClient, buf, sizeof(buf), 0);
	price = atof(buf);

	buf[0] = { 0 };
	recv(socketClient, buf, sizeof(buf), 0);
	amount = atoi(buf);

	buf[0] = { 0 };
	recv(socketClient, buf, sizeof(buf), 0);
	describe = buf;

	if (flag == 1)
	{
		Book book;
		book.getCommodityType();
		book.Num = num;
		cout << "用户输入商品名称！" << endl;
		book.Name = name;
		cout << "用户输入商品原价！" << endl;
		book.Ocost = price;
		book.Discount = 1.0;
		cout << "用户输入商品数量！" << endl;
		book.Amout = amount;
		cout << "用户输入商品描述！" << endl;
		book.Host = id1;
		book.Describle = describe;
		object.push_back(book);	//将该商品信息存入object 
	}
	else if (flag == 2)
	{
		ElectricApp app;
		app.getCommodityType();
		app.Num = num;
		cout << "用户输入商品名称！" << endl;
		app.Name = name;
		cout << "用户输入商品原价！" << endl;
		app.Ocost = price;
		app.Discount = 1.0;
		cout << "用户输入商品数量！" << endl;
		app.Amout = amount;
		cout << "用户输入商品描述！" << endl;
		app.Describle = describe;
		app.Host = id1;
		object.push_back(app);	//将该商品信息存入object  
	}
	else if (flag == 3)
	{
		//添加服装类商品 
		Clothing clothing;
		clothing.getCommodityType();
		clothing.Num = num;
		cout << "用户输入商品名称！" << endl;
		clothing.Name = name;
		cout << "用户输入商品原价！" << endl;
		clothing.Ocost = price;
		clothing.Discount = 1.0;
		cout << "用户输入商品数量！" << endl;
		clothing.Amout = amount;
		cout << "用户输入商品描述！" << endl;
		clothing.Describle = describe;
		clothing.Host = id1;
		object.push_back(clothing);		//将该商品信息存入object 
	}
	else if (flag == 4)
	{
		//添加服装类商品 
		Food food;
		food.getCommodityType();
		food.Num = num;
		cout << "用户输入商品名称！" << endl;
		food.Name = name;
		cout << "用户输入商品原价！" << endl;
		food.Ocost = price;
		food.Discount = 1.0;
		cout << "用户输入商品数量！" << endl;
		food.Amout = amount;
		cout << "用户输入商品描述！" << endl;
		food.Describle = describe;
		food.Host = id1;
		object.push_back(food);		//该商品信息存入object
	}
	cout << "恭喜您！商品添加成功！" << endl << endl;
	return;
}

void Commodity::CommodityCheck(string id2)		//修改商品 
{
	string comtype;
	string name;
	int choice;
	int a;
	float b;
	cout << "用户选择修改方式：\n          (1)修改单件商品\n          (2)修改一类商品！" << endl;
	buf[0] = { 0 };
	recv(socketClient, buf, sizeof(buf), 0);
	choice = atoi(buf);

	if (choice == 2)
		//如果选择修改一类商品 
	{
		//输入商品类别 
		cout << "用户输入商品类别以修改商品折扣（图书/电器/服装/食品）！" << endl;
		recv(socketClient, buf, sizeof(buf), 0);
		comtype = buf;

		//输入商品折扣
		cout << "用户输入新的折扣（小数）！" << endl;

		recv(socketClient, buf, sizeof(buf), 0);
		b = atof(buf);
		if (comtype == "图书")
		{	//图书类
			for (int j = 0; j < object.size(); j++)
				if (object.at(j).Type == "图书" && object.at(j).Host == id2)
					object.at(j).Discount = b;		//修改object中商品信息 
		}
		else if (comtype == "电器")
		{	//电器类
			for (int j = 0; j < object.size(); j++)
				if (object.at(j).Type == "电器" && object.at(j).Host == id2)
					object.at(j).Discount = b;		//修改object中商品信息 
		}
		else if (comtype == "服装")
		{	//服装类 
			for (int j = 0; j < object.size(); j++)
				if (object.at(j).Type == "服装" && object.at(j).Host == id2)
					object.at(j).Discount = b;		//修改object中商品信息 
		}
		else if (comtype == "食品")
		{	//食品类 
			for (int j = 0; j < object.size(); j++)
				if (object.at(j).Type == "食品" && object.at(j).Host == id2)
					object.at(j).Discount = b;		//修改object中商品信息 
		}
		sent = "1";
		send(socketClient, sent.c_str(), sizeof(sent), 0);
		return;
	}
	else if (choice == 1)
		//若用户选择修改单件商品 
	{
		cout << "用户输入商品名称以查询想要修改的商品！" << endl;
		int flag = 0;
		//判断是否有该商品 
		buf[0] = { 0 };
		recv(socketClient, buf, sizeof(buf), 0);
		name = buf;

		for (int i = 0; i < object.size(); i++)
		{
			//若在商品库object中找到了该商品 
			if (object.at(i).Name == name && object.at(i).Host == id2)
			{
				flag = 1;
				sent = "1";
				send(socketClient, sent.c_str(), sizeof(sent), 0);

				cout << "用户选择修改方式：\n		(1)修改商品数量\n		(2)修改商品折扣！ "<< endl;
				buf[0] = { 0 };
				recv(socketClient, buf, sizeof(buf), 0);
				choice = atoi(buf);
				if (choice == 1)
				{
					//修改商品数量
					buf[0] = { 0 };
					itoa(object.at(i).Amout, buf, 10);
					sent = buf;
					send(socketClient, sent.c_str(), sizeof(sent), 0);
					cout << "当前商品数量为：" << object.at(i).Amout << endl;
					cout << "用户输入新的商品数量！" << endl;
					buf[0] = { 0 };
					recv(socketClient, buf, sizeof(buf), 0);
					a = atoi(buf);
					//更改object商品库中的商品信息 ——数量 
					object.at(i).Amout = a;
					cout << "恭喜您！商品数量修改成功！" << endl;
					cout << "目前此商品的数量为：" << object.at(i).Amout << endl;
					return;
				}
				else if (choice == 2)
				{
					//修改商品折扣 
					cout << "用户输入新的折扣（小数）！" << endl;
					recv(socketClient, buf, sizeof(buf), 0);
					b = atof(buf);
					//更改object商品库中的商品信息——折扣 
					object.at(i).Discount = b;
					float price = (object.at(i).Ocost * object.at(i).Discount);
					cout << "恭喜您！商品折扣修改成功！" << endl;
					cout << "目前此商品的折扣为：" << b << endl;
					cout << "目前此商品的价格为：" << price << endl;
					buf[0] = { 0 };
					gcvt(price, 6, buf);
					sent = buf;
					send(socketClient, sent.c_str(), sizeof(sent), 0);
					return;
				}
			}
		}
		if (flag == 0)
		{
			sent = "0";
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			cout << "该商品不存在！" << endl;
			return;
		}
	}
	else if (choice == 0)
	{
		cout << "用户输入错误" << endl;
		return;
	}
}

void Commodity::CommodityShow()		//商品展示 
{
	int choice;
	string name;
	Commodity c;
	cout << "用户选择想要查看的商品信息：\n     (1)查看所有商品信息\n     (2)查看单个商品信息！" << endl;
	buf[0] = { 0 };
	recv(socketClient, buf, sizeof(buf), 0);
	choice = atoi(buf);
	if (choice == 1)
	{
		int flag = 0;
		int i;
		for (i = 0; i < object.size(); i++)
		{
			//告诉客户端还有商品
			sent = "1";
			send(socketClient, sent.c_str(), sizeof(sent), 0);

			flag = 1;
			sent = object.at(i).Num;
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			cout << "商品编号：" << object.at(i).Num << endl;
			Sleep(10);
			sent = object.at(i).Name;
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			cout << "商品名称：" << object.at(i).Name << endl;
			Sleep(10);
			sent = object.at(i).Type;
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			cout << "商品类型：" << object.at(i).Type << endl;
			Sleep(10);
			sent = object.at(i).Host;
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			cout << "商品店铺：" << object.at(i).Host << endl;
			Sleep(10);

			buf[0] = { 0 };
			gcvt(object.at(i).Ocost, 6, buf);
			sent = buf;
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			cout << "商品原价：" << object.at(i).Ocost << endl;
			Sleep(10);
			buf[0] = { 0 };
			gcvt(object.at(i).Discount, 6, buf);
			sent = buf;
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			cout << "商品折扣：" << object.at(i).Discount << endl;
			cout << "商品现价：" << (object.at(i).Ocost * object.at(i).Discount) << endl;
			Sleep(10);
			buf[0] = { 0 };
			itoa(object.at(i).Amout, buf, 10);
			sent = buf;
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			cout << "商品数量：" << sent << endl;
			Sleep(10);

			sent = object.at(i).Describle;
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			cout << "商品描述：" << sent << endl;
			cout << endl;
			Sleep(10);
		}
		if (flag == 1 )
		{
			//告诉客户端没有商品了
			sent = "0";
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			return; //返回
		}
		else if (flag == 0)
		{
			//告诉客户端没有商品
			sent = "2";
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			return; //返回
		}
	}
	else if (choice == 2)
	{
		//接收商品的名称
		buf[0] = { 0 };
		recv(socketClient, buf, sizeof(buf), 0);
		name = buf;
		int flag = 0;
		for (int i = 0; i < object.size(); i++)
		{
			//在商品库object中通过名字寻找该商品 
			if (object.at(i).Name == name)
			{
				//告诉客户端有这个商品
				sent = "1";
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				flag = 1;

				sent = object.at(i).Num;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				cout << "商品编号：" << object.at(i).Num << endl;
				sent = object.at(i).Name;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				cout << "商品名称：" << object.at(i).Name << endl;
				sent = object.at(i).Type;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				cout << "商品类型：" << object.at(i).Type << endl;
				sent = object.at(i).Host;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				cout << "商品店铺：" << object.at(i).Host << endl;

				buf[0] = { 0 };
				gcvt(object.at(i).Ocost, 6, buf);
				sent = buf;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				cout << "商品原价：" << object.at(i).Ocost << endl;
				buf[0] = { 0 };
				gcvt(object.at(i).Discount, 6, buf);
				sent = buf;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				cout << "商品折扣：" << object.at(i).Discount << endl;
				cout << "商品现价：" << (object.at(i).Ocost * object.at(i).Discount) << endl;
				buf[0] = { 0 };
				itoa(object.at(i).Amout, buf, 10);
				sent = buf;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				cout << "商品数量：" << object.at(i).Amout << endl;

				sent = object.at(i).Describle;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				cout << "商品描述：" << object.at(i).Describle << endl;
				cout << endl;
				break;
			}
		}
		if (flag == 0)
		{
			sent = "0";
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			cout << "		用户查找的商品不存在！" << endl << endl;
		}
	}
	return;
}

void Merchant::Display(int i)		//商家管理
{
	int choice;
	int flag = 0;
	string pw1, pw2;
	Merchant m;
	Commodity c;
	Book book;
	ElectricApp electricapp;
	Clothing clothing;
	Food food;
	//向文件中写入用户和商品信息 
	m.Output();
	c.Output();
	
	cout << "     (1)个人信息\n     (2)修改密码\n     (3)商品展示\n     (4)商品管理\n     (5)退出登录\n\nChoice:";
	buf[0] = { 0 };
	recv(socketClient, buf, sizeof(buf), 0);
	choice = atoi(buf);
	if (choice == 1)
	{
		//显示商家个人信息 

		cout << "用户类型：" << MerUser.at(i).Type << endl;

		sent = MerUser.at(i).ID;
		send(socketClient, sent.c_str(), sizeof(sent), 0);
		cout << "用户的ID为：" << MerUser.at(i).ID << endl;
		sent = MerUser.at(i).Password;
		send(socketClient, sent.c_str(), sizeof(sent), 0);
		cout << "用户的密码为：" << MerUser.at(i).Password << endl;
		buf[0] = { 0 };
		gcvt(MerUser.at(i).Money, 10, buf);
		sent = buf;
		send(socketClient, sent.c_str(), sizeof(sent), 0);
		cout << "用户的余额为：" << MerUser.at(i).Money << endl;
		m.Display(i);	//返回商家管理界面
		return;
	}
	else if (choice == 2)
	{
		cout << "用户重新设置密码！" << endl;
		buf[0] = { 0 };
		recv(socketClient, buf, sizeof(buf), 0);
		MerUser.at(i).Password = buf;
		cout << "密码修改成功！" << endl;
		m.Display(i);	//返回商家管理界面
		return;
	}
	else if (choice == 3)
	{
		c.CommodityShow();	//展示商品信息 
		m.Display(i);	//返回商家管理界面
		return;			//退出商家管理界面 
	}
	else if (choice == 4)
	{
		buf[0] = { 0 };
		recv(socketClient, buf, sizeof(buf), 0);
		int choice = atoi(buf);
		if (choice == 1)
		{
			c.CommodityAdd(MerUser.at(i).ID);	//增加商品 
		}
		else if(choice == 2)
		{
			c.CommodityCheck(MerUser.at(i).ID);	//修改商品
		}
		m.Display(i);	//返回商家管理界面
		return;			//退出商家管理界面
	}
	else if (choice == 5)
	{
		cout << "用户退出登录" << endl << endl; 
		return;			//退出商家管理界面 
	}
}

void Merchant::Enter()		//商家登录 
{
	string id;
	string pwd;
	Merchant m;
	int i;
	int flag = 0;
	cout << "用户输入ID！" << endl;
	//判断ID是否存在 
	while (flag == 0)
	{
		buf[0] = { 0 };
		recv(socketClient, buf, sizeof(buf), 0);
		id = buf;
		flag = 0;
		for (i = 0; i < MerUser.size() && flag == 0; i++)
		{
			//判断是否存在该ID
			sent = "0";
			if (MerUser.at(i).ID == id)
			{
				sent = "1";
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				flag = 1;
				sent = MerUser.at(i).Password;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				cout << "用户登录成功！" << endl << endl;
				m.Display(i);	//返回商家管理界面
				return; 		//退出商家登录界面
			}
		}
		if (flag == 0)
		{
			sent = "0";
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			cout << "该ID不存在，用户重新输入:";
		}
	}
}

void Consumer::Display(int i)		//消费者管理 
{
	int choice;
	int flag = 0;
	string pw1, pw2;
	Commodity cm;
	Consumer c;
	Order order;
	//向文件中写入用户和商品信息 
	c.Output();
	cm.Output();

	buf[0] = { 0 };
	recv(socketClient, buf, sizeof(buf), 0);
	choice = atoi(buf);
	if (choice == 1)
	{
		//显示消费者个人信息 
		cout << "用户类型：" << ConUser.at(i).Type << endl;
		sent = ConUser.at(i).ID;
		send(socketClient, sent.c_str(), sizeof(sent), 0);
		Sleep(10);
		cout << "用户的ID为：" << ConUser.at(i).ID << endl;
		sent = ConUser.at(i).Password;
		send(socketClient, sent.c_str(), sizeof(sent), 0);
		Sleep(10);
		cout << "用户的密码为：" << ConUser.at(i).Password << endl;
		buf[0] = { 0 };
		gcvt(ConUser.at(i).Money, 10, buf);
		sent = buf;
		send(socketClient, sent.c_str(), sizeof(sent), 0);
		Sleep(10);
		cout << "用户的余额为：" << ConUser.at(i).Money << endl;
		c.Display(i);	//返回消费者管理界面
		return;
	}
	else if (choice == 2)
	{
		cout << "用户重新设置密码！" << endl;
		buf[0] = { 0 };
		recv(socketClient, buf, sizeof(buf), 0);
		ConUser.at(i).Password = buf;
		cout << "密码修改成功！" << endl;
		c.Display(i);	//返回消费者管理界面
		return;
	}
	else if (choice == 3)
	{
		cm.CommodityShow();	//展示商品信息 
		c.Display(i);	//返回消费者管理界面
		return;
	}
	else if (choice == 4)
	{
		int money;
		cout << "用户输入您要充值的金额！" << endl;
		buf[0] = { 0 };
		recv(socketClient, buf, sizeof(buf), 0);
		money = atof(buf);
		cout << "余额修改成功！" << endl;
		ConUser.at(i).Money += money;	//修改消费者信息库ConUser中的信息 
		cout << "目前用户的余额为：" << ConUser.at(i).Money << endl;
		buf[0] = { 0 };
		gcvt(ConUser.at(i).Money, 10, buf);
		sent = buf;
		send(socketClient, sent.c_str(), sizeof(sent), 0);
		Sleep(10);
		c.Display(i);	//返回消费者管理界面
		return;
	}
	else if (choice == 5)
	{
		cout << "用户输入要购买的商品名称！" << endl;
		string name;
		//接收商品名称
		buf[0] = { 0 };
		recv(socketClient, buf, sizeof(buf), 0);
		name = buf;
		for (int j = 0; j < object.size(); j++)
		{
			//判断是否存在该商品名称
			if (object.at(j).Name == name)
			{
				sent = "1";
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				Sleep(10);
				//发送商品数量
				buf[0] = { 0 };
				itoa(object.at(j).Amout, buf, 10);
				sent = buf;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				Sleep(10);
				//接收库存是否充足
				buf[0] = { 0 };
				recv(socketClient, buf, sizeof(buf), 0);
				choice = atoi(buf);
				if (choice == 0)
				{	//库存不足
					c.Display(i);	//返回消费者管理界面
					return;
				}
				else
				{
					//接收选项
					buf[0] = { 0 };
					recv(socketClient, buf, sizeof(buf), 0);
					choice = atoi(buf);
					if (choice == 1)
					{
						float price;
						//发送价格
						buf[0] = { 0 };
						price = object.at(j).Discount * object.at(j).Ocost;
						gcvt(price, 6, buf);
						sent = buf;
						send(socketClient, sent.c_str(), sizeof(sent), 0);
						Sleep(10);
						//发送余额
						float money;
						buf[0] = { 0 };
						money = ConUser.at(i).Money;
						gcvt(money, 6, buf);
						sent = buf;
						send(socketClient, sent.c_str(), sizeof(sent), 0);
						Sleep(10);
						//接收消息
						buf[0] = { 0 };
						recv(socketClient, buf, sizeof(buf), 0);
						choice = atoi(buf);
						if (choice == -1)
						{
							cout << "用户余额不足，无法购买！" << endl;
							c.Display(i);	//返回消费者管理界面
							return;
						}
						else
						{
							cout << "用户完成购买！" << endl;
							int amout = choice;
							object.at(j).Amout -= amout;	//修改object中商品信息
							money = amout * price;
							ConUser.at(i).Money -= money;
							for (int k = 0; k < MerUser.size(); k++)
							{
								if (object.at(j).Host == MerUser.at(k).ID)
								{
									MerUser.at(k).Money += money;
									break;
								}
							}
							c.Display(i);	//返回消费者管理界面
							return;
						}
					}
					else if (choice == 2)
					{
						//接收购买数量
						buf[0] = { 0 };
						recv(socketClient, buf, sizeof(buf), 0);
						int amout = atoi(buf);
						//设置购物车商品信息
						order.ConHost = ConUser.at(i).ID;
						order.MerHost = object.at(j).Host;
						order.Commoditynum = amout;
						order.isExist = 1;
						order.CommodityName = name;
						order.CommodityPrice = object.at(j).Discount * object.at(j).Ocost;
						order.Ordernum = orders.size() + 1;
						orders.push_back(order);
						cout << "用户已加入到购物车中！" << endl << endl;
						c.Display(i);	//返回消费者管理界面
						return;
					}
				}
			}
		}
		sent = "0";
		send(socketClient, sent.c_str(), sizeof(sent), 0);
		Sleep(10);
		cout << "该商品名称不存在！" << endl;
		c.Display(i);	//返回消费者管理界面
		return;
	}
	else if (choice == 6)
	{
		flag = 0;
		cout << "用户购物车展示如下：" << endl;
		for (int j = 0; j < orders.size(); j++)
		{
			if (orders.at(j).ConHost == ConUser.at(i).ID && orders.at(j).isExist == 1)
			{
				flag = 1;
				//发送1说明有商品
				sent = "1";
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				Sleep(100);
				//发送编号
				buf[0] = { 0 };
				itoa(orders.at(j).Ordernum, buf, 10);
				sent = buf;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				Sleep(10);
				cout << "编号：" << orders.at(j).Ordernum << endl;
				//发送名称
				sent = orders.at(j).CommodityName;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				Sleep(10);
				cout << "商品名称：" << orders.at(j).CommodityName << endl;
				//发送数量
				buf[0] = { 0 };
				itoa(orders.at(j).Commoditynum, buf, 10);
				sent = buf;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				Sleep(10); 
				cout << "商品数量：" << orders.at(j).Commoditynum << endl;
				//发送价格
				buf[0] = { 0 };
				gcvt(orders.at(j).CommodityPrice, 6, buf);
				sent = buf;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				Sleep(10); 
				cout << "商品价格：" << orders.at(j).CommodityPrice << endl;
				//商品店铺
				sent = orders.at(j).MerHost;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				Sleep(10); 
				cout << "商品店铺：" << orders.at(j).MerHost << endl << endl;
			}
		}
		if (flag == 0)
		{
			sent = "0";
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			Sleep(10);
			cout << "用户的购物车为空！" << endl << endl;
			c.Display(i);	//返回消费者管理界面
			return;			//退出消费者管理界面
		}
		else
		{
			//后面没有商品了
			sent = "0";
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			Sleep(10);
			//接收用户选项
			buf[0] = { 0 };
			recv(socketClient, buf, sizeof(buf), 0);
			cout << buf << endl;
			int choice = atoi(buf);
			if (choice == 1)
			{
				cout << "生成订单" << endl;
				order.Pay(i);			//生成订单
			}
			else if (choice == 2)
			{
				cout << "删除商品" << endl;
				order.Delete(i);		//删除商品
			}
			else if (choice == 3)
			{
				cout << "修改商品" << endl;
				order.Change(i);		//修改商品
			}

			c.Display(i);
			return;
		}
	}
	else if (choice == 7)
	{
		flag = 0;
		cout << "用户生成的订单展示如下：" << endl;
		for (int j = 0; j < orders.size(); j++)
		{
			if (orders.at(j).ConHost == ConUser.at(i).ID && orders.at(j).isExist == 2)
			{
				flag = 1;
				//发送1说明有商品
				sent = "1";
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				Sleep(100);
				//发送编号
				buf[0] = { 0 };
				itoa(orders.at(j).Ordernum, buf, 10);
				sent = buf;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				Sleep(10);
				cout << "编号：" << orders.at(j).Ordernum << endl;
				//发送名称
				sent = orders.at(j).CommodityName;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				Sleep(10);
				cout << "商品名称：" << orders.at(j).CommodityName << endl;
				//发送数量
				buf[0] = { 0 };
				itoa(orders.at(j).Commoditynum, buf, 10);
				sent = buf;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				Sleep(10);
				cout << "商品数量：" << orders.at(j).Commoditynum << endl;
				//发送价格
				buf[0] = { 0 };
				gcvt(orders.at(j).CommodityPrice, 6, buf);
				sent = buf;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				Sleep(10);
				cout << "商品价格：" << orders.at(j).CommodityPrice << endl;
				//商品店铺
				sent = orders.at(j).MerHost;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				Sleep(10);
				cout << "商品店铺：" << orders.at(j).MerHost << endl << endl;
			}
		}
		if (flag == 0)
		{
			sent = "0";
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			Sleep(10);
			cout << "用户生成的订单为空！" << endl << endl;
			c.Display(i);	//返回消费者管理界面
			return;			//退出消费者管理界面
		}
		else
		{
			//后面没有商品了
			sent = "0";
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			Sleep(10);
			//接收用户选项
			buf[0] = { 0 };
			recv(socketClient, buf, sizeof(buf), 0);
			choice = atoi(buf);
			if (choice == 1)
			{
				order.Payment(i);		//支付订单
			}
			c.Display(i);	//返回消费者管理界面
			return;			//退出消费者管理界面
		}
	}
	else if (choice == 8)
	{
		cout << "		退出登录" << endl << endl;
		return;	//退出消费者管理界面 
	}
}

void Consumer::Enter()		//消费者登录 
{
	string id;
	string pwd;
	Consumer c;
	int i;
	int flag = 0;

	while (flag == 0)
	{
		buf[0] = { 0 };
		recv(socketClient, buf, sizeof(buf), 0);
		id = buf;
		flag = 0;
		for (i = 0; i < ConUser.size() && flag == 0; i++)
		{
			//判断是否存在该ID
			sent = "0";
			if (ConUser.at(i).ID == id)
			{
				sent = "1";
				flag = 1;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				sent = ConUser.at(i).Password;
				send(socketClient, sent.c_str(), sizeof(sent), 0);
				cout << "		登录成功！" << endl << endl;
				c.Display(i);	//返回消费者管理界面
				return; 		//退出消费者管理界面
			}
		}
		if (flag == 0)
		{
			sent = "0";
			send(socketClient, sent.c_str(), sizeof(sent), 0);
			cout << "该ID不存在，用户重新输入:";
		}
	}
}

void Merchant::Register()		//商家注册 
{
	Merchant m;
	int flag = 0;
	cout << "用户设置ID！" << endl;
	buf[0] = { 0 };
	while (!flag)
	{
		buf[0] = { 0 };
		recv(socketClient, buf, sizeof(buf), 0);
		m.ID = buf;
		flag = 1;
		//检查ID是否已经存在 
		sent = "1";
		for (int i = 0; i < MerUser.size(); i++)
			if (MerUser.at(i).ID == m.ID)
			{
				cout << "该ID已存在，用户重新设置:";
				sent = "0";
				flag = 0;
			}
		for (int j = 0; j < ConUser.size(); j++)
			if (ConUser.at(j).ID == m.ID)
			{
				cout << "该ID已存在，用户重新设置:";
				sent = "0";
				flag = 0;
			}
		send(socketClient, sent.c_str(), sizeof(sent), 0);
	}
	cout << "用户设置密码！" << endl;

	//检查两次输入的密码是否相同 
	buf[0] = { 0 };
	recv(socketClient, buf, sizeof(buf), 0);
	m.Password = buf;		//设置密码 
	m.Money = 0.0;			//初始化余额
	m.getUserType();		//获取该用户类型
	MerUser.push_back(m);	//保存用户信息
	cout << "商家用户注册成功！" << endl;
	return;
}

void Consumer::Register()		//消费者注册 
{
	Consumer c;
	int flag = 0;

	while (!flag)
	{
		buf[0] = { 0 };
		recv(socketClient, buf, sizeof(buf), 0);
		c.ID = buf;
		flag = 1;
		//检查ID是否已经存在
		sent = "1";
		for (int i = 0; i < MerUser.size(); i++)
			if (MerUser.at(i).ID == c.ID)
			{
				sent = "0";
				cout << "该ID已存在，用户重新设置:";
				flag = 0;
			}
		for (int j = 0; j < ConUser.size(); j++)
			if (ConUser.at(j).ID == c.ID)
			{
				sent = "0";
				cout << "该ID已存在，用户重新设置:";
				flag = 0;
			}
		send(socketClient, sent.c_str(), sizeof(sent), 0);
	}
	cout << "用户设置密码！" << endl;

	//检查两次输入的密码是否相同 
	buf[0] = { 0 };
	recv(socketClient, buf, sizeof(buf), 0);
	c.Password = buf;		//设置密码 
	c.Money = 0.0;			//初始化余额
	c.getUserType();		//获取该用户类型
	ConUser.push_back(c);	//保存用户信息
	cout << "消费者用户注册成功！" << endl;
	return;
}




void Merchant::Input()		//将文件中的信息读入到Merchant库 
{
	int count;
	Merchant m;
	string temp;
	ifstream filein("merchant.txt");
	if (!filein.is_open())	//若文件打不开 
	{
		cout << "文件打开错误! " << endl;
		return;
	}
	getline(filein, temp);
	count = atoi(temp.c_str());
	for (int j = 0; j < count; j++)
	{
		getline(filein, m.Type);
		getline(filein, m.ID);
		getline(filein, m.Password);
		getline(filein, temp);
		m.Money = atof(temp.c_str());
		MerUser.push_back(m);
	}
	filein.close();
}

void Consumer::Input()		//将文件中的信息读入到Consumer库 
{
	int count;
	Consumer c;
	string temp;
	ifstream filein("consumer.txt");
	if (!filein.is_open())	//若文件打不开 
	{
		cout << "文件打开错误! " << endl;
		return;
	}
	getline(filein, temp);
	count = atoi(temp.c_str());		//存储的信息个数 
	for (int j = 0; j < count; j++)
	{
		getline(filein, c.Type);
		getline(filein, c.ID);
		getline(filein, c.Password);
		getline(filein, temp);
		c.Money = atof(temp.c_str());
		ConUser.push_back(c);
	}
	filein.close();
}

void Merchant::Output()	//将Merchant库中信息输出到文件中
{
	ofstream mycout("merchant.txt");
	if (!mycout.is_open())	//若文件打不开 
	{
		cout << "文件打开错误! " << endl;
		return;
	}
	mycout << MerUser.size() << endl;	//存储的信息个数 
	for (int i = 0; i < MerUser.size(); i++)
	{
		mycout << MerUser.at(i).Type << endl;
		mycout << MerUser.at(i).ID << endl;
		mycout << MerUser.at(i).Password << endl;
		mycout << MerUser.at(i).Money << endl;
	}
	mycout.close();
}

void Consumer::Output()	//将Consumer库中信息输出到文件中
{
	ofstream mycout("consumer.txt");
	if (!mycout.is_open())	//若文件打不开 
	{
		cout << "文件打开错误! " << endl;
		return;
	}
	mycout << ConUser.size() << endl;	//存储的信息个数 
	for (int i = 0; i < ConUser.size(); i++)
	{
		mycout << ConUser.at(i).Type << endl;
		mycout << ConUser.at(i).ID << endl;
		mycout << ConUser.at(i).Password << endl;
		mycout << ConUser.at(i).Money << endl;
	}
	mycout.close();
}

void Commodity::Input()		//将文件中的信息读入到Commodity库 
{
	int count;
	Commodity c;
	ifstream filein("object.txt");
	if (!filein.is_open())	//若文件打不开 
	{
		cout << "文件打开错误! " << endl;
		return;
	}
	string temp;
	getline(filein, temp);
	count = atoi(temp.c_str());		//存储的信息个数 
	for (int j = 0; j < count; j++)
	{
		getline(filein, c.Num);
		getline(filein, c.Name);
		getline(filein, c.Type);
		getline(filein, c.Host);
		getline(filein, temp);
		c.Ocost = atof(temp.c_str());
		getline(filein, temp);
		c.Discount = atof(temp.c_str());
		getline(filein, temp);
		c.Amout = atoi(temp.c_str());
		getline(filein, c.Describle);
		object.push_back(c);
	}
	filein.close();
}

void Commodity::Output()	//将Commodity库中信息输出到文件中
{
	ofstream fileout("object.txt");
	if (!fileout.is_open())	//若文件打不开 
	{
		cout << "文件打开错误! " << endl;
		return;
	}
	fileout << object.size() << endl;		//存储的信息个数 
	for (int i = 0; i < object.size(); i++)
	{
		fileout << object.at(i).Num << endl;
		fileout << object.at(i).Name << endl;
		fileout << object.at(i).Type << endl;
		fileout << object.at(i).Host << endl;
		fileout << object.at(i).Ocost << endl;
		fileout << object.at(i).Discount << endl;
		fileout << object.at(i).Amout << endl;
		fileout << object.at(i).Describle << endl;
	}
	fileout.close();
}


int main(void)
{
	//打开网络库
	WORD wdVersion = MAKEWORD(2, 2);
	WSADATA wdSockMsg;
	//成功打开则返回0
	WSAStartup(wdVersion, &wdSockMsg);

	//创建服务器Socket
	socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//创建失败
	if (INVALID_SOCKET == socketServer)
	{
		//关闭网络库
		WSACleanup();
		return 0;
	}

	//绑定地址与端口——bind
	struct sockaddr_in si;
	si.sin_family = AF_INET;//地址类型
	si.sin_port = htons(12345);//端口号（htons整型转无符号short）（端口号0-65535但0-1023是为系统保留的，不能用）
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//IP地址 127.0.0.1本机地址，在本电脑内部传递数据

	if (SOCKET_ERROR == bind(socketServer, (const struct sockaddr*)&si, sizeof(si)))
	{
		//执行出错
		//销毁套接字
		closesocket(socketServer);
		//关闭网络库
		WSACleanup();
		return 0;
	}

	if (SOCKET_ERROR == listen(socketServer, SOMAXCONN))		//开始监听
	{
		//销毁套接字
		closesocket(socketServer);
		//关闭网络库
		WSACleanup();
		return 0;
	}

	//创建客户端连接
	struct sockaddr_in clientMsg;
	int len = sizeof(clientMsg);//客户端大小
	//Accept函数创建客户端Socket
	//服务器socket、客户端地址端口信息结构体、客户端大小——结构体大小
	socketClient = accept(socketServer, (struct sockaddr*)&clientMsg, &len);

	if (INVALID_SOCKET == socketClient)
	{
		//销毁套接字
		closesocket(socketServer);
		//关闭网络库
		WSACleanup();
		return 0;
	}
	//开始与客服端通信

	//与客户端收发消息
	//recv(socketClient, buf, 1499, 0);
	//返回值为实际读出来的字节大小
	//客户端的socket、字符数组（消息、最大传输单元）、数组长度、数据读取方式（填0）
	//send(socketClient, "Yes", sizeof("Yes"), 0);
	//服务器的socke、const(消息)、数组长度、数据读取方式（填0）


	int choice;
	string type;
	Merchant merchant;
	Consumer consumer;
	Commodity commodity;
	//从文件中读入用户和商品信息 
	merchant.Input();
	consumer.Input();
	commodity.Input();
	//展示平台系统 
	while (1)
	{
		buf[0] = { 0 };
		recv(socketClient, buf, sizeof(buf), 0);
		choice = atoi(buf);
		if (choice == 11)
		{
			cout << "商家注册" << endl;
			merchant.Register();	//商家注册
			//向文件中写入信息
			merchant.Output();
			consumer.Output();
			commodity.Output();
		}
		else if (choice == 12)
		{
			cout << "消费者注册" << endl;
			consumer.Register();	//消费者注册 
			//向文件中写入信息
			merchant.Output();
			consumer.Output();
			commodity.Output();
		}
		else if (choice == 21)
		{
			cout << "商家登录" << endl;
			merchant.Enter();	//商家登录 
		}
		else if (choice == 22)
		{
			cout << "消费者登录" << endl;
			consumer.Enter();	//消费者登录 
		}
		else if (choice == 3)
		{
			//展示商品信息
			cout << "展示商品信息" << endl;
			commodity.CommodityShow();
		}
		else if (choice == 4)
		{
			cout << "退出系统" << endl;
			//退出平台系统 
			exit(EXIT_FAILURE);
		}
	}


	//销毁套接字
	closesocket(socketClient);
	closesocket(socketServer);
	//关闭网络库
	WSACleanup();

	system("pause");
	return 0;
}