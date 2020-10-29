rule all:
	g++ -c RankSupport.cpp
	g++ -c SelectSupport.cpp
	g++ -c BitVector.cpp
	g++ BitVector.o SelectSupport.o RankSupport.o -o bitvector
	rm *.o

clean:
	rm *.o
