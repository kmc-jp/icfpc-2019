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

class Solver{
private:
  std::vector<std::string> field;
  int h, w;
  std::random_device seed_gen;
  std::mt19937 engine;
  const std::vector<int> vx = {1, 0, -1, 0};
  const std::vector<int> vy = {0, 1, 0, -1};
  const std::vector<char> vc = {'W','D','S','A'};
  std::vector<int> p={0, 1, 2, 3};
  int step = 0;
  int ccc=0;

  std::vector<std::vector<int>> comp;
  std::vector<std::vector<int>> dir;
  std::vector<std::string> dest;
  std::set<std::pair<int,int>> xpos;
  Point next;
  const int SD = 10;
  int renketu = 0;

  struct robot{
    Point pos;
    int len;
    int cl;
    std::queue<char> task;
    std::string sol;
    std::vector<std::vector<int>> mask;
  };
  std::vector<robot> bot;

  void print(int botid, std::string s){
    bot[botid].sol+=s;
  }
  void print(int botid, char c){
    bot[botid].sol.push_back(c);
  }

  bool calcComponent(int botid){
    int ma = 1e9;
    int ansid = -1;
    comp = std::vector<std::vector<int>>(h, std::vector<int>(w, -1));
    std::queue<Point> bfs;
    for(int i = 0; i < h; i++){
      for(int j = 0; j < w; j++){
        if(field[i][j]!='#'&&field[i][j]!='+'&&comp[i][j]==-1&&bot[botid].mask[i][j]){
          bfs.push({i, j});
          comp[i][j] = i*w+j;
          int count = 0;
          while(!bfs.empty()){
            auto now = bfs.front();
            bfs.pop();
            for(int k = 0; k < 4; k++){
              int nx = now.x + vx[k];
              int ny = now.y + vy[k];
              if(field[nx][ny]!='#'&&field[nx][ny]!='+'&&comp[nx][ny]==-1&&bot[botid].mask[nx][ny]){
                comp[nx][ny] = i*w+j;
                bfs.push({nx, ny});
              }
            }
            count++;
          }
          if(ma>count){
            ma = count;
            renketu = count;
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

  void resetMask(int botid){
    for(int x = 0; x < h; x++){
      for(int y = 0; y < w; y++){
        bot[botid].mask[x][y]=(field[x][y]!='#'&&field[x][y]!='+'?1:0);
      }
    }
  }

  void paint(int botid, int nowx, int nowy, std::vector<std::string> &f){
    for(int k = -1; k <= 1; k++){
      if(f[nowx+k][nowy+1]=='.') f[nowx+k][nowy+1] = '+';
    }
    for(int k = 1; k <= bot[botid].len; k++){
      if(f[nowx][nowy+k]=='#') break;
      if(f[nowx][nowy+k]=='.') f[nowx][nowy+k] = '+';
    }
  }

  void calcDestination(int botid){
    dest = field;
    for(int j = 0; j < w; j++){
      for(int i = 0; i < h; i++){
        if(dest[i][j]!='#'&&dest[i][j]!='*'&&dest[i][j]!='+'&&comp[i][j]){
          paint(botid, i, j, dest);
          dest[i][j] = '*';
        }
      }
    }
  }

  void calcNext(int botid){
    std::queue<Point> bfs;
    Point startpos = bot[botid].pos;
    bfs.push(startpos);
    dir = std::vector<std::vector<int>>(h, std::vector<int>(w, -1));
    dir[startpos.x][startpos.y] = SD;
    Point nxt = {-1, -1};
    const int limit = 500;
    const int th = 50;
    int tm = 100000000;
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
          if(dest[nx][ny] == '*' && bot[botid].mask[nx][ny]==1){
            if(nxt.x == -1){
              nxt = {nx, ny};
              if(ccc==0||renketu<=th){
                next = nxt;
                return;
              }
              tm = limit;
            }
          }else if(field[nx][ny]=='C'){
            nxt = {nx, ny};
          }else if(bot[botid].cl > 0){
            if(xpos.find({nx,ny})!=xpos.end()){
              nxt = {nx, ny};
            }
          }
        }
      }
      tm--;
      if(tm==0) break;
    }
    next = nxt;
    return;
  }

  void calcRoute(int botid){
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
      case 'C':
        field[nx][ny] = '+';
        bot[botid].cl++;
      case 'X':
      default:
        field[nx][ny] = '+';
        break;
      }
      paint(botid, nx, ny, field);
      // output_table(field);
      nx -= vx[d];
      ny -= vy[d];
    }
    bot[botid].pos = next;
    for(int i = move.length()-1; i >= 0; i--){
      bot[botid].task.push(move[i]);
    }
    if(bot[botid].cl>0){
      if(xpos.find({bot[botid].pos.x,bot[botid].pos.y})!=xpos.end()){
        for(int i = 0; i < bot[botid].cl; i++){
          bot[botid].task.push('C');
        }
      }
    }
    for(int i = 0; i < bcount; i++){
      bot[botid].task.push('B');
    }
    bcount = 0;
  }

  
  Point distantPoint(Point p, std::vector<std::vector<int>> &mask){
    std::queue<Point> bfs;
    std::vector<std::vector<int>> dist = std::vector<std::vector<int>>(h, std::vector<int>(w, -1));
    dist[p.x][p.y] = 0;
    bfs.push({p.x, p.y});
    Point q;
    while(!bfs.empty()){
      auto now = bfs.front();
      bfs.pop();
      for(int i = 0; i < 4; i++){
        int nx = now.x + vx[i];
        int ny = now.y + vy[i];
        if(field[nx][ny]!='#'&&dist[nx][ny]==-1){
          dist[nx][ny] = dist[now.x][now.y]+1;
          bfs.push({nx, ny});
        }
      }
      if(mask[now.x][now.y]==1) q = now;
    }
    return q;
  }

