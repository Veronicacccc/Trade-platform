//添加c++语言库
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <string>
//添加网络头文件
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

char buf[1500];	//接收内容缓存区
string sent;	//发送内容缓存区
SOCKET socketServer;//全局变量服务端socket


void OrderPayment()//用户支付订单
{
	int num;
	int money;
	int choice;
	cout << "请输入您要支付订单的编号：";
	cin >> num;
	buf[0] = { 0 };
	itoa(num, buf, 10);
	sent = buf;
	send(socketServer, sent.c_str(), sizeof(sent), 0);
	int flag = 0;
	while (flag == 0)
	{
		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		choice = atoi(buf);
		if (choice == 1)
		{
			flag = 1;
			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			choice = atoi(buf);
			if (choice == 2)
			{
				cout << "您的余额不足，支付失败！" << endl;
				return;
			}
			else if (choice == 3)
			{
				cout << "支付成功！" << endl;
				buf[0] = { 0 };
				recv(socketServer, buf, sizeof(buf), 0);
				money = atof(buf);
				cout << "您的余额为：" << money << endl;
				return;	//返回消费者管理界面
			}
		}
		else if (choice == 0)
		{
			flag = 0;
			cout << "您输入的订单编号不正确，请重新输入！" << endl;
			cin >> num;
			itoa(num, buf, 10);
			sent = buf;
			send(socketServer, sent.c_str(), sizeof(sent), 0);
		}
	}
}

void OrderPay()//用户生成订单
{
	int num;
	int money;
	int amount;
	cout << "请输入您要生成订单的个数：";
	cin >> amount;
	buf[0] = { 0 };
	itoa(amount, buf, 10);
	sent = buf;
	send(socketServer, sent.c_str(), sizeof(sent), 0);
	for (amount; amount > 0; amount--)
	{
		cout << "请输入您要生成订单的编号：";
		cin >> num;
		buf[0] = { 0 };
		itoa(num, buf, 10);
		sent = buf;
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		Sleep(10);
	}
	cout << "订单生成成功！" << endl << endl;
	return;
}

void OrderDelete()		//删除购物车中的商品 
{
	int num;
	int choice;
	cout << "请输入您要删除的编号：" << endl;
	cin >> num;
	buf[0] = { 0 };
	itoa(num, buf, 10);
	sent = buf;
	send(socketServer, sent.c_str(), sizeof(sent), 0);
	int flag = 0;
	while (flag == 0)
	{
		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		choice = atoi(buf);
		if (choice == 0)
		{
			flag = 0;
			cout << "您输入的编号不正确，请重新输入！" << endl;
			cin >> num;
			buf[0] = { 0 };
			itoa(num, buf, 10);
			sent = buf;
			send(socketServer, sent.c_str(), sizeof(sent), 0);
		}
		else if (choice == 1)
		{
			flag = 1;
			cout << "商品删除成功！" << endl;
		}
	}
	return;
}

void OrderChange()
{
	int choice;
	int num;
	int storenum;
	cout << "请输入您要修改的编号：" << endl;
	cin >> num;
	buf[0] = { 0 };
	itoa(num, buf, 10);
	sent = buf;
	send(socketServer, sent.c_str(), sizeof(sent), 0);

	int flag = 0;
	while (flag == 0)
	{
		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		storenum = atoi(buf);
		if (storenum != -1)
		{
			flag = 1;
			cout << "请输入您要修改的商品数量：";
		flag2:
			int amout;
			cin >> amout;
			if (amout > storenum)
			{
				//若商品库存不够
				cout << "您选择的数量大于商品库存" << endl;
				cout << "商品库存为：" << storenum << endl;
				cout << "请您重新选择要购买的商品数量：";
				goto flag2;
			}
			buf[0] = { 0 };
			itoa(amout, buf, 10);
			sent = buf;
			send(socketServer, sent.c_str(), sizeof(sent), 0);


			cout << "商品数量已修改成功！" << endl;
			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			num = atoi(buf);
			cout << "编号：" << num << endl;

			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			cout << "商品名称：" << buf << endl;
			
			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			amout = atoi(buf);
			cout << "商品数量：" << amout << endl;
			
			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			float price;
			price = atof(buf); 
			cout << "商品价格：" << price << endl;
			
			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			cout << "商品店铺：" << buf << endl << endl;

		}
		else if (storenum == -1)
		{
			flag = 0;
			cout << "您输入的订单编号不正确，请重新输入！" << endl;
			cin >> num;
			buf[0] = { 0 };
			itoa(num, buf, 10);
			sent = buf;
			send(socketServer, sent.c_str(), sizeof(sent), 0);
		}
	}
	return; 
}

