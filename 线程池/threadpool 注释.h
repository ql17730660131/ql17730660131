#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <iostream>
#include <functional>
#include <thread>
#include <condition_variable>
#include <future>
#include <atomic>
#include <vector>
#include <queue>
namespace qianlan{
	class ThreadPool;
}

class qianlan::ThreadPool
{
	using Task = std::function<void()>;
private:
	//管理线程容器
	std::vector<std::thread> pool;
	//任务队列
	std::queue<Task> tasks;
	//同步
	std::mutex m_task;

	std::condition_variable cv_task;
	//是否关闭提交
	std::atomic<bool> stop;
	//保证子线程顺利退出的标志
	bool m_exit = false;//保证子线程顺利退出的标志
	
public:
	//构造函数
	ThreadPool(size_t size = 4):stop(false){
		size = size < 1 ? 1 : size;
		for(size_t i = 0; i < size; i++){
			pool.emplace_back(&ThreadPool::schedule,this);// push_back(std::thread{...})
//			pool.push_back(std::thread(&ThreadPool::schedual,this));
		}
	}
	//析构函数
	~ThreadPool(){
		shutdown();//停止任务提交
		m_exit = true;
		cv_task.notify_all();
		for(std::thread& thread : pool){
// 			thread.detach();//让线程自生自灭
			thread.join();//等待所有线程执行完毕
		}
	}
	//停止任务提交
	void shutdown(){
		this->stop.store(true);
	}
	
	//重启任务提交
	void restart(){
		this->stop.store(false);
	}
	
	//提交一个任务
	template <class F,class... Args>
	auto commit(F&& f,Args&&... args) ->std::future<decltype(f(args...))>{
		if(stop.load()){//stop==true停止提交
			throw std::runtime_error("ThreadPool have closed commit.");
		}
		
		using ResType = decltype(f(args...));//函数的返回值类型  typename std::result_of<F(Args...)>::type 
		
		//返回智能指针指向packaged_task对象
		//std::packaged_task 是 C++11 中的一个模板类，它可以将一个可调用对象（比如函数、函数对象或者 Lambda 表达式）
		//与一个 future 关联起来，从而实现异步调用。它将一个异步操作封装成一个可调用对象，并将结果通过 future 来传递，
		//可以在多线程环境下使用。

        //通过 std::packaged_task 可以将一个任务异步化，将任务的计算与任务的执行分离，使得任务可以在另外一个线程中执
		//行，从而实现并发执行多个任务，提高程序的效率。
		//bind()返回值是std::packaged_task<ResType()>构造时的参数
		//std::packaged_task见explain.txt
		auto task = std::make_shared<std::packaged_task<ResType()>>(
			std::bind(std::forward<F>(f),std::forward<Args>(args)...)
			);//std::forward<T>(u)有两个参数：T 与 u。当T为左值引用类型时，u将被转换为T类型的左值，否则u将被转换
			//为T类型右值。如此定义std::forward是为了在使用右值引用参数的函数模板中解决参数的完美转发问题
		{
			//添加到任务队列
			std::lock_guard<std::mutex> lock(m_task);//见最后
			tasks.emplace([task](){    //存储一个匿名函数,匿名函数执行这个任务
				(*task)();//存储任务此时并不会执行
				});
		}
		cv_task.notify_all();//唤醒线程执行//cv_task.notify_one();
		std::future<ResType> future = task->get_future();
		return future;
	}
private:
	// 获取一个待执行的 task
	Task get_one_task(){
		std::unique_lock<std::mutex> lock(m_task);
		cv_task.wait(lock,[this](){return !tasks.empty()||m_exit;});//wait直到有task
		if(m_exit){//相当于std::function<void()>(nullptr);
			return nullptr;//若调用析构，直接返回子线程退出
			//using Task = std::function<void()>,std::function<>有function(nullptr_t) noexcept
		}
		Task task(std::move(tasks.front()));//加入任务用的是std::packaged_task，这里用std::move()
		tasks.pop();
		return task;
	}
	
	//任务调度
	void schedule(){
		while(true){
			if(Task task = get_one_task()){
				task();
			}
			else{
			std::cout << "子线程结束,线程id: " <<std::this_thread::get_id()<< std::endl;
			return;
		}
		}	
	}
};
// unique_lock 是一个可以更灵活地控制锁的类型，它支持锁的条件获取、手动释放和转移所有权。
// lock_guard 是一个更简单的锁类型，在构造时获取锁，在析构时释放锁，并且不支持手动释放和转移所有权。

// 通常建议使用 lock_guard，因为它更安全、更易于使用。但是，如果您需要对锁进行更细粒度的控制，
// 则可以使用 unique_lock。
#endif