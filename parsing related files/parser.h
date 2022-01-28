#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <regex>
#include <mutex>
#include <condition_variable>

class File_Descriptor;
class Dir;

class Parser{
public:
	static void parse_dir(Dir *dir, Dir *parent_dir = nullptr);
	void parse_file(File_Descriptor *file_descr);
	static std::vector<Dir*> dirs;
	static std::vector<File_Descriptor*> all_files;
	static std::vector<File_Descriptor*> header_files;
	static std::vector<File_Descriptor*> source_files;
	static std::vector<File_Descriptor*> other_files;

    static std::condition_variable *m_data_condition;
    static std::mutex *m_ptr;

private:
	static void do_header(const std::string &header_file, File_Descriptor *file_descr);
	void do_function(const std::smatch &match, File_Descriptor *file_descr);
	std::vector<std::regex> regexes = {std::regex("#include [<|\"]([^ \t\r\n\f]+)[>|\"]")}; // regex("[void|int|char|string|long|double|float]")

	static std::mutex dir_mutex;
	static std::mutex file_task_queue_mutex;
	static std::mutex header_files_mutex;
	static std::mutex source_files_mutex;
	static std::mutex other_files_mutex;
};

#endif