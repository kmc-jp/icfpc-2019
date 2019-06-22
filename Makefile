CXXFLAGS=-std=c++17 -O2 -mtune=native -march=native -Wall -Wextra

solver: main.cpp
	$(CXX) -o $@ $(CXXFLAGS) $^

clean:
	-rm solver