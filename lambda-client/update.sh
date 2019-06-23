#!/bin/bash -euvx
git fetch
git checkout origin/master -- ../generator/grid_generator.cpp
git checkout origin/yamunaku -- ../solver-mining.cpp
make -B all
