OBJS    = CRF.o DoubleArray.o crf_test.o
CPPFLAG = -Wall -O3
INCLUDE = -I../DoubleArray


crf:$(OBJS)
	g++ -o crf $(OBJS)

CRF.o: CRF.cpp
	g++ $(CPPFLAG) $(INCLUDE) -c CRF.cpp

DoubleArray.o: ../DoubleArray/DoubleArray.cpp
	g++ $(CPPFLAG) -c ../DoubleArray/DoubleArray.cpp

crf_test.o: crf_test.cpp
	g++ $(CPPFLAG) $(INCLUDE) -c crf_test.cpp

clean:
	rm -f crf $(OBJS)

