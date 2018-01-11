CC = g++
CXX = g++
CXXFLAGS = -O3 -Wall -Wextra -g
LDLIBS += -lntl -lgmp -lm -lpthread

PROGS = lwe_oracle 

.PHONY: all clean

all: $(PROGS)

lwe_oracle: lwe_oracle.o

lwe_oracle.o: lwe_oracle.cpp lwe_oracle.hpp

clean:
	rm -rf *.o $(PROGS)

