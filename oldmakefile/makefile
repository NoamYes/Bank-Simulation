# Makefile for the bank program

CCC = g++ -pthread
CXXFLAGS = -Wall -std=c++0x
CXXLINK = $(CCC)
OBJS = mainBank.o
RM = rm -f

# all: Bank

Bank: $(OBJS) $(CXXLINK) -o Bank $(OBJS)

mainBank.o: mainBank.cpp

clean:

	$(RM) $(TARGET) *.o *~ "#"* Bank
