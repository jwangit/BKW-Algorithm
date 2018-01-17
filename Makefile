CC = g++
CXX = g++
CXXFLAGS = -O3 -Wall -Wextra -g
LDLIBS += -lntl -lgmp -lm -lpthread

PROGS = lwe_oracle discrete_gaussian test

.PHONY: all clean

all: $(PROGS)

lwe_oracle: lwe_oracle.o

discrete_gaussian: discrete_gaussian.o

test: test.o

lwe_oracle.o: lwe_oracle.cpp lwe_oracle.hpp

discrete_gaussian.o: discrete_gaussian.cpp discrete_gaussian.hpp

test.o: test.cpp

clean:
	rm -rf *.o $(PROGS)

