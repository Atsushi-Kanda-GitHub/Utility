OBJS    = DoubleArray.o da_test.o
CPPFLAG = -Wall -O3

da:$(OBJS)
	g++ -o da $(OBJS)

DoubleArray.o: DoubleArray.cpp
	g++ $(CPPFLAG) -c DoubleArray.cpp

da_test.o: da_test.cpp
	g++ $(CPPFLAG) -c da_test.cpp

clean:
	rm -f $(OBJS)