void CommodityCommodityAdd()	//添加商品 
{
	string num;
	string type;
	int choice = 0;
	int flag = 0;
	cout << "请输入商品编号：";
	cin >> num;
	sent = num;
	send(socketServer, sent.c_str(), sizeof(sent), 0);

	while (flag == 0)
	{
		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		choice = atoi(buf);
		if (choice == 0)
		{
			cout << "该编号已存在，请重新设置：";
			cin >> num;
			sent = num;
			send(socketServer, sent.c_str(), sizeof(sent), 0);
		}
		else if (choice == 1)
			flag = 1;
	}

	flag = 0;
	while (flag == 0)
	{
		cout << "请输入商品类型（图书/电器/服装/食品）：";
		cin >> type;
		if (type == "图书")
		{
			flag = 1;
			sent = "1";
			send(socketServer, sent.c_str(), sizeof(sent), 0);
		}
		else if (type == "电器")
		{
			flag = 1;
			sent = "2";
			send(socketServer, sent.c_str(), sizeof(sent), 0);
		}
		else if (type == "服装")
		{
			flag = 1;
			sent = "3";
			send(socketServer, sent.c_str(), sizeof(sent), 0);
		}
		else if (type == "食品")
		{
			flag = 1;
			sent = "4";
			send(socketServer, sent.c_str(), sizeof(sent), 0);
		}
		else
		{
			cout << "请规范您的输入！" << endl;
			flag = 0;
		}
	}
	cout << "请输入商品名称：";
	string name;
	cin >> name;
	sent = name;
	send(socketServer, sent.c_str(), sizeof(sent), 0);

	cout << "请输入商品原价：";
	string price;
	cin >> price;
	sent = price;
	send(socketServer, sent.c_str(), sizeof(sent), 0);

	cout << "请输入商品数量：";
	string amount;
	cin >> amount;
	sent = amount;
	send(socketServer, sent.c_str(), sizeof(sent), 0);

	cout << "请输入商品描述：";
	string describe;
	cin >> describe;
	sent = describe;
	send(socketServer, sent.c_str(), sizeof(sent), 0);
	cout << "恭喜您！商品添加成功！" << endl << endl;
	return;
}

