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
  const auto boosters = (line[index3+1]!='#'?parse_boosters(line, index3+1):std::vector<Booster>());
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

struct XorShift
{
    using result_type = uint32_t;
    result_type w = 123456789, x = 362436069, y = 521288629, z = 88675123;
    XorShift(result_type seed = time(nullptr))
    {
        w = seed;
        x = w << 13;
        y = (w >> 9) ^ (x << 6);
        z = y >> 7;
    }
    static const result_type min() { return 0; }
    static const result_type max() { return 0x7FFFFFFF; }
    result_type operator()()
    {
        result_type t = x ^ (x << 11);
        x = y;
        y = z;
        z = w;
        return w = (w ^ (w >> 19) ^ (t ^ (t >> 8)));
    }
    result_type rand()
    {
        result_type t = x ^ (x << 11);
        x = y;
        y = z;
        z = w;
        return w = (w ^ (w >> 19) ^ (t ^ (t >> 8)));
    }
    // [min,max] の整数値乱数
    result_type randInt(result_type min = 0, result_type max = 0x7FFFFFFF)
    {
        return rand() % (max - min + 1) + min;
    }
    // [min,max] の浮動小数点乱数
    double randDouble(double min = 0, double max = 1)
    {
        return (double)(rand() % 0xFFFF) / 0xFFFF * (max - min) + min;
    }
    // 変数をデフォルト値に設定する
    void SetDefault()
    {
        w = 123456789;
        x = 362436069;
        y = 521288629;
        z = 88675123;
    }
};

class Solver{
private:
  XorShift xsft;
  std::vector<std::string> field;
  int h, w;
  const std::vector<int> vx = {1, 0, -1, 0};
  const std::vector<int> vy = {0, 1, 0, -1};
  const std::vector<char> vc = {'W','D','S','A'};
  std::vector<int> p={0, 1, 2, 3};
  int step = 0;
  double powt, powu;

  std::vector<std::vector<int>> comp;
  std::vector<std::vector<int>> dir;
  std::vector<std::vector<int>> yosa;
  std::vector<std::vector<double>> cef;
  std::vector<std::string> dest;
  std::set<std::pair<int,int>> xpos;
  Point next;
  const int SD = 10;
  int renketu = 0;
  int bcount = 0;
  std::queue<int> cq,dq;
  int ccount = 0;
  int dcount = 0;
  int aaa;
  int distance;

  struct robot{
    Point pos;
    int len;
    int cl;
    int solidx;
    int chargesize;
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

  int updateCharge(){
    int c = 0;
    for(int i = 0; i < bot.size(); i++){
      bot[i].chargesize = 0;
      for(int x = 0; x < h; x++){
        for(int y = 0; y < w; y++){
          if(bot[i].mask[x][y]){
            if(field[x][y]!='#'&&field[x][y]!='+'){
              bot[i].chargesize++;
            }else{
              bot[i].mask[x][y] = 0;
            }
          }
        }
      }
      c+=bot[i].chargesize;
    }
    return c;
  }

