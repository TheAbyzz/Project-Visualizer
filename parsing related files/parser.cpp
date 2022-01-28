#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <filesystem>

#include "parser.h"
#include "file_descriptor.h"
#include "project_structure.h"
#include "dir.h"
#include "thread_pool.h"

using std::cout;
using std::endl;

// checks if file is a directory, source or header file or other file
void Parser::parse_dir(Dir *dir, Dir *parent_dir /*=nullptr*/){
    std::unique_lock<std::mutex> lck1(dir_mutex);
    dirs.push_back(dir);
    lck1.unlock();
    for(auto it = std::filesystem::directory_iterator(dir->dir_path); it != std::filesystem::directory_iterator(); it++){
        if(it->is_directory()){
            Dir *sub_dir = new Dir(it->path(), dir);  
            dir->sub_dirs.push_back(sub_dir);
            
            std::unique_lock<std::mutex> lck2(*m_ptr);
            Thread_Pool::dir_queue.push(std::bind(&Parser::parse_dir, sub_dir, parent_dir));
            lck2.unlock();
            m_data_condition->notify_one();
        }
        else if(it->is_regular_file()){
         File_Descriptor *file_descr = new File_Descriptor(it->path(), dir);
         dir->file_descriptors.push_back(file_descr);
         
         std::unique_lock<std::mutex> lck3(file_task_queue_mutex);
         Thread_Pool::file_queue.push(std::bind(&Parser::parse_file, file_descr->parser, file_descr));
         lck3.unlock();

         if(it->path().filename().string().length() > 3 && 
            (it->path().filename().string().substr(it->path().filename().string().length()-2, it->path().filename().string().length()-1) == ".h" ||
            it->path().filename().string().substr(it->path().filename().string().length()-3, it->path().filename().string().length()-1) == ".hh")){
            
            std::unique_lock<std::mutex> lck4(header_files_mutex);
            header_files.push_back(file_descr);
            lck4.unlock();
         }
         else if(it->path().filename().string().length() > 3 && 
            (it->path().filename().string().substr(it->path().filename().string().length()-2, it->path().filename().string().length()-1) == ".c" ||
            it->path().filename().string().substr(it->path().filename().string().length()-3, it->path().filename().string().length()-1) == ".cc")){

            std::unique_lock<std::mutex> lck5(source_files_mutex);
            source_files.push_back(file_descr);
            lck5.unlock();
         }
         else{
            std::unique_lock<std::mutex> lck6(source_files_mutex);
            other_files.push_back(file_descr);
            lck6.unlock();
         }
      }
   }
}

// parses the file for details specified in regexes (vector)
void Parser::parse_file(File_Descriptor *file_descr){
	std::ifstream file(file_descr->file_path);
	if(file.is_open()){
		std::string line;
		while(getline(file,line)){
			for(auto i = 0; i < regexes.size(); i++){
				std::smatch match;
				if(regex_search(line, match, regexes[i])){
                    switch(i){
                    	case 0:
                    		do_header(match.str(1), file_descr);
                    		break;
                    	case 1:
                    	    do_function(match, file_descr);
                    	    break;
                        default: break;
                    }
				}
			}
		}
	}
}

// gets the header file name from the path. Ignores the path and preprocessor #include lookup rules for now.
void Parser::do_header(const std::string &header_file, File_Descriptor *file_descr){
	
    std::size_t slash_indx = header_file.find("/");
    if(slash_indx != std::string::npos){
    	std::string hdr_root_dir_name = header_file.substr(0,slash_indx);
        Dir *dir = file_descr->dir;
        Dir *sub_dir = nullptr;
        while(dir && !sub_dir){
            for(auto i = 0; i < dir->sub_dirs.size(); i++){
                if(dir->sub_dirs[i]->dir_name == hdr_root_dir_name){
                    sub_dir = dir->sub_dirs[i];
                    break;
                }
            }
            dir = dir->parent_dir;
        }
        if(!dir || !sub_dir){return;}       
        std::size_t str_indx = slash_indx+1;
        slash_indx = header_file.substr(str_indx, header_file.length()-1).find("/");
        while(slash_indx != std::string::npos){
            for(auto i = 0; i < sub_dir->sub_dirs.size(); i++){
                if(header_file.substr(str_indx, slash_indx) == sub_dir->sub_dirs[i]->dir_name){
                    sub_dir = sub_dir->sub_dirs[i];
                    break;
                }
            }
            slash_indx = header_file.substr(str_indx, slash_indx).find("/");
            str_indx = slash_indx != std::string::npos ? slash_indx : str_indx;
            str_indx++;
        }
        for(auto i = 0; i < sub_dir->file_descriptors.size(); i++){
            if(sub_dir->file_descriptors[i]->file_name == header_file.substr(str_indx, header_file.length()-1)){
                std::lock_guard<std::mutex> lck(header_files_mutex);
                sub_dir->file_descriptors[i]->counter++;
                file_descr->header_files.push_back(sub_dir->file_descriptors[i]);
                break;
            }
        }
    }
    else{
    	for(auto i = 0; i < header_files.size(); i++){
    		if(header_file == header_files[i]->file_name){             
    			std::lock_guard<std::mutex> lck(header_files_mutex);
                header_files[i]->counter++;
                header_files.push_back(header_files[i]);
    		    break;
    	    }
        }    	
    }
}

// finds the defintion of the function
void Parser::do_function(const std::smatch &match, File_Descriptor *file_descr){
    // regex("[void|int|char|string|long|double|float]")
}

std::vector<Dir*> Parser::dirs;
std::vector<File_Descriptor*> Parser::all_files;
std::vector<File_Descriptor*> Parser::header_files;
std::vector<File_Descriptor*> Parser::source_files;
std::vector<File_Descriptor*> Parser::other_files;

std::condition_variable *Parser::m_data_condition;
std::mutex *Parser::m_ptr;

std::mutex Parser::dir_mutex;
std::mutex Parser::file_task_queue_mutex;
std::mutex Parser::header_files_mutex;
std::mutex Parser::source_files_mutex;
std::mutex Parser::other_files_mutex;