#include <iostream>

#include "thread_pool.h"
#include "parser.h"
#include "dir.h"

using std::cout;
using std::endl;

// creates the thread pool
Thread_Pool::Thread_Pool():
nr_of_threads(std::thread::hardware_concurrency()), t_sleep_count(0){
	for(auto i = 0; i < nr_of_threads; i++){
		thread_pool.push_back(std::thread([this]{process_task();}));

		/* alternative ways to the previous
	
		   thread_pool.push_back(std::thread(&Thread_Pool::process_task, this));
		   thread_pool.push_back(std::thread(process_task, i));
		*/
	}
}

// joins the threads
Thread_Pool::~Thread_Pool(){
	for(auto i = 0; i < nr_of_threads; i++){
		thread_pool[i].join();
	}
}

// shares the syncronization details with Parser class and initiates the parsing of project
// directory files 
std::vector<File_Descriptor*> *Thread_Pool::process_dir(Dir *dir){
	Parser::m_data_condition = &m_data_condition;
	Parser::m_ptr = &task_queue_mutex;
	task_queue_mutex.lock();
	dir_queue.push(std::bind(&Parser::parse_dir, dir, nullptr));
	task_queue_mutex.unlock();
	m_data_condition.notify_one();
	

	f = p.get_future();
   return f.get();
}

/* (thread worker function)

   1. Round: Threads are parsing the project directory and sub directories and filling
      dir_queue with (Parser::parse_dir(Dir *dir, Dir *parent_dir = nullptr)) and file_queue
      with (Parser::parse_file(File_Descriptor *file_descr))

      (intermediate stage)
      Last active thread switches to file_queue which is now populated with functors 
      (Parser::parse_file(File_Descriptor *file_descr)) and wakes up all the threads

   2. Round: Threads are parsing the files (emptying the file_queue)

   -----------------------------------------------------------------------------------------

   Improvement

   Share the file_queue between the threads into equal shares and call Parser::parse_dir()
   directly and do not decrease the size of file_queue. No unloading and locking.

*/
void Thread_Pool::process_task(){
	std::function<void()> task;
	while(true){
		{
			std::unique_lock<std::mutex> lck(task_queue_mutex);
			m_data_condition.wait(lck, [this]{
				if(task_queue_ptr->empty()){
					t_sleep_count++;
					if(t_sleep_count == nr_of_threads){
						if(task_queue_ptr != &file_queue){
							task_queue_ptr = &file_queue;
						}
                  else{
                  	terminate = true;
                  	p.set_value(&Parser::header_files);
                  }
						m_data_condition.notify_all();
						return true;
               }
               return false;
				}
				if(t_sleep_count){
					t_sleep_count--;
				}
				return true;
			});
			if(terminate.load()){return;}
			task = task_queue_ptr->front();
			int size = dir_queue.size();
			int size2 = file_queue.size();
			cout << size << " " << size2 << endl;
			task_queue_ptr->pop();
		}
		task();
	}
}

std::queue<std::function<void()>> Thread_Pool::file_queue;
std::queue<std::function<void()>> Thread_Pool::dir_queue;