# Makefile for the Bank program
CCC = g++ -pthread
CXXFLAGS = -Wall -std=c++0x
CXXLINK = $(CCC)
OBJS = main.o Account.o Atm.o
RM = rm -f

#Default target
all: Bank

# Creating the  executable
Bank: $(OBJS)
	$(CXXLINK) -o Bank $(OBJS)
# Creating the object files
Account.o: Account.cpp Account.h
Atm.o: Atm.cpp Atm.h Account.h
main.o: main.cpp Atm.h Account.h


# Cleaning old files before  new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* Bank