  void divideMask(Point a, Point b, std::vector<std::vector<int>> &mask, std::vector<std::vector<int>> &newmask){
    std::queue<Point> bfs;
    std::vector<std::vector<int>> color = std::vector<std::vector<int>>(h, std::vector<int>(w, -1));
    color[a.x][a.y] = 0;
    color[b.x][b.y] = 1;
    bfs.push({a.x, a.y});
    bfs.push({b.x, b.y});
    while(!bfs.empty()){
      auto now = bfs.front();
      bfs.pop();
      for(int i = 0; i < 4; i++){
        int nx = now.x + vx[i];
        int ny = now.y + vy[i];
        if(field[nx][ny]!='#'&&color[nx][ny]==-1){
          color[nx][ny] = color[now.x][now.y];
          bfs.push({nx, ny});
        }
      }
    }
    
    for(int i = 0; i < h; i++){
      for(int j = 0; j < w; j++){
        if(mask[i][j]){
          if(color[i][j]){
            mask[i][j]=1;
            newmask[i][j]=0;
          }else{
            mask[i][j]=0;
            newmask[i][j]=1;
          }
        }
      }
    }
    
        // for(int x = h-1;x>=0;x--){
        //   for(int y =0;y<w;y++){
        //     std::cout << mask[x][y];
        //   }
        //   std::cout << std::endl;
        // }
        // std::cout << std::endl;
        // for(int x = h-1;x>=0;x--){
        //   for(int y =0;y<w;y++){
        //     std::cout << newmask[x][y];
        //   }
        //   std::cout << std::endl;
        // }
        // std::cout << std::endl;
    return;
  }