void CommodityCommodityCheck()		//修改商品 
{
	string comtype;
	int choice;
	cout << "请选择您的修改方式：\n          (1)修改单件商品\n          (2)修改一类商品\n\nChoice:";
	cin >> choice;
	if (choice == 2)
	{
		//如果选择修改一类商品
		sent = "2";
		send(socketServer, sent.c_str(), sizeof(sent), 0);

		//输入商品类别 
		cout << "请输入商品类别以修改商品折扣（图书/电器/服装/食品）：";
		cin >> comtype;
		//判断是否有该商品类别
		while (comtype != "图书" && comtype != "电器" && comtype != "服装" && comtype != "食品")
		{
			cout << "请规范您的输入（图书/电器/服装/食品）：";
			cin >> comtype;
		}
		sent = comtype;
		send(socketServer, sent.c_str(), sizeof(sent), 0);

		//输入商品折扣
		cout << "请输入新的折扣（小数）：";
		float b;
		cin >> b;
		buf[0] = { 0 };
		gcvt(b, 6, buf);
		sent = buf;
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		
		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		choice = atoi(buf);
		if (choice == 1)
		{
			cout << "恭喜您！该类商品折扣修改成功！" << endl;
		}
		return;
	}
	else if (choice == 1)
	{
		//若用户选择修改单件商品 
		sent = "1";
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		cout << "请输入商品名称以查询想要修改的商品：";
		int flag = 0;
		//判断是否有该商品
		cin >> comtype;
		sent = comtype;
		send(socketServer, sent.c_str(), sizeof(sent), 0);

		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		choice = atoi(buf);

		if (choice == 1)
		{
			cout << "请选择您的修改方式：\n		(1)修改商品数量\n		(2)修改商品折扣\n\nChoice：";
			cin >> choice;
			if (choice == 1)
			{
				sent = "1";
				send(socketServer, sent.c_str(), sizeof(sent), 0);
				//修改商品数量 
				buf[0] = { 0 };
				recv(socketServer, buf, sizeof(buf), 0);
				int num;
				num = atoi(buf);
				cout << "当前商品数量为：" << num << endl;
				cout << "请输入新的商品数量：";
				int a;
				cin >> a;
				buf[0] = { 0 };
				itoa(a, buf, 10);
				sent = buf;
				send(socketServer, sent.c_str(), sizeof(sent), 0);
				//更改object商品库中的商品信息 ——数量 

				cout << "恭喜您！商品数量修改成功！" << endl;
				cout << "目前此商品的数量为：" << a << endl;
				return;
			}
			else if (choice == 2)
			{
				sent = "2";
				send(socketServer, sent.c_str(), sizeof(sent), 0);
				//修改商品折扣 
				cout << "请输入新的折扣（小数）：";
				float b;
				cin >> b;
				buf[0] = { 0 };
				gcvt(b, 6, buf);
				sent = buf;
				send(socketServer, sent.c_str(), sizeof(sent), 0);
				float price;
				buf[0] = { 0 };
				recv(socketServer, buf, sizeof(buf), 0);
				price = atof(buf);
				cout << "恭喜您！商品折扣修改成功！" << endl;
				cout << "目前此商品的折扣为：" << b << endl;
				cout << "目前此商品的价格为：" << price << endl;
				return;
			}
		}
		else
		{
			cout << "该商品名称不存在！" << endl;
			return;
		}
	}
	else
	{	//用户输入错误
		sent = "0";
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		return;
	}
}

void CommodityCommodityShow()		//商品展示 
{
	int choice;
	string name;
	cout << "请选择您想要查看的商品信息：\n     (1)查看所有商品信息\n     (2)查看单个商品信息\n\nChoice：";
	cin >> choice;

	if (choice == 1)
	{
		//请求返回所有商品信息
		sent = "1";
		send(socketServer, sent.c_str(), sizeof(sent), 0);

		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		choice = atoi(buf);
		while (choice == 1)
		{
			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			cout << "商品编号：" << buf << endl;

			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			cout << "商品名称：" << buf << endl;

			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			cout << "商品类型：" << buf << endl;

			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			cout << "商品店铺：" << buf << endl;

			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			float Ocost;
			Ocost = atof(buf);
			cout << "商品原价：" << Ocost << endl;
			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			float Discount;
			Discount = atof(buf);
			cout << "商品折扣：" << Discount << endl;
			
			cout << "商品现价：" << (Ocost * Discount) << endl;
			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			int Amout = atoi(buf);
			cout << "商品数量：" << Amout << endl;
			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			cout << "商品描述：" << buf << endl << endl;

			//接收消息，判断是否还有商品
			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			choice = atoi(buf);
		}
		if (choice == 2)
		{
			cout << "		商品库为空！" << endl;
			return;	//返回
		}
		else if (choice == 0)
		{
			return;	//返回
		}
	}
	else if (choice == 2)
	{
		//请求返回单个商品信息
		sent = "2";
		send(socketServer, sent.c_str(), sizeof(sent), 0);

		cout << "请输入您想要查看的商品名称：";
		int flag = 0;
		//检查该商品是否存在
		cin >> name;
		sent = name;
		send(socketServer, sent.c_str(), sizeof(sent), 0);

		//接收消息，判断是否有这个商品
		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		choice = atoi(buf);
		if (choice == 1)
		{
			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			cout << "商品编号：" << buf << endl;
			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			cout << "商品名称：" << buf << endl;
			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			cout << "商品类型：" << buf << endl;
			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			cout << "商品店铺：" << buf << endl;

			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			float Ocost = atof(buf);
			cout << "商品原价：" << Ocost << endl;
			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			float Discount = atof(buf);
			cout << "商品折扣：" << Discount << endl;

			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			int Amout = atoi(buf);
			cout << "商品数量：" << Amout << endl;
			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			cout << "商品描述：" << buf << endl << endl;
		}
		else if (choice == 0)
		{
			cout << "		不存在这个商品" << endl;
		}
	}
	return;
}

