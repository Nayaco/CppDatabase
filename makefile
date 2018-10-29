
PP = g++
CC = gcc

INCLUDE = -I"/mnt/d/GDCode/CPP/libevent/libevent-0/include"
LIB = -L"/mnt/d/GDCode/CPP/libevent/libevent-0/lib"

Hserver : Httpserver.o
	$(PP) -o Hserver Httpserver.o $(INCLUDE) $(LIB) -levent

Httpserver.o : Httpserver.cpp
	$(PP) -c Httpserver.cpp  $(INCLUDE) $(LIB)

Dserver : Dserver.o handle.o buffparser.o 
	$(PP) -o Dserver Dserver.o handle.o buffparser.o -lpthread

client : client.o buffparser.o rnplone.o
	$(PP) -o client client.o buffparser.o rnplone.o

client.o : client.cpp
	$(PP) -c client.cpp

Dserver.o : Dserver.cpp
	$(PP) -c Dserver.cpp

handle.o : handle.cpp
	$(PP) -c handle.h handle.cpp

buffparser.o : buffparser.cpp
	$(PP) -c buffparser.h buffparser.cpp

rnplone.o : rnplone.cpp
	$(PP) -c rnplone.h rnplone.cpp

clean :
	rm -f Dserver Dserver.o client.o handle.o buffparser.o handle.h.gch buffparser.h.gch Httpserver.o Hserver client client.o rnplone.o rnplone.h.gch
