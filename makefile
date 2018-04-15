Monitor.o: Monitor.cpp
	g++ -c -o Monitor.o Monitor.cpp -lzmq -std=c++11

Token.o: Token.cpp
	g++ -c -o Token.o Token.cpp -lzmq -std=c++11

all: Monitor.o Token.o main.cpp
	g++ -o main main.cpp Monitor.o Token.o -std=c++11

clean:
	rm Monitor.o
