a.out: main.o project_structure.o thread_pool.o dir.o file_descriptor.o parser.o
	g++ -pthread main.o project_structure.o thread_pool.o dir.o file_descriptor.o parser.o

main.o: main.cpp project_structure.h
	g++ --std=c++17 -c main.cpp

project_structure.o: project_structure.cpp project_structure.h dir.h file_descriptor.h thread_pool.h parser.h
	g++ --std=c++17 -c project_structure.cpp

thread_pool.o: thread_pool.cpp thread_pool.h parser.h dir.h
	g++ --std=c++17 -c thread_pool.cpp

dir.o: dir.cpp dir.h
	g++ --std=c++17 -c dir.cpp

file_descriptor: file_descriptor.cpp file_descriptor.h parser.h
	g++ --std=c++17 -c file_descriptor.cpp

parser.o: parser.cpp parser.h file_descriptor.h project_structure.h dir.h thread_pool.h
	g++ --std=c++17 -c parser.cpp