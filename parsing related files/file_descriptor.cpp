#include <algorithm>

#include "file_descriptor.h"
#include "parser.h"

File_Descriptor::File_Descriptor(const std::string &file_path, Dir *dir):file_path(file_path),dir(dir){
	file_name = getname(file_path);
    parser = new Parser();
}

// frees memory
File_Descriptor::~File_Descriptor(){
    delete parser;
}

// gets the file name from the file path
std::string File_Descriptor::getname(const std::string &file_path){
    std::string copy = file_path; 
    reverse(copy.begin(), copy.end());
    return file_path.substr(copy.length()-copy.find("/"),copy.length()-2);	
}

// operator function used for compare function (bool Project_Structure::sortFunction(File_Descriptor *file_descr1, File_Descriptor *file_descr2))
bool File_Descriptor::operator< (const File_Descriptor *file_descr) const{
    return counter < file_descr->counter;
}