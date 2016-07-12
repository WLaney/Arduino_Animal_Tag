CXX = g++
CFLAGS = -std=c++11 -Wall -pedantic

all:	main tests

main: parser.o main.o
	$(CXX) parser.o main.o -o main
	
tests: parser.o tests.o
	$(CXX) parser.o tests.o -o tests
	
parser.o: parser.cpp parser.hpp
	$(CXX) $(CFLAGS) -c parser.cpp
	
main.o: main.cpp
	$(CXX) $(CFLAGS) -c main.cpp
	
tests.o: tests.cpp
	$(CXX) $(CFLAGS) -c tests.cpp

clean:
	rm *.o main tests
