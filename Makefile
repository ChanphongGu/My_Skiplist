main: main.cpp
	g++ -o main main.cpp --std=c++11 -pthread

clean:
	rm -rf *.o
	rm -rf dumpSKL
	rm -rf main