void MerchantDisplay()		//商家管理
{
	int choice;
	int flag = 0;
	string pw1, pw2;
	cout << "     (1)个人信息\n     (2)修改密码\n     (3)商品展示\n     (4)商品管理\n     (5)退出登录\n\nChoice:";
	cin >> choice;
	if (choice == 1)
	{
		sent = "1";
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		//展示商家信息
		//显示消费者个人信息 
		cout << "用户类型：商家" << endl;
		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		string ID = buf;
		cout << "您的ID为：" << ID << endl;
		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		string Password = buf;
		cout << "您的密码为：" << Password << endl;
		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		float Money = atof(buf);
		cout << "您的余额为：" << Money << endl;
		MerchantDisplay();	//返回商家管理界面
		return;
	}
	else if (choice == 2)
	{
		sent = "2";
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		//重新设置密码

		cout << "请重新设置您的密码：";
		flag = 0;
		//检查两次输入密码是否相同
		while (!flag)
		{
			cin >> pw1;
			cout << "再次确认您的密码：";
			cin >> pw2;
			if (pw1 != pw2)
				cout << "前后两次输入密码不一致，请重新设置密码:";
			else
				flag = 1;
		}
		sent = pw1;
		send(socketServer, sent.c_str(), sizeof(sent), 0);

		cout << "密码修改成功！" << endl;
		MerchantDisplay();	//返回商家管理界面
		return;
	}
	else if (choice == 3)
	{
		sent = "3";
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		//展示商品信息

		CommodityCommodityShow();
		MerchantDisplay();	//返回商家管理界面 
	}
	else if (choice == 4)
	{
		sent = "4";
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		//修改商品

		int way;
		cout << "     (1)增加商品\n     (2)修改商品\n\nChoice:";
		cin >> way;
		if (way == 1)
		{
			sent = "1";
			send(socketServer, sent.c_str(), sizeof(sent), 0);
			CommodityCommodityAdd();	//增加商品 
		} 
		else if (way == 2)
		{
			sent = "2";
			send(socketServer, sent.c_str(), sizeof(sent), 0);
			CommodityCommodityCheck();	//修改商品
		}
		MerchantDisplay();	//返回商家管理界面
		return;			//退出商家管理界面
	}
	else 
	{
		sent = "5";
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		cout << "退出登录" << endl << endl;
		return;	//退出商家管理界面 
	}
}

void MerchantEnter()		//商家登录 
{
	string id;
	string pwd;
	int i = 0;
	int flag = 0;
	cout << "请输入您的ID：";
	//判断ID是否存在 
	while (flag == 0)
	{
		cin >> id;
		sent = id;
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		flag = 0;
		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		int choice = atoi(buf);
		//判断是否存在该ID
		if (choice == 1)
		{
			cout << "请输入您的密码：";
			flag = 1;
			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			while (flag == 1)
			{
				//判断两次密码是否相同 
				cin >> pwd;
				flag = 2;
				if (buf != pwd)
				{
					cout << "密码错误，请重新输入:";
					flag = 1;
				}
				else if (buf == pwd)
				{
					cout << "		登录成功！" << endl << endl;
					MerchantDisplay();	//返回商家管理界面
					return; 		//退出商家登录界面
				}
			}
		}
		else
			cout << "该ID不存在，请重新输入:";
	}
}

