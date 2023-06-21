#include <iostream>
#include "Connection.h"
#include "CommonConnectionPool.h"
using namespace std;
void test();  // 单线程测试
void test2(); // 8个多线程测试(连接少看不出效果，用8个线程)
int main()
{
#if 0
	test();
#else
	test2();
#endif
	return 0;
}

void test()
{
	steady_clock::time_point begin = steady_clock::now();
#if 0
	//不使用连接池
	for (int i = 0; i < 10000; ++i)
	{
		Connection conn;
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
			"zhang san", 20, "male");
		conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
		conn.update(sql);
	}
#else
	// 使用连接池
	ConnectionPool *cp = ConnectionPool::getConnectionPool();
	for (int i = 0; i < 10000; ++i)
	{
		shared_ptr<Connection> sp = cp->getConnection();
		char sql[1024] = {0};
		sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
		sp->update(sql);
	}
#endif
	steady_clock::time_point end = steady_clock::now();
	nanoseconds res = end - begin;
	milliseconds millsec = duration_cast<milliseconds>(res); // 纳秒转换为毫秒
	std::cout << millsec.count() << "ms" << endl;
	// this_thread::sleep_for(chrono::seconds(20));
}

void test2() // 多线程测试
{
	// 不用数据库连接池
#if 0
	Connection conn;
	conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
	steady_clock::time_point begin = steady_clock::now();
	thread t1([]()
			  {
		for (int i = 0; i < 2500; ++i)
		{
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		} });
	thread t2([]()
			  {
		for (int i = 0; i < 2500; ++i)
		{
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		} });
	thread t3([]()
			  {
		for (int i = 0; i < 2500; ++i)
		{
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		} });
	thread t4([]()
			  {
		for (int i = 0; i < 2500; ++i)
		{
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		} });
	thread t5([]()
			  {
		for (int i = 0; i < 2500; ++i)
		{
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		} });
	thread t6([]()
			  {
		for (int i = 0; i < 2500; ++i)
		{
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		} });
	thread t7([]()
			  {
		for (int i = 0; i < 2500; ++i)
		{
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		} });
	thread t8([]()
			  {
		for (int i = 0; i < 2500; ++i)
		{
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		} });
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();
	t7.join();
	t8.join();

// 用数据库连接池
#else

	/*测试产生连接是否正常
	for (int i = 0; i < 100; i++)
	{
		thread t([]()
				  {
		ConnectionPool *cp = ConnectionPool::getConnectionPool();
		for (int i = 0; i < 100; ++i)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		} });
		t.detach();
	}
	*/

	ConnectionPool *cp = ConnectionPool::getConnectionPool();
	steady_clock::time_point begin = steady_clock::now();
	thread t1([&]()
			  {
		for (int i = 0; i < 2500; ++i)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		} });
	thread t2([&]()
			  {
		for (int i = 0; i < 2500; ++i)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		} });
	thread t3([&]()
			  {
		for (int i = 0; i < 2500; ++i)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		} });
	thread t4([&]()
			  {
		for (int i = 0; i < 2500; ++i)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		} });
	thread t5([&]()
			  {
		for (int i = 0; i < 2500; ++i)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		} });
	thread t6([&]()
			  {
		for (int i = 0; i < 2500; ++i)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		} });
	thread t7([&]()
			  {
		for (int i = 0; i < 2500; ++i)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		} });
	thread t8([&]()
			  {
		for (int i = 0; i < 2500; ++i)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		} });

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();
	t7.join();
	t8.join();
#endif
	steady_clock::time_point end = steady_clock::now();
	nanoseconds res = end - begin;
	milliseconds millsec = duration_cast<milliseconds>(res); // 纳秒转换为毫秒
	std::cout << millsec.count() << "ms" << endl;
	// this_thread::sleep_for(chrono::seconds(20));//测试超时连接删除功能
}
