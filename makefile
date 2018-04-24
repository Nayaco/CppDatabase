Dserver : Dserver.o handle.o buffparser.o 
	g++ -o Dserver Dserver.o handle.o buffparser.o -lpthread

client : client.o buffparser.o rnplone.o
	g++ -o client client.o buffparser.o rnplone.o

client.o : client.cpp
	g++ -c client.cpp

Dserver.o : Dserver.cpp
	g++ -c Dserver.cpp

handle.o : handle.cpp
	g++ -c handle.h handle.cpp

buffparser.o : buffparser.cpp
	g++ -c buffparser.h buffparser.cpp

rnplone.o : rnplone.cpp
	g++ -c rnplone.h rnplone.cpp

clean :
	rm Dserver Dserver.o client.o handle.o buffparser.o handle.h.gch buffparser.h.gch
