OBJS    = CoordinateDescent.o cd_test.o
CPPFLAG = -Wall -O3

da:$(OBJS)
	g++ -o cd $(OBJS)

CoordinateDescent.o: CoordinateDescent.cpp
	g++ $(CPPFLAG) -c CoordinateDescent.cpp

cd_test.o: cd_test.cpp
	g++ $(CPPFLAG) -c cd_test.cpp

clean:
	rm -f cd $(OBJS)

