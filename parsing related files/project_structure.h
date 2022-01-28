#ifndef PROJECT_STRUCTURE_H
#define PROJECT_STRUCTURE_H

#include <string>
#include <vector>

class Dir;
class Thread_Pool;
class File_Descriptor;

class Project_Structure{
public:
	Project_Structure(const std::string &project_path);
	~Project_Structure();
	void draw_map(std::vector<File_Descriptor*> *ptr);
	static bool sortFunction(File_Descriptor *file_descr1, File_Descriptor *file_descr2);

private:
	Dir *project_dir;
	Thread_Pool *thread_pool;
};

#endif