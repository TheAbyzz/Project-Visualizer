#include <algorithm>

#include "dir.h"

Dir::Dir(const std::string &dir_path, Dir *parent_dir /*= nullptr*/):dir_path(dir_path), parent_dir(parent_dir){
	dir_name = getname(dir_path);
}

// gets the directory name from the direcotry path
std::string Dir::getname(const std::string &dir_path){
    std::string copy = dir_path; 
    reverse(copy.begin(), copy.end());
    return dir_path.substr(copy.length()-copy.find("/"),copy.length()-2);
}