  bool useItem(int botid, char c){
    switch(c){
    case 'B':
      bot[botid].len++;
      print(botid, "B(" + std::to_string(bot[botid].len) + ",0)");
      return true;
    case 'C':
      print(botid, "C");
      for(int i=0;i<bot[botid].cl;i++){
        for(int x = 0; x < h; x++){
          for(int y = 0; y < w; y++){
            bot[botid].mask[x][y]=bot[botid].mask[x][y]*(field[x][y]!='#'&&field[x][y]!='+'?1:0);
          }
        }
        Point one = distantPoint({bot[botid].pos.x, bot[botid].pos.y}, bot[botid].mask);
        Point two = distantPoint(one, bot[botid].mask);
        bot.push_back({bot[botid].pos, 1, 0, std::queue<char>(), "", std::vector<std::vector<int>>(h, std::vector<int>(w, 0))});
        divideMask(one, two, bot[botid].mask, bot[bot.size()-1].mask);
      }
      bot[botid].cl=0;
      ccc--;
      return true;
    }
    return false;
  }

public:
  Solver(const std::vector<std::string>& table, const Input& input){
    field = table;
    Point startpos = input.point;
    startpos.x++;
    startpos.y++;
    std::swap(startpos.x, startpos.y);
    h = field.size();
    w = field[0].size();
    engine=std::mt19937(seed_gen());
    xpos = std::set<std::pair<int,int>>();
    for(int i = 0; i < h; i++){
      for(int j = 0; j < w; j++){
        if(field[i][j]=='X'){
          xpos.insert({i, j});
        }else if(field[i][j]=='C'){
          ccc++;
        }
      }
    }
    field[startpos.x][startpos.y]='+';
    for(int k = -1; k <= 1; k++){
      if(field[startpos.x+k][startpos.y+1]=='.'){
        field[startpos.x+k][startpos.y+1]='+';
      }
    }
    bot.push_back({startpos, 1, 0, std::queue<char>(), "", std::vector<std::vector<int>>(h, std::vector<int>(w, 1))});
  }

  ~Solver(){
    std::cout << std::endl;
  }

  int solve(){
    while(1){
      int sz = bot.size();
      for(int i = 0; i < bot.size(); i++){
        if(!bot[i].task.empty()) continue;
        if(!calcComponent(i)){
          resetMask(i);
          if(!calcComponent(i)){
            goto end;
          }
        }
        calcDestination(i);
        calcNext(i);
        calcRoute(i);
      }
      for(int i = 0; i < sz; i++){
        // for(int x = h-1;x>=0;x--){
        //   for(int y =0;y<w;y++){
        //     std::cout << bot[i].mask[x][y];
        //   }
        //   std::cout << std::endl;
        // }
        // std::cout << std::endl;
        bool wt = useItem(i, bot[i].task.front());
        if(!wt) print(i, bot[i].task.front());
        bot[i].task.pop();
      }
      // output_table(field);
      step++;
    }
    end:;
    while(1){
      bool endflag = true;
      int sz = bot.size();
      for(int i = 0; i < bot.size(); i++){
        if(!bot[i].task.empty()) endflag=false;
      }
      if(endflag) break;
      for(int i = 0; i < sz; i++){
        if(!bot[i].task.empty()){
          bool wt = useItem(i, bot[i].task.front());
          if(!wt) print(i, bot[i].task.front());
          bot[i].task.pop();
        }else{
          print(i, 'Z');
        }
      }
      step++;
    }
    for(int i = 0; i < bot.size()-1; i++){
      std::cout << bot[i].sol << "#";
    }
    std::cout << bot[bot.size()-1].sol << std::endl;
    return step;
  }

};

int main() {

  // int lid, rid;
  // std::cin >> lid >> rid;

  // for(int i = lid; i <= rid; i++){

    // std::string id = std::to_string(i);
    // std::string pad = "";
    // while(pad.length()+id.length()<3) pad+="0";
    // id=pad+id;
    // std::string inputfile="problems/prob-"+id+".desc";
    // std::string outputfile="solutions/prob-"+id+".sol";
    // std::cout << inputfile << std::endl;
    // std::ifstream fin(inputfile);

    std::string line;
    std::getline(std::cin, line);
    const auto input = parse(line);
    auto table = to_table(input);
    // output_table(table);

    Solver s(table, input);
    // std::cout << s.solve() << std::endl;
    s.solve();

    // fin.close();

  // }

  return 0;
}