void ConsumerDisplay()		//消费者管理 
{
	int choice;
	int flag = 0;
	string pw1, pw2;
	cout << "     (1)个人信息\n     (2)修改密码\n     (3)商品展示\n     (4)余额充值\n";
	cout << "     (5)购买商品\n     (6)查看购物车\n     (7)查看订单\n     (8)退出登录\n\nChoice:";
	cin >> choice;
	if (choice == 1)
	{
		sent = "1";
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		Sleep(10);
	
		//显示消费者个人信息 
		cout << "用户类型：消费者" << endl;
		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		string ID = buf;
		cout << "您的ID为：" << ID << endl;
		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		string Password = buf;
		cout << "您的密码为：" << Password << endl;
		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		float Money = atof(buf);
		cout << "您的余额为：" << Money << endl;
		ConsumerDisplay();	//返回消费者管理界面
		return;
	}
	else if (choice == 2)
	{
		sent = "2";
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		Sleep(10);

		cout << "请重新设置您的密码：";
		flag = 0;
		//检查两次输入密码是否相同
		while (!flag)
		{
			cin >> pw1;
			cout << "再次确认您的密码：";
			cin >> pw2;
			if (pw1 != pw2)
				cout << "前后两次输入密码不一致，请重新设置密码:";
			else
				flag = 1;
		}
		sent = pw1;
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		Sleep(10);
		cout << "密码修改成功！" << endl;
		ConsumerDisplay();	//返回消费者管理界面
		return;
	}
	else if (choice == 3)
	{
		sent = "3";
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		Sleep(10);

		CommodityCommodityShow();	//展示商品信息 
		ConsumerDisplay();	//返回消费者管理界面
		return;
	}
	else if (choice == 4)
	{
		sent = "4";
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		Sleep(10);
		//用户充值

		int money;
		cout << "请输入您要充值的金额：";
		cin >> money;
		buf[0] = { 0 };
		gcvt(money, 10, buf);
		sent = buf;
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		Sleep(10);
		cout << "余额修改成功！" << endl;
		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		float Money = atof(buf);
		cout << "目前您的余额为：" << Money << endl;
		ConsumerDisplay();	//返回消费者管理界面
		return;
	}
	else if (choice == 5)
	{
		sent = "5";
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		Sleep(150);

		cout << "请输入您要购买的商品名称：";
		string name;
		cin >> name;
		sent = name;
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		Sleep(10);
		//接收判断是否存在该商品                                                            
		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		choice = atoi(buf);
		if (choice == 1)
		{
			//接收商品数量
			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			int Amout = atoi(buf);
			cout << "请输入您要购买的此商品的数量：";
			int amout;
			cin >> amout;
			if (amout > Amout)
			{
				//若商品库存不够
				cout << "购买失败，您选择的数量大于商品库存" << endl;
				cout << "商品库存为：" << Amout << endl;
				sent = "0";
				send(socketServer, sent.c_str(), sizeof(sent), 0);
				Sleep(10);
				ConsumerDisplay();	//返回消费者管理界面
				return;
			}
			else
			{
				sent = "1";
				send(socketServer, sent.c_str(), sizeof(sent), 0);
				Sleep(10);
				cout << "请选择您的购买方式：" << endl;
				cout << "     (1)直接购买\n     (2)加入购物车\n\nChoice:";
				cin >> choice;
				if (choice == 1)
					{
						sent = "1";
						send(socketServer, sent.c_str(), sizeof(sent), 0);
						Sleep(10);

						buf[0] = { 0 };
						recv(socketServer, buf, sizeof(buf), 0);
						float price;
						price = atof(buf);

						buf[0] = { 0 };
						recv(socketServer, buf, sizeof(buf), 0);
						float money;
						money = atof(buf);

						if (money < price * amout)
						{
							//若用户余额不足 
							cout << "您的余额不足，请先充值！" << endl;
							sent = "-1";
							send(socketServer, sent.c_str(), sizeof(sent), 0);
							Sleep(10);
							ConsumerDisplay();	//返回消费者管理界面
							return;
						}
						else
						{
							//余额充足，可以购买返回数量
							buf[0] = { 0 };
							itoa(amout, buf, 10);
							sent = buf;
							send(socketServer, sent.c_str(), sizeof(sent), 0);
							Sleep(10);
							cout << "您已完成购买！" << endl;
							money -= price * amout;
							cout << "目前您的余额为：" << money << endl;
							ConsumerDisplay();	//返回消费者管理界面
							return;
						}
					}
					//加入购物车
					else if (choice == 2)
					{
						sent = "2";
						send(socketServer, sent.c_str(), sizeof(sent), 0);
						Sleep(10);

						//发送购买数量
						buf[0] = { 0 };
						itoa(amout, buf, 10);
						sent = buf;
						send(socketServer, sent.c_str(), sizeof(sent), 0);
						Sleep(10);

						cout << "您已加入到购物车中！" << endl << endl;
						ConsumerDisplay();	//返回消费者管理界面
						return;
					}
				
			}
		}
		else if(choice == 0)
		{
			cout << "该商品名称不存在！" << endl;
			ConsumerDisplay();	//返回消费者管理界面
			return;
		}
	}
	else if (choice == 6)
	{
		sent = "6";
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		Sleep(10);

		flag = 0;
		cout << "     购物车展示如下：" << endl;
		//接收一个字符
		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		choice = atoi(buf);
		if (choice == 1)
		{
			while (choice == 1)
			{
				buf[0] = { 0 };
				recv(socketServer, buf, sizeof(buf), 0);
				cout << "编号：" << buf << endl;
				buf[0] = { 0 };
				recv(socketServer, buf, sizeof(buf), 0);
				cout << "商品名称：" << buf << endl;
				buf[0] = { 0 };
				recv(socketServer, buf, sizeof(buf), 0);
				cout << "商品数量：" << buf << endl;
				buf[0] = { 0 };
				recv(socketServer, buf, sizeof(buf), 0);
				cout << "商品价格：" << buf << endl;
				buf[0] = { 0 };
				recv(socketServer, buf, sizeof(buf), 0);
				cout << "商品店铺：" << buf << endl << endl;

				//接收是否还有商品
				buf[0] = { 0 };
				recv(socketServer, buf, sizeof(buf), 0);
				choice = atoi(buf);
			}
			if (choice == 0)
			{
				cout << "请选择您的操作：" << endl;
				cout << "     (1)生成订单\n     (2)删除商品\n     (3)修改商品数量\n     (4)返回\n\nChoice:";
				while (1)
				{
					cin >> choice;
					if (choice == 1)
					{
						sent = "1";
						send(socketServer, sent.c_str(), sizeof(sent), 0);
						Sleep(10);
						OrderPay();
					}
					else if (choice == 2)
					{
						sent = "2";
						send(socketServer, sent.c_str(), sizeof(sent), 0);
						Sleep(10);
						OrderDelete();
					}
					else if (choice == 3)
					{
						sent = "3";
						send(socketServer, sent.c_str(), sizeof(sent), 0);
						Sleep(10);
						OrderChange();
					}
					else if (choice == 4)
					{
						sent = "4";
						send(socketServer, sent.c_str(), sizeof(sent), 0);
						Sleep(10);
					}
					if (choice == 1 || choice == 2 || choice == 3 || choice == 4)
					{
						ConsumerDisplay();
						return;
					}
				}
			}
		}
		else if (choice == 0)
		{
			cout << "          您的购物车为空！" << endl << endl;
			ConsumerDisplay();	//返回消费者管理界面
			return;			//退出消费者管理界面
		}
	}
	else if (choice == 7)
	{
		sent = "7";
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		Sleep(10);

		flag = 0;
		cout << "     您生成的订单展示如下：" << endl;
		//接收一个字符
		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		choice = atoi(buf); 
		//说明有商品
		if (choice == 1)
		{
			while (choice == 1)
			{
				buf[0] = { 0 };
				recv(socketServer, buf, sizeof(buf), 0);
				cout << "编号：" << buf << endl;

				buf[0] = { 0 };
				recv(socketServer, buf, sizeof(buf), 0);
				cout << "商品名称：" << buf << endl;

				buf[0] = { 0 };
				recv(socketServer, buf, sizeof(buf), 0);
				cout << "商品数量：" << buf << endl;

				buf[0] = { 0 };
				recv(socketServer, buf, sizeof(buf), 0);
				cout << "商品价格：" << buf << endl;

				buf[0] = { 0 };
				recv(socketServer, buf, sizeof(buf), 0);
				cout << "商品店铺：" << buf << endl << endl;

				//接收是否还有商品
				buf[0] = { 0 };
				recv(socketServer, buf, sizeof(buf), 0);
				choice = atoi(buf);
			}
			if (choice == 0)
			{
				cout << "请选择您的操作：" << endl;
				cout << "     (1)支付订单\n     (2)返回\n\nChoice:";
				while (1)
				{
					cin >> choice;
					if (choice == 1)
					{
						sent = "1";
						send(socketServer, sent.c_str(), sizeof(sent), 0);
						Sleep(10);
						OrderPayment();
					}
					else if (choice == 2)
					{
						sent = "2";
						send(socketServer, sent.c_str(), sizeof(sent), 0);
						Sleep(10);
					}
					if (choice == 1 || choice == 2 )
					{
						ConsumerDisplay();
						return;
					}
				}
			}
		}
		else if (choice == 0)
		{
			cout << "          您生成的订单为空！" << endl << endl;
			ConsumerDisplay();	//返回消费者管理界面
			return;			//退出消费者管理界面
		}
	}
	else
	{
		sent = "8";
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		Sleep(10);
		cout << "		退出登录" << endl << endl;
		return;	//退出消费者管理界面 
	}
}

