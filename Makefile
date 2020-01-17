
GXX49_VERSION := $(shell g++-4.9 --version 2>/dev/null)

ifdef GXX49_VERSION
	CXX_COMMAND := g++-4.9
else
	CXX_COMMAND := g++
endif

CXX = ${CXX_COMMAND} -std=c++11 -Wall

all: knapsack_timing run_test

run_test: knapsack_test
	./knapsack_test

headers: rubrictest.hpp timer.hpp maxprotein.hh knapsack.hpp

knapsack_test: headers knapsack_test.cpp
	${CXX} knapsack_test.cpp -o knapsack_test

knapsack_timing: headers knapsack_timing.cpp
	${CXX} knapsack_timing.cpp -o knapsack_timing

clean:
	rm -f knapsack_test knapsack_timing
