CXXFLAGS=-std=c++17 -O2 -mtune=native -march=native -Wall -Wextra

solver: main.o input.o
	$(CXX) -o $@ $(CXXFLAGS) $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $^

clean:
	-rm solver
