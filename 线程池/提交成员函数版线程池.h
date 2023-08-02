#include <iostream>
#include <vector>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <functional>
#include <future>
#include <unistd.h>
namespace qianlan{
	class Threadpool;
}
class qianlan::Threadpool
{
	using Task = std::function<void()>;
private:
	std::vector<std::thread> pool;
	std::queue <Task> tasks;//任务队列
	std::atomic <bool> stop;//是否关闭提交
	bool m_exit = false;//保证子线程顺利退出的标志
	std::condition_variable cv_task;//条件变量
	std::mutex m_task;
public:
	Threadpool(size_t size = 4);//构造函数,默认创建4个线程
	~Threadpool();
	//关闭提交
	void shutdown(){
		stop.store(true);
	}
	//开启提交
	void restart(){
		stop.store(false);
	}
	//提交一个任务
	template<class F,class... Args>
	void commit(F&& f,Args&&... args);
	
private:
	//获取一个任务
	Task get_one_task(){//其返回值task放在外部会出现未定义
		std::unique_lock<std::mutex> lock(m_task);
		cv_task.wait(lock,[this](){return !tasks.empty()||m_exit;});
		if(m_exit){
			return nullptr;//相当于std::function<void()>(nullptr);
			//using Task = std::function<void()>,std::function<void()>有function(nullptr_t) noexcept
		}
		Task task = std::move(tasks.front());
		tasks.pop();
		return task;
	}
	void schedule();//任务调度
	
	
	
};

qianlan::Threadpool::Threadpool(size_t size):stop(false)
{
	size = size < 1 ? 1 : size;
	for(size_t i = 0; i < size; ++i)
	{
		pool.emplace_back(&Threadpool::schedule,this);
	}
}
qianlan::Threadpool::~Threadpool()
{
	shutdown();
	m_exit = true;
	cv_task.notify_all();
	for (std::thread& thread : pool){
		thread.join();
		//thread.detach();
	}
}
template <class F, class... Args>//提交成员函数时第二参数为对象地址
void qianlan::Threadpool::commit(F&& f,Args&&... args)
{
	if(stop.load()){
		throw std::runtime_error("ThreadPool have closed commit...");
	}
	
	auto task = std::make_shared<std::packaged_task<void()>>(
		std::bind(std::forward<F>(f),std::forward<Args>(args)...)
		);
	{
		std::lock_guard<std::mutex> lock(m_task);
		tasks.emplace([task](){
			(*task)();
			});
	}
	cv_task.notify_one();//只唤醒一个线程
	
	
}

void qianlan::Threadpool::schedule()
{
	while (1){
		if(auto task = get_one_task()){
			task();
		}
		else{
			std::cout << "子线程结束,线程id: " <<_threadid<< std::endl;
			return;
		}
	}
}