  void calcComponent(int botid){
    comp = std::vector<std::vector<int>>(h, std::vector<int>(w, -1));
    std::map<int,int> compsize;
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
          compsize[i*w+j] = count;
        }
      }
    }
    for(int i = 0; i < h; i++){
      for(int j = 0; j < w; j++){
        if(comp[i][j]!=-1) comp[i][j]=compsize[comp[i][j]];
      }
    }
    return;
  }

  int paint(int botid, int nowx, int nowy, std::vector<std::string> &f){
    int count = 1;
    for(int k = -1; k <= 1; k++){
      if(f[nowx+k][nowy+1]=='.') f[nowx+k][nowy+1] = '+';
      if(field[nowx+k][nowy+1]) count++;
    }
    for(int k = 1; k <= bot[botid].len; k++){
      if(f[nowx][nowy+k]=='#') break;
      if(f[nowx][nowy+k]=='.') f[nowx][nowy+k] = '+', count++;
      if(field[nowx][nowy+k]) count++;
    }
    return count;
  }

  void calcDestination(int botid){
    dest = field;
    for(int j = 0; j < w; j++){
      for(int i = 0; i < h; i++){
        if(dest[i][j]!='#'&&dest[i][j]!='*'&&dest[i][j]!='+'&&bot[botid].mask[i][j]){
          yosa[i][j] = 1;
          for(int k = -1; k <= 1; k++){
            if(field[i][j+1]) yosa[i][j]++;
          }
          for(int k = 1; k <= bot[botid].len; k++){
            if(dest[i][j+k]=='#') break;
            if(field[i][j+k]) yosa[i][j]++;
          }
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
    std::vector<std::vector<int>> dist(h, std::vector<int>(w, -1));
    dir[startpos.x][startpos.y] = SD;
    dist[startpos.x][startpos.y] = 0;
    Point nxt = {-1, -1};
    // const int limit = aaa;
    const int limit = aaa;
    int tm = 100000000;
    double cost = 1e18;
    while(!bfs.empty()){
      auto now = bfs.front();
      bfs.pop();
      for(int i = 0; i < 4; i++){
        int nx = now.x + vx[p[i]];
        int ny = now.y + vy[p[i]];
        if(field[nx][ny] != '#' && dir[nx][ny] == -1){
          dir[nx][ny] = p[i];
          dist[nx][ny] = dist[now.x][now.y]+1;
          bfs.push({nx, ny});
          if(dest[nx][ny] == '*'){
            if(tm>limit) tm = limit;
            if(cost > std::pow(dist[nx][ny],powt)*comp[nx][ny]*cef[nx][ny]/std::pow(yosa[nx][ny],powu)){
              cost = std::pow(dist[nx][ny],powt)*comp[nx][ny]*cef[nx][ny]/std::pow(yosa[nx][ny],powu);
              nxt = {nx, ny};
            }
          }
          if(field[nx][ny]=='C'){
            nxt = {nx, ny};
            break;
          }else if(ccount > 0){
            if(xpos.find({nx,ny})!=xpos.end()){
              nxt = {nx, ny};
              break;
            }
          }
        }
      }
      tm--;
      if(tm==0) break;
    }
    next = nxt;
    distance = dist[next.x][next.y];
    if(dcount>0&&dist[nxt.x][nxt.y]>=100){
      while(!bfs.empty()) bfs.pop();
      bfs.push(startpos);
      std::vector<std::vector<int>> dir2(h, std::vector<int>(w, -1));
      dist=std::vector<std::vector<int>>(h, std::vector<int>(w, -1));
      dir2[startpos.x][startpos.y] = SD;
      dist[startpos.x][startpos.y] = 0;
      nxt={-1,-1};
      while(!bfs.empty()){
        auto now = bfs.front();
        bfs.pop();
        if(dist[now.x][now.y]>=30) break;
        for(int i = 0; i < 4; i++){
          int nx = now.x + vx[i];
          int ny = now.y + vy[i];
          if(0<=nx&&nx<h&&0<=ny&&ny<w){
            if(dir2[nx][ny] == -1){
              dir2[nx][ny] = i;
              dist[nx][ny] = dist[now.x][now.y]+1;
              bfs.push({nx, ny});
              if(dest[nx][ny] == '*'){
                nxt = {nx, ny};
                break;
              }
            }
          }
        }
      }
      if(nxt.x!=-1){
        dir = dir2;
        next = nxt;
        distance = dist[next.x][next.y];
        print(botid, 'L');
        dcount--;
      }
    }
    return;
  }

  void calcRoute(int botid){
    std::string move="";
    int nx = next.x, ny = next.y;
    int k = 0;
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
        cq.push(step+distance-k);
        break;
      case 'L':
        field[nx][ny] = '+';
        dq.push(step+distance-k);
        break;
      case 'X':
      default:
        field[nx][ny] = '+';
        break;
      }
      paint(botid, nx, ny, field);
      // output_table(field);
      nx -= vx[d];
      ny -= vy[d];
      k++;
    }
    bot[botid].pos = next;
    for(int i = move.length()-1; i >= 0; i--){
      print(botid, move[i]);
    }
    if(ccount>0){
      if(xpos.find({bot[botid].pos.x,bot[botid].pos.y})!=xpos.end()){
        for(int i = 0; i < ccount; i++){
          print(botid, 'C');
        }
        ccount=0;
      }
    }
    for(int i = 0; i < bcount; i++){
      print(botid, "B(" + std::to_string(bot[botid].len+1+i) + ",0)");
    }
    bcount = 0;
  }

  Point distantPoint(Point p, std::vector<std::vector<int>> &mask){
    std::queue<Point> bfs;
    std::vector<std::vector<int>> dist = std::vector<std::vector<int>>(h, std::vector<int>(w, -1));
    dist[p.x][p.y] = 0;
    bfs.push({p.x, p.y});
    Point q = {-1, -1};
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

  void assign(int botidp, int botidc){
    for(int x = 0; x < h; x++){
      for(int y = 0; y < w; y++){
        bot[botidp].mask[x][y]=bot[botidp].mask[x][y]&(field[x][y]!='#'&&field[x][y]!='+'?1:0);
      }
    }
    if (bot[botidp].chargesize < h+w) { // vary small to divide
      return;
    }
    Point one = distantPoint({bot[botidp].pos.x, bot[botidp].pos.y}, bot[botidp].mask);
    if(one.x == -1) return;
    Point two = distantPoint(one, bot[botidp].mask);
    if(two.x == -1) return;
    divideMask(one, two, botidp, botidc);
  }

  void divideMask(Point a, Point b, int botidp, int botidc){
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
    
    std::vector<std::vector<int>> &mask=bot[botidp].mask;
    std::vector<std::vector<int>> &newmask=bot[botidc].mask;
    int cp=0,cc=0;
    for(int i = 0; i < h; i++){
      for(int j = 0; j < w; j++){
        if(mask[i][j]){
          if(color[i][j]){
            mask[i][j]=1;
            newmask[i][j]=0;
            cp++;
          }else{
            mask[i][j]=0;
            newmask[i][j]=1;
            cc++;
          }
        }
      }
    }
    bot[botidp].chargesize=cp;
    bot[botidc].chargesize=cc;
    return;
  }

  void doMove(int botid, char c){
    if(c=='B'){
      bot[botid].len++;
      while(bot[botid].sol[bot[botid].solidx]!=')') bot[botid].solidx++;
      bot[botid].solidx++;
    }else if(c=='C'){
      bot.push_back({bot[botid].pos, 1, 0, 0, 0, "", std::vector<std::vector<int>>(h, std::vector<int>(w, 0))});
      assign(botid, bot.size()-1);
      bot[botid].solidx++;
    }else{
      bot[botid].solidx++;
    }
    if(!cq.empty()){
      if(cq.front()==step){
        cq.pop();
        ccount++;
      }
    }
    if(!dq.empty()){
      if(dq.front()==step){
        dq.pop();
        dcount++;
      }
    }
    std::cout << dcount << std::endl;
  }

public:
  Solver(const std::vector<std::string>& table, const Input& input, std::vector<int> supply){
    field = table;
    Point startpos = input.point;
    startpos.x++;
    startpos.y++;
    std::swap(startpos.x, startpos.y);
    h = field.size();
    w = field[0].size();


    aaa = xsft.rand()%(h*w)+1;
    powt = (double)(xsft.rand()%4000+500)/1000;
    powu = (double)(xsft.rand()%800+200)/1000;
    double keisu = 1+(double)(xsft.rand()%300)/10000;

    
    xpos = std::set<std::pair<int,int>>();
    for(int i = 0; i < h; i++){
      for(int j = 0; j < w; j++){
        if(field[i][j]=='X'){
          xpos.insert({i, j});
        }
      }
    }
    field[startpos.x][startpos.y]='+';
    for(int k = -1; k <= 1; k++){
      if(field[startpos.x+k][startpos.y+1]=='.'){
        field[startpos.x+k][startpos.y+1]='+';
      }
    }
    bot.push_back({startpos, 1, 0, 0, h*w, "", std::vector<std::vector<int>>(h, std::vector<int>(w, 1))});std::vector<std::vector<bool>> fl(h, std::vector<bool>(w, false));
    assert(supply.size() == 5);
    bcount = supply[0];
    ccount = supply[4];

    cef = std::vector<std::vector<double>>(h, std::vector<double>(w, 1));
    yosa = std::vector<std::vector<int>>(h, std::vector<int>(w, 1));
    std::queue<Point> bfs;
    for(int i = 0; i < h; i++){
      for(int j = 0; j < w; j++){
        if(field[i][j]=='#'){
          bfs.push({i,j});
          fl[i][j]=true;
        }
      }
    }
    while(!bfs.empty()){
      auto now = bfs.front();
      bfs.pop();
      for(int i = 0; i < 4; i++){
        int nx = now.x + vx[i];
        int ny = now.y + vy[i];
        if(0<=nx&&nx<h&&0<=ny&&ny<w){
          if(field[nx][ny]!='#'&&!fl[nx][ny]){
            fl[nx][ny]=true;
            cef[nx][ny]=keisu*cef[now.x][now.y];
            bfs.push({nx, ny});
          }
        }
      }
    }
  }

  ~Solver(){
    std::cout << std::endl;
  }

  int solve(){
    while(1){
      int sz = bot.size();
      int kk = updateCharge();
      // std::cout << step << " " << sz << " " << kk << std::endl;
      if(kk == 0) goto end;
      for(int i = 0; i < bot.size(); i++){
        if(bot[i].sol.length() > bot[i].solidx) continue;
        if(bot[i].chargesize==0){
          int ma=1, maid=-1;
          for(int j = 0; j < bot.size(); j++){
            if(i!=j){
              if(ma < bot[j].chargesize){
                ma = bot[j].chargesize;
                maid = j;
              }
            }
          }
          if(maid==-1){
            print(i, 'Z');
            continue;
          }else{
            assign(maid, i);
            if(bot[i].chargesize==0){
              print(i, 'Z');
              continue;
            }
            calcComponent(i);
          }
        }else{
          calcComponent(i);
        }
        calcDestination(i);
        calcNext(i);
        if(next.x==-1){
          print(i, 'Z');
          continue;
        }
        calcRoute(i);
      }
      for(int i = 0; i < sz; i++){
        doMove(i, bot[i].sol[bot[i].solidx]);
      }
      step++;
    }
    end:;
    std::vector<int> end(bot.size());
    for(int i = 0; i < bot.size(); i++){
      end[i]=bot[i].sol.length();
    }
    while(1){
      bool endflag = true;
      int sz = bot.size();
      for(int i = 0; i < bot.size(); i++){
        if(bot[i].sol.length() > bot[i].solidx) endflag=false;
      }
      if(endflag) break;
      for(int i = 0; i < sz; i++){
        if(bot[i].sol.length() > bot[i].solidx){
          doMove(i, bot[i].sol[bot[i].solidx]);
        }else{
          print(i, 'Z');
          bot[i].solidx++;
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
    
    // manip fast drill tele clone
    std::vector<int> supply = {0, 0, 0, 0, 0};

    Solver s(table, input, supply);
    // std::cout << s.solve() << std::endl;
    s.solve();

    // fin.close();

  // }

  return 0;
}
