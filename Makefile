CC = g++
CXX = g++
CXXFLAGS = -O3 -Wall -Wextra -g
LDLIBS += -lntl -lgmp -lm -lpthread

PROGS = test_lwe_oracle test_discrete_gaussian bkw

.PHONY: all clean

all: $(PROGS)

bkw: bkw.o lwe_oracle.o discrete_gaussian.o

test_discrete_gaussian: test_discrete_gaussian.o discrete_gaussian.o

test_lwe_oracle: test_lwe_oracle.o lwe_oracle.o discrete_gaussian.o

test_discrete_gaussian.o: test_discrete_gaussian.cpp discrete_gaussian.hpp

test_lwe_oracle.o: test_lwe_oracle.cpp lwe_oracle.hpp

lwe_oracle.o: lwe_oracle.cpp lwe_oracle.hpp

discrete_gaussian.o: discrete_gaussian.cpp discrete_gaussian.hpp

bkw.o: bkw.cpp lwe_oracle.hpp

clean:
	rm -rf *.o $(PROGS)