void ConsumerEnter()		//消费者登录 
{
	string id;
	string pwd;
	int flag = 0;
	cout << "请输入您的ID：";
	while (flag == 0)
	{
		cin >> id;
		sent = id;
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		flag = 0;
		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		int choice = atoi(buf);
		//判断是否存在该ID
		if (choice == 1)
		{
			cout << "请输入您的密码：";
			flag = 1;
			buf[0] = { 0 };
			recv(socketServer, buf, sizeof(buf), 0);
			while (flag == 1)
			{
				//判断两次密码是否相同 
				cin >> pwd;
				flag = 2;
				if (buf != pwd)
				{
					cout << "密码错误，请重新输入:";
					flag = 1;
				}
				else if (buf == pwd)
				{
					cout << "		登录成功！" << endl << endl;
					int i = 0;
					ConsumerDisplay();	//返回消费者管理界面
					return; 		//退出消费者管理界面
				}
			}
		}
		else if (choice == 0)
			cout << "该ID不存在，请重新输入:";
	}
}

void MerchantRegister()		//商家注册 
{
	string pw1, pw2;		//两个字符串用来设置密码 
	int flag = 0;
	cout << "请设置您的ID：";
	buf[0] = { 0 };
	while (!flag)
	{
		string ID;
		cin >> ID;
		sent = ID;
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		flag = 1;
		//检查ID是否已经存在
		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		int choice = atoi(buf);
		if (choice == 0)
		{
			cout << "该ID已存在，请重新设置:";
			flag = 0;
		}
		else if (choice == 1)
		{
			cout << "请设置您的密码：";
			flag = 0;
			//检查两次输入的密码是否相同 
			while (!flag)
			{
				cin >> pw1;
				cout << "请再次确认您的密码：";
				cin >> pw2;
				if (pw1 != pw2)
				{
					cout << "前后两次输入密码不一致，请重新设置密码:";
					flag = 0;
				}
				else
				{
					flag = 1;
					cout << "注册成功！" << endl;
					sent = pw1;
					send(socketServer, sent.c_str(), sizeof(sent), 0);
					return;
				}
			}
		}
	}
}

