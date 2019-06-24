#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
using namespace std;
#include <cassert>
#include <cctype>
#include <iostream>
#include <set>
#include <tuple>
#include <vector>
#include <queue>
#include <algorithm>
#include <random>
#include <fstream>
#include <utility>
#include <map>
#include <math.h>
#include <time.h>

struct Point {
  int x, y;
};

enum class BoosterCode {
  B,
  F,
  L,
  R,
  X,
  C,
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
  while (line[i] == '(') {
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
    case 'C': return BoosterCode::C;
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
            table[i][j]+='#'-'.';
          }
        }
      } else {
        for (int i = y; i < h-1; ++i) {
          for (int j = tx; j < sx; ++j) {
            table[i][j]-='#'-'.';
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
      case BoosterCode::C: code = 'C'; break;
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
  vector<int> score(300);
  for(int i = 1; i <= 300; i++){
    std::string id = std::to_string(i);
    std::string pad = "";
    while(pad.length()+id.length()<3) pad+="0";
    id=pad+id;
    std::string inputfile="benchmarker/bestzip/prob-"+id+".sol";
    std::cout << inputfile << std::endl;
    std::ifstream fin(inputfile);
    char c;
    int step=0;
    while(fin >> c){
      if('A'<=c&&c<='Z') step++;
      if(c=='#') break;
    }
    fin.close();
    score[i-1]=step;
  }
  vector<int> scoreuse(300);
  for(int i = 1; i <= 300; i++){
    std::string id = std::to_string(i);
    std::string pad = "";
    while(pad.length()+id.length()<3) pad+="0";
    id=pad+id;
    std::string inputfile="benchmarker-buy/bestzip/prob-"+id+".sol";
    std::cout << inputfile << std::endl;
    std::ifstream fin(inputfile);
    char c;
    int step=0;
    while(fin >> c){
      if('A'<=c&&c<='Z') step++;
      if(c=='#') break;
    }
    fin.close();
    scoreuse[i-1]=step;
  }
  vector<double> hw(300);
  hw[0]=21;
  for(int i = 2; i <= 300; i++){
    std::string id = std::to_string(i);
    std::string pad = "";
    while(pad.length()+id.length()<3) pad+="0";
    id=pad+id;
    std::string inputfile="problems/prob-"+id+".desc";
    std::cout << inputfile << std::endl;
    std::ifstream fin(inputfile);
    std::string line;
    std::getline(fin, line);
    fin.close();
    const auto input = parse(line);
    auto table = to_table(input);
    hw[i-1]=((int)table.size()-1)*((int)table[0].size()-1);
  }
  vector<double> s(300);
  ofstream fout("result");
  // fout << "id hw score score-use delta" << endl;
  for(int i=0;i<300;i++){
    s[i] = 1000*log2(hw[i])*(1-(double)scoreuse[i]/(double)score[i]);
  //   fout << i+1 << " " << hw[i] << " " << score[i] << " " << scoreuse[i] << " " << s[i] << endl;
  }
  // fout << endl << endl << "sort" << endl << endl;
  vector<int> id(300);
  iota(id.begin(),id.end(),0);
  sort(id.begin(),id.end(),[&](int l, int r){
    return s[l]>s[r];
  });
  for(int i=0;i<145;i++){
    if(s[id[i]]<4000) break;
    fout << id[i] << " ";
  }
  fout <<"-1" << endl;
  // fout << "id hw score score-use delta" << endl;
  for(int i=0;i<300;i++){
    fout << id[i]+1 << " " << hw[id[i]] << " " << score[id[i]] << " " << scoreuse[id[i]] << " " << s[id[i]] << endl;
  }
}