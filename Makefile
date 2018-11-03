all:
	g++ *.cpp -o raytracer -std=c++11 -O3
no_opt:
	g++ *.cpp -o raytracer -std=c++11
debug:
	g++ *.cpp -g -o raytracer -std=c++11
