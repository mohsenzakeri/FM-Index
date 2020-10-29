rule all:
	g++ -c RankSupport.cpp
	g++ -c SelectSupport.cpp
	g++ -c BitVector.cpp
	g++ -c main.cpp
	g++ main.o BitVector.o SelectSupport.o RankSupport.o -o build/bitvector
	rm *.o

clean:
	rm *.o
