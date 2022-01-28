#include <iostream>
#include <vector>
#include <fstream>

#include "project_structure.h"
#include "dir.h"
#include "file_descriptor.h"
#include "thread_pool.h"
#include "parser.h"

using std::cout;
using std::endl;
using std::ofstream;

Project_Structure::Project_Structure(const std::string &project_path):
project_dir(new Dir(project_path)), thread_pool(new Thread_Pool())
{
   /*std::vector<File_Descriptor*> *processed_header_files =*/ thread_pool->process_dir(project_dir);

   cout << "finnished" << endl;

   //draw_map(processed_header_files);
}

// frees memory
Project_Structure::~Project_Structure(){
   for(auto i = 0; i < Parser::dirs.size(); i++){
      delete Parser::dirs[i];
   }
   cout << 1 << endl;
	//for(auto i = 0; i < Parser::all_files.size(); i++){
	//	delete Parser::all_files[i];
	//}
   cout << 2 << endl;
	for(auto i = 0; i < Parser::header_files.size(); i++){  // brakes here ???
		delete Parser::header_files[i];
	}
   cout << 3 << endl;
	for(auto i = 0; i < Parser::source_files.size(); i++){
		delete Parser::source_files[i];
	}
   cout << 4 << endl;
	for(auto i = 0; i < Parser::other_files.size(); i++){
		delete Parser::other_files[i];
	}
   cout << 5 << endl;
   delete thread_pool;
}

// draws an independece graph of the source and header files using sfdp
void Project_Structure::draw_map(std::vector<File_Descriptor*> *ptr){
   ofstream file("pic.gv");
   if(file.is_open()){
      file << "digraph G\n";

      int indx = 0;
      for(auto i = 0; i < Parser::source_files.size(); i++){
         if(Parser::source_files[i]->header_files.size()){
            Parser::source_files[i]->counter = indx;
            indx++;
         }
      }
      for(auto i = 0; i < Parser::header_files.size(); i++){
         if(Parser::header_files[i]->header_files.size()){
            Parser::header_files[i]->counter = indx;
            indx++;
         }
      }
 
      //COLORED NODES
      //file << "\tnode [colorscheme=ylorbr9];\n";
      //for(auto i = 0; i < header_files.size(); i++){
      //   int col = header_files[i]->counter/467;
      //   if(col == 0){col = 1;}
      //   file << "\t" << i << " [color=" << 9 << ", fillcolor=" << 9 << ", style=filled];\n";
      //   header_files[i]->counter = i;
      //}

      for(auto i = 0; i < Parser::header_files.size(); i++){
         for(auto j = 0; j < Parser::header_files[i]->header_files.size(); j++){
            file << "\t" << Parser::header_files[i]->counter 
            << " -> " << Parser::header_files[i]->header_files[j]->counter << ";\n";
         }
      }
      for(auto i = 0; i < Parser::source_files.size(); i++){
         for(auto j = 0; j < Parser::source_files[i]->header_files.size(); j++){
            file << "\t" << Parser::source_files[i]->counter << " -> " << Parser::source_files[i]->header_files[j]->counter << ";\n";
         }
      }
 
      file << "}";
      file.close(); 
      //system("dot -Tpng graph1.gv -o graph1.png");
      system("sfdp -Gsize=67! -Goverlap=prism -Tpng pic.gv > pic.png");     
   }
}

// compare function for sort() (<algorithm>)
bool Project_Structure::sortFunction(File_Descriptor *file_descr1, File_Descriptor *file_descr2){
   return file_descr1->counter > file_descr2->counter;
}