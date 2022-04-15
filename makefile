main:main.cpp info.cpp server.cpp database.cpp server.h info.h database.h
	g++ main.cpp info.cpp server.cpp database.cpp -o main -lpthread -levent -lmysqlclient -std=c++11 -ljsoncpp
