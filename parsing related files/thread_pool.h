#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <atomic>
#include <condition_variable>
#include <future>

class Dir;
class File_Descriptor;

class Thread_Pool{
public:
	Thread_Pool();
	~Thread_Pool();
    std::vector<File_Descriptor*> *process_dir(Dir *dir);
    void process_task();
	std::vector<std::thread> thread_pool;
	static std::queue<std::function<void()>> file_queue;
    static std::queue<std::function<void()>> dir_queue;
	std::queue<std::function<void()>> *task_queue_ptr = &dir_queue;
private:
	friend class Project_Structure;
	std::mutex task_queue_mutex;
	std::atomic<int> t_sleep_count;
	std::atomic<bool> terminate{false};
	std::condition_variable m_data_condition;
	std::promise<std::vector<File_Descriptor*>*> p;
	std::future<std::vector<File_Descriptor*>*> f;
	int nr_of_threads;
};

#endif