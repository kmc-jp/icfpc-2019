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

class Solver{
private:
  std::vector<std::string> field;
  Point startpos;
  std::ofstream fout;
  int h, w;
  int len=1; // length of manipulator
  std::random_device seed_gen;
  std::mt19937 engine;
  const std::vector<int> vx = {1, 0, -1, 0};
  const std::vector<int> vy = {0, 1, 0, -1};
  const std::vector<char> vc = {'W','D','S','A'};
  std::vector<int> p={0, 1, 2, 3};
  
  std::vector<std::vector<int>> comp;
  std::vector<std::vector<int>> dir;
  std::vector<std::string> dest;
  // std::vector<Point> next;
  Point next;
  const int SD = 10;

  void print(std::string s){
    fout << s;
  }

  void useB(){
    len++;
    fout << "B(" << len << "," << 0 << ")";
  }

  bool calcComponent(){
    int ma = 1e9;
    int ansid = -1;
    comp = std::vector<std::vector<int>>(h, std::vector<int>(w, -1));
    std::queue<Point> bfs;
    for(int i = 0; i < h; i++){
      for(int j = 0; j < w; j++){
        if(field[i][j]!='#'&&field[i][j]!='+'&&comp[i][j]==-1){
          bfs.push({i, j});
          comp[i][j] = i*w+j;
          int count = 0;
          while(!bfs.empty()){
            auto now = bfs.front();
            bfs.pop();
            for(int k = 0; k < 4; k++){
              int nx = now.x + vx[k];
              int ny = now.y + vy[k];
              if(field[nx][ny]!='#'&&field[nx][ny]!='+'&&comp[nx][ny]==-1){
                comp[nx][ny] = i*w+j;
                bfs.push({nx, ny});
              }
            }
            count++;
          }
          if(ma>count){
            ma = count;
            ansid = i*w+j;
          }
        }
      }
    }
    for(int i = 0; i < h; i++){
      for(int j = 0; j < w; j++){
        if(comp[i][j]==ansid) comp[i][j]=1;
        else comp[i][j]=0;
      }
    }
    if(ansid>-1) return true;
    else return false;
  }

  void paint(int nowx, int nowy, std::vector<std::string> &f){
    for(int k = -1; k <= 1; k++){
      if(f[nowx+k][nowy+1]=='.') f[nowx+k][nowy+1] = '+';
    }
    for(int k = 1; k <= len; k++){
      if(f[nowx][nowy+k]=='#') break;
      if(f[nowx][nowy+k]=='.') f[nowx][nowy+k] = '+';
    }
  }

  void calcDestination(){
    dest = field;
    for(int j = 0; j < w; j++){
      for(int i = 0; i < h; i++){
        if(dest[i][j]!='#'&&dest[i][j]!='*'&&dest[i][j]!='+'&&comp[i][j]){
          paint(i, j, dest);
          dest[i][j] = '*';
        }
      }
    }
  }

  void calcNext(){
    std::queue<Point> bfs;
    bfs.push(startpos);
    dir = std::vector<std::vector<int>>(h, std::vector<int>(w, -1));
    dir[startpos.x][startpos.y] = SD;
    while(!bfs.empty()){
      auto now = bfs.front();
      bfs.pop();
      std::shuffle(p.begin(), p.end(), engine);
      for(int i = 0; i < 4; i++){
        int nx = now.x + vx[p[i]];
        int ny = now.y + vy[p[i]];
        if(field[nx][ny] != '#' && dir[nx][ny] == -1){
          dir[nx][ny] = p[i];
          bfs.push({nx, ny});
          if(dest[nx][ny] == '*'){
            next = {nx, ny};
            // next.push_back({nx, ny});
            return;
          }
        }
      }
    }
  }

  void calcRoute(){
    std::string move="";
    int nx = next.x, ny = next.y;
    int bcount = 0;
    while(dir[nx][ny] != SD){
      move.push_back(vc[dir[nx][ny]]);
      int d = dir[nx][ny];
      switch(field[nx][ny]){
      case '+':
      case '#':
        break;
      case 'B':
        field[nx][ny] = '+';
        bcount++;
        break;
      default:
        field[nx][ny] = '+';
        break;
      }
      paint(nx, ny, field);
      nx -= vx[d];
      ny -= vy[d];
    }
    std::reverse(move.begin(), move.end());
    print(move);
    for(int i = 0; i < bcount; i++) useB();
  }


public:
  Solver(const std::vector<std::string>& table, const Input& input, std::string outputfile){
    field = table;
    startpos = input.point;
    startpos.x++;
    startpos.y++;
    std::swap(startpos.x, startpos.y);
    fout = std::ofstream(outputfile);
    h = field.size();
    w = field[0].size();
    engine=std::mt19937(seed_gen());
    
    field[startpos.x][startpos.y]='+';
    for(int k = -1; k <= 1; k++){
      if(field[startpos.x+k][startpos.y+1]=='.'){
        field[startpos.x+k][startpos.y+1]='+';
      }
    }
  }

  ~Solver(){
    fout << std::endl;
    fout.close();
  }

  void solve(){
    std::queue<Point> bfs;
    while(1){
      if(!calcComponent()) return;
      calcDestination();
      calcNext();
      calcRoute();
      startpos = next;
    }
  }

};

int main() {

  int lid, rid;
  std::cin >> lid >> rid;

  for(int i = lid; i <= rid; i++){

    std::string id = std::to_string(i);
    std::string pad = "";
    while(pad.length()+id.length()<3) pad+="0";
    id=pad+id;
    std::string inputfile="problems/prob-"+id+".desc";
    std::string outputfile="solutions/prob-"+id+".sol";
    std::cout << inputfile << std::endl;
    std::ifstream fin(inputfile);

    std::string line;
    std::getline(fin, line);
    const auto input = parse(line);
    auto table = to_table(input);
    // output_table(table);

    Solver s(table, input, outputfile);
    s.solve();

    fin.close();

  }

  return 0;
}