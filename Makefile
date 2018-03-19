CC = g++
CXX = g++
CXXFLAGS = -O3 -Wall -Wextra -g
LDLIBS += -lntl -lgmp -lm -lpthread

PROGS = lwe_oracle test_discrete_gaussian

.PHONY: all clean

all: $(PROGS)

test_discrete_gaussian: test_discrete_gaussian.o discrete_gaussian.o

lwe_oracle: lwe_oracle.o

test_discrete_gaussian.o: test_discrete_gaussian.cpp discrete_gaussian.hpp

lwe_oracle.o: lwe_oracle.cpp lwe_oracle.hpp

discrete_gaussian.o: discrete_gaussian.cpp discrete_gaussian.hpp

clean:
	rm -rf *.o $(PROGS)

