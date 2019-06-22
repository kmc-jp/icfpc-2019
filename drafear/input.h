#ifndef INCLUDE_INPUT_H
#define INCLUDE_INPUT_H

#include <vector>
#include <string>

#include "defs.h"
#include "booster.h"

struct Booster {
  BoosterCode code;
  Point point;
};

struct Input {
  std::vector<Point> map;
  Point point;
  std::vector<std::vector<Point>> obstacles;
  std::vector<Booster> boosters;
};

Input parse(const std::string& line);

#endif
