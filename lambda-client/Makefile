generator.a: ../generator/grid_generator.cpp
	g++ -O3 -std=c++17 $< -o $@

solver.a: ../solver-mining.cpp
	g++ -O3 -std=c++17 $< -o $@

all: generator.a solver.a
