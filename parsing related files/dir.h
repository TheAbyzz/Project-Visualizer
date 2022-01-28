#ifndef DIR_H
#define DIR_H

#include <string>
#include <vector>

class File_Descriptor;

class Dir{
public:
	Dir(const std::string &dir_path, Dir* parent_dir = nullptr);
	std::string getname(const std::string &dir_path);
	Dir *parent_dir;
	std::string dir_name;
	std::vector<Dir*> sub_dirs;
	std::vector<File_Descriptor*> file_descriptors;
private:
	std::string dir_path;
    friend class Project_Structure;
    friend class Parser;
};

#endif