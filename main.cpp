#include <cassert>
#include <cctype>
#include <iostream>
#include <set>
#include <tuple>
#include <vector>

struct Point {
  int x, y;
};

enum class BoosterCode {
  B,
  F,
  L,
  R,
  X,
};

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

std::tuple<int, std::size_t> parse_num(const std::string& line, std::size_t i) {
  int num = 0;
  while (std::isdigit(line[i])) {
    num *= 10;
    num += line[i] - '0';
    ++i;
  }
  return std::make_tuple(num, i);
}

std::tuple<Point, std::size_t> parse_point(const std::string& line, const std::size_t i) {
  assert(line[i] == '(');
  const auto [x, index] = parse_num(line, i+1);
  assert(line[index] == ',');
  const auto [y, index2] = parse_num(line, index+1);
  assert(line[index2] == ')');
  return std::make_tuple(Point { x, y }, index2+1);
}

std::tuple<std::vector<Point>, std::size_t> parse_map(const std::string& line, std::size_t i) {
  std::vector<Point> res;
  while (true) {
    const auto [point, index] = parse_point(line, i);
    res.push_back(point);
    i = index;
    if (line[index] != ',') break;
    ++i;
  }
  return std::make_tuple(res, i);
}

std::tuple<std::vector<std::vector<Point>>, std::size_t> parse_obstacles(const std::string& line, std::size_t i) {
  std::vector<std::vector<Point>> res;
  while (true) {
    const auto [map, index] = parse_map(line, i);
    res.push_back(map);
    i = index;
    if (line[index] != ';') break;
    ++i;
  }
  return std::make_tuple(res, i);
}

BoosterCode parse_booster_code(const std::string& line, const std::size_t i) {
  switch (line[i]) {
    case 'B': return BoosterCode::B;
    case 'F': return BoosterCode::F;
    case 'L': return BoosterCode::L;
    case 'R': return BoosterCode::R;
    case 'X': return BoosterCode::X;
    default: assert(false);
  }
  return BoosterCode::X;
}

std::tuple<Booster, std::size_t> parse_booster(const std::string& line, const std::size_t i) {
  const auto booster_code = parse_booster_code(line, i);
  const auto [point, index] = parse_point(line, i+1);
  return std::make_tuple(Booster { booster_code, point }, index);
}

std::vector<Booster> parse_boosters(const std::string& line, std::size_t i) {
  std::vector<Booster> res;
  while (true) {
    const auto [booster, index] = parse_booster(line, i);
    res.push_back(booster);
    i = index;
    if (index >= line.size() || line[index] != ';') break;
    ++i;
  }
  return res;
}

Input parse(const std::string& line) {
  const auto [map, index] = parse_map(line, 0);
  assert(line[index] == '#');
  const auto [point, index2] = parse_point(line, index+1);
  assert(line[index2] == '#');
  const auto [obstacles, index3] = parse_obstacles(line, index2+1);
  assert(line[index3] == '#');
  const auto boosters = parse_boosters(line, index3+1);
  return Input { map, point, obstacles, boosters };
}

std::vector<std::string> to_table(const Input& input) {
  auto w = 0;
  auto h = 0;
  for (auto&& point : input.map) {
    w = std::max(w, point.x);
    h = std::max(h, point.y);
  }
  w += 2;
  h += 2;
  std::vector<std::string> table(h, std::string(w, '#'));
  std::set<std::tuple<int, int, int>> s;
  for (std::size_t i = 0; i < input.map.size(); ++i) {
    const auto p = input.map[i];
    const auto q = input.map[(i+1)%input.map.size()];
    if (p.y != q.y) continue; // continue if vertical line
    s.emplace(p.y+1, p.x+1, q.x+1);
  }
  for (auto [y, sx, tx] : s) {
    if (sx < tx) {
      for (int i = y; i < h-1; ++i) {
        for (int j = sx; j < tx; ++j) {
          table[i][j] = '.';
        }
      }
    } else {
      for (int i = y; i < h-1; ++i) {
        for (int j = tx; j < sx; ++j) {
          table[i][j] = '#';
        }
      }
    }
  }
  for (auto&& obstacle : input.obstacles) {
    s.clear();
    for (std::size_t i = 0; i < obstacle.size(); ++i) {
      const auto p = obstacle[i];
      const auto q = obstacle[(i+1)%obstacle.size()];
      if (p.y != q.y) continue; // continue if vertical line
      s.emplace(p.y+1, p.x+1, q.x+1);
    }
    for (auto [y, sx, tx] : s) {
      if (sx < tx) {
        for (int i = y; i < h-1; ++i) {
          for (int j = sx; j < tx; ++j) {
            table[i][j] = '#';
          }
        }
      } else {
        for (int i = y; i < h-1; ++i) {
          for (int j = tx; j < sx; ++j) {
            table[i][j] = '.';
          }
        }
      }
    }
  }
  for (auto&& booster : input.boosters) {
    char code = 'X';
    switch (booster.code) {
      case BoosterCode::B: code = 'B'; break;
      case BoosterCode::F: code = 'F'; break;
      case BoosterCode::L: code = 'L'; break;
      case BoosterCode::R: code = 'R'; break;
      case BoosterCode::X: code = 'X'; break;
      default:;
    }
    table[booster.point.y+1][booster.point.x+1] = code;
  }
  return table;
}

void output_table(const std::vector<std::string>& table) {
  for (int i = table.size() - 1; i >= 0; --i) {
    std::cout << table[i] << std::endl;
  }
}

int main() {
  std::string line;
  std::getline(std::cin, line);
  const auto input = parse(line);
  auto table = to_table(input);
  output_table(table);
  return 0;
}