void ConsumerRegister()		//消费者注册 
{
	string pw1, pw2;			//两个字符串用来设置密码
	int flag = 0;
	cout << "请设置您的ID：";
	while (!flag)
	{
		string ID;
		cin >> ID;
		sent = ID;
		send(socketServer, sent.c_str(), sizeof(sent), 0);
		flag = 1;
		//检查ID是否已经存在
		buf[0] = { 0 };
		recv(socketServer, buf, sizeof(buf), 0);
		int choice = atoi(buf);
		if (choice == 0)
		{
			cout << "该ID已存在，请重新设置:";
			flag = 0;
		}
		else if (choice == 1)
		{
			cout << "请设置您的密码：";
			flag = 0;
			//检查两次输入的密码是否相同 
			while (!flag)
			{
				cin >> pw1;
				cout << "请再次确认您的密码：";
				cin >> pw2;
				if (pw1 != pw2)
				{
					cout << "前后两次输入密码不一致，请重新设置密码:";
					flag = 0;
				}
				else
				{
					flag = 1;
					cout << "注册成功！" << endl;
					sent = pw1;
					send(socketServer, sent.c_str(), sizeof(sent), 0);
					return;
				}
			}
		}
	}
}



int main(void)
{
	//打开网络库
	WORD wdVersion = MAKEWORD(2, 2);
	WSADATA wdSockMsg;
	//成功打开则返回0
	WSAStartup(wdVersion, &wdSockMsg);

	//创建socket
	socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//创建失败
	if (INVALID_SOCKET == socketServer)
	{
		//关闭网络库
		WSACleanup();
		return 0;
	}
	//链接服务器
	struct sockaddr_in serveMsg;
	serveMsg.sin_family = AF_INET;
	serveMsg.sin_port = htons(12345);
	serveMsg.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	//连接服务器开始与服务端通信
	connect(socketServer, (const struct sockaddr*)&serveMsg, sizeof(serveMsg));

	int choice;
	string type;
	while (1)
	{
		cout << "     (1)注册账号\n     (2)登录账号\n     (3)展示商品\n     (4)退出平台系统\n\nChoice:";
		cin >> choice;
		type = " ";		//初始化类型字符串
		if (choice == 1)
		{
			//注册用户账号
			cout << "请输入用户类型（商家/消费者）：";
			//检查是否输入了正确的用户类型
			while (type != "商家" && type != "消费者")
			{
				cin >> type;
				if (type == "商家")
				{
					send(socketServer, "11", sizeof("11"), 0);
					MerchantRegister();	//商家注册 
				}
				else if (type == "消费者")
				{
					send(socketServer, "12", sizeof("12"), 0);
					ConsumerRegister();	//消费者注册 
				}
				else
					cout << "输入错误！请重新输入用户类型（商家/消费者）：";
			}
		}
		else if (choice == 2)
		{
			//登录用户账号 
			cout << "请输入用户类型（商家/消费者）：";
			//检查是否输入了正确的用户类型
			while (type != "商家" && type != "消费者")
			{
				cin >> type;
				if (type == "商家")
				{
					send(socketServer, "21", sizeof("21"), 0);
					MerchantEnter();	//商家登录 
				}
				else if (type == "消费者")
				{
					send(socketServer, "22", sizeof("22"), 0);
					ConsumerEnter();	//消费者登录
				} 
				else
					cout << "输入错误！请重新输入用户类型（商家/消费者）：";
			}
		}
		else if (choice == 3)
		{
			send(socketServer, "3", sizeof("3"), 0);
			//展示商品信息 
			CommodityCommodityShow();
		}
		else if (choice == 4)
		{
			send(socketServer, "4", sizeof("4"), 0);
			//退出平台系统
			exit(EXIT_FAILURE);
		}
	}

	//销毁套接字
	closesocket(socketServer);
	//关闭网络库
	WSACleanup();

	system("pause");
	return 0;
}