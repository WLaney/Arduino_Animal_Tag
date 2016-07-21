CXX = g++
CFLAGS = -std=c++11 -Wall -pedantic
SRC=src

all:	main tests

main: parser.o main.o
	$(CXX) $(SRC)/parser.o $(SRC)/main.o -o shark-parser
	
tests: parser.o tests.o
	$(CXX) $(SRC)/parser.o $(SRC)/tests.o -o tests
	
parser.o: $(SRC)/parser.cpp $(SRC)/parser.hpp
	$(CXX) $(CFLAGS) -c $(SRC)/parser.cpp -o $(SRC)/parser.o
	
main.o: $(SRC)/main.cpp
	$(CXX) $(CFLAGS) -c $(SRC)/main.cpp -o $(SRC)/main.o
	
tests.o: $(SRC)/tests.cpp
	$(CXX) $(CFLAGS) -c $(SRC)/tests.cpp -o $(SRC)/tests.o

clean:
	rm $(SRC)/*.o shark-parser tests
