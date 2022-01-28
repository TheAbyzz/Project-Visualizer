#include "thread_pool.h"
#include "parser.h"
#include "dir.h"

#include <iostream>
using namespace std;

Thread_Pool::Thread_Pool():
nr_of_threads(std::thread::hardware_concurrency()), t_sleep_count(std::thread::hardware_concurrency()){
	for(auto i = 0; i < nr_of_threads; i++){
		//thread_pool.push_back(std::thread([this]{process_task();}));

		// alternative ways to the previous
		//thread_pool.push_back(std::thread(&Thread_Pool::process_task, this, i));
		//thread_pool.push_back(std::thread(process_task, i));
	}
}

Thread_Pool::~Thread_Pool(){
	for(auto i = 0; i < nr_of_threads; i++){
		thread_pool[i].join();
	}
}

std::vector<File_Descriptor*> *Thread_Pool::process_dir(Dir *dir){
	Parser::m_data_condition = &m_data_condition;
	Parser::m_ptr = &task_queue_mutex;
	dir_queue.push(std::bind(&Parser::parse_dir, dir, nullptr));
	/*function<void()> task = dir_queue.front();
	dir_queue.pop();
	task();
   return &Parser::header_files;*/
   
   for(auto i = 0; i < nr_of_threads; i++){
   	thread_pool.push_back(std::thread(&Thread_Pool::process_task, this, i));
   }

	//int size = dir_queue.size();
	//m_data_condition.notify_one();
	f = p.get_future();
   return f.get(); // or f.wait();*/
}

/*
   HOW DO WE GET INTO PARSER_DIR

   WE STILL DONT GET INTO PARSER::PARSE_DIR()

   THE PROBLEM IS THAT THE PROGRAM IS NOT FAST ENOUGH TO FILL THE DIR_QUEUE WITH THE ROOT NODE.

   PROCESS_DIR()

   WHAT HAPPENS IS THAT THE FIRST THREAD SWITCHES TO FILE_QUEUE WHICH IS ALSO EMPTY BECAUSE
   DIR_QUEUE IS STILL EMPTY AND T_SLEEP_COUNT EQUALS NR_OF_THREADS. WHAT HAPPENS NEXT IS NOT
   CONTROLED BEHAVIOUR ANYMORE. TASK() WILL NEVER POINT TO A FUNCTION.

   TASK_QUEUE_PTR->SIZE() TURNS TO GIBBERISH (LARGE VALUE) BECAUSE TASK_QUEUE_PTR->POP() IS 
   CALLED TO MANY TIMES

   HOW DOES EACH THREAD INCREASE AND DECRESE T_SLEEP_COUNT 1 TIME AT MOST BETWEEN SLEEPING????

   THEORY: 1 THREAD BITES INTO THE FIRST DIR AND DECREASE T_SLEEP_COUNT WITH 1, 8 -> 7
           2 THREAD SEES TASK_QUEUE_PTR (QUEUE) IS EMPTY AND INCREASE T_SLEEP_COUNT WITH 1, 7 -> 8
             AND GOES TO SLEEP
           3 THREAD SEES THE QUEUE IS EMPTY AND T_SLEEP_COUNT EQUALS 8 AND KILLS EVERYTHING
*/
void Thread_Pool::process_task(int id){
	std::function<void()> task;
	while(true){
		{
			std::unique_lock<std::mutex> lck(task_queue_mutex);
			m_data_condition.wait(lck, [this,id]{
				//cout << "mark 0" << endl;
				if(task_queue_ptr->empty()){

               // take the size of the containers and put a breakpoint here

					if(t_sleep_count != nr_of_threads){
						t_sleep_count++;
						//arr[id]++;
					}
					else{
						// continue right away with the file processing
						if(task_queue_ptr != &file_queue){
							task_queue_ptr = &file_queue;
							//lck.unlock(); not captured???
							//cout << "mark" << endl;

                     //int dir_queue_size = dir_queue.size();
                     //int file_queue_size = file_queue.size();

                     flag = true;
                     m_data_condition.notify_all();
							return true;
						}
						// terminate all threads after files have been processed
                        terminate = true;
                        //lck.unlock(); not captured???
                        m_data_condition.notify_all();
                        p.set_value(&Parser::header_files);
					    return true; 
					}
					return false;
				}
				if(t_sleep_count){
					t_sleep_count--;
					//arr[id]--;
				}
				return true;
			});
			//cout << "mark 1" << endl;
			if(terminate.load()){return;}
			//cout << "mark 2 " << task_queue_ptr->size() << endl;
			//int size_t = task_queue_ptr->size();
			task = task_queue_ptr->front(); // <---- PROBLEM IS HERER
			//cout << "mark 3" << endl; 
			int size = dir_queue.size();
			int size2 = file_queue.size();
			cout << size << " " << size2 << endl;
			task_queue_ptr->pop();
			//cout << "here 3 " << task_queue_ptr->size() << endl;
			//cout << "mark 4" << endl;
		}
		//cout << "mark 4" << endl;
		task(); // WHY WAS THIS COMMENTED OUT (bad function call)
	}
}

/*
   WE KNOW THE DIR PROCESSING IS DONE. IT GETS STUCK AT FILE PROCESSING AT TASK = TASK_QUEUE->FRONT(), WHY???
*/

std::queue<std::function<void()>> Thread_Pool::file_queue;
std::queue<std::function<void()>> Thread_Pool::dir_queue;