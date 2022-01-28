#ifndef FILE_DESCRIPTOR_H
#define FILE_DESCRIPTOR_H

#include <string>
#include <vector>
#include <atomic>

class Dir;
class Parser;

class File_Descriptor{
public:
	File_Descriptor(const std::string &file_path, Dir *dir);
	~File_Descriptor();
	std::string getname(const std::string &file_path);
	std::atomic<int> counter{0};
	bool operator< (const File_Descriptor *file_descr) const;
private:
	std::string file_path;
	std::string file_name;
	std::vector<File_Descriptor*> header_files;
	Dir *dir;
	Parser *parser;
	friend class Parser;
	friend class Project_Structure;
};

#endif