all:
	g++ *.cpp -g -o main -std=c++11 -O3
no_opt:
	g++ *.cpp -g -o main -std=c++11