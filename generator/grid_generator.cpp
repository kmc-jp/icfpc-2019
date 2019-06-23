#include <vector>
#include <string>
#include <cassert>
#include <tuple>
#include <iostream>
#include <algorithm>
#include <map>
#include <set>

// !!!
using namespace std;

struct Point
{
    int x, y;
};

std::tuple<int, std::size_t> parse_num(const std::string &line, std::size_t i)
{
    int num = 0;
    while (std::isdigit(line[i]))
    {
        num *= 10;
        num += line[i] - '0';
        ++i;
    }
    return std::make_tuple(num, i);
}

std::tuple<Point, std::size_t> parse_point(const std::string &line, const std::size_t i)
{
    assert(line[i] == '(');
    const auto [x, index] = parse_num(line, i + 1);
    assert(line[index] == ',');
    const auto [y, index2] = parse_num(line, index + 1);
    assert(line[index2] == ')');
    return std::make_tuple(Point{x, y}, index2 + 1);
}

// puzzle パーサー
class PuzzleParser
{
public:
    vector<Point> iSqs, oSqs;
    int bNum, eNum;
    vector<int> nums;
    PuzzleParser(const string &task)
    {
        int index = 0;
        // bNum
        {
            auto res = parse_num(task, index);
            bNum = get<0>(res);
            index = get<1>(res);
            index++; // ,
        }
        // eNum
        {
            auto res = parse_num(task, index);
            eNum = get<0>(res);
            index = get<1>(res);
            index++; // ,
        }
        for (int i = 0; i < 9; i++)
        {
            auto res = parse_num(task, index);
            nums.emplace_back(get<0>(res));
            index = get<1>(res);
            index++; // ,
        }
        while (true)
        {
            // assert(iSqs.size() == 0)
            auto res = parse_point(task, index);
            iSqs.emplace_back(get<0>(res));
            index = get<1>(res);
            if (task[index] == '#')
            {
                index++;
                break;
            }
            index++; // ,
        }
        while (index < task.size())
        {
            // assert(oSqs.size() == 0)
            auto res = parse_point(task, index);
            oSqs.emplace_back(get<0>(res));
            index = get<1>(res);
            if (task[index] == '#')
            {
                index++;
                break;
            }
            index++; // ,
        }
    }
};

class Generator
{
    using P = pair<int, int>;
    // 作る盤面の座標最大値(min:= tSize - floor(0.1*tSize), area_min:= floor(0.2*tSize-2))
    int tSize; // UNREFERENCED
    // 盤面の最小/最大頂点数
    int vMin, vMax;
    // ブラシ, 倍速, ドリル, ワープ, クローン, Xマス
    int mNum, fNum, dNum, rNum, cNum, xNum;
    // 必須マス, 禁止マス
    std::vector<Point> iSqs, oSqs;
    const string items = "$$$BFLRCX";
    std::vector<int> _nums;    // 数値パース結果
    vector<vector<char>> _ret; // generate 結果

public:
    // 数値パース結果, 必須マス, 禁止マス
    Generator(const vector<int> &nums, std::vector<Point> &is, std::vector<Point> &os)
    {
        iSqs = is;
        oSqs = os;
        // 座標でソート
        sort(oSqs.begin(), oSqs.end(), [](Point l, Point r) { return l.x != r.x ? l.x < r.x : l.y < r.y; });
        _nums = nums;
        assert(nums.size() == 9);
        tSize = nums[0];
        vMin = nums[1];
        vMax = nums[2];
        mNum = nums[3];
        fNum = nums[4];
        dNum = nums[5];
        rNum = nums[6];
        cNum = nums[7];
        xNum = nums[8];
    }

    // グリッド(多角形)を反時計回りに辿る経路を返す
    // マスの中心ではなく, 通る辺の座標
    vector<P> route(const vector<vector<char>> &field)
    {
        int dx[4] = {1, 0, -1, 0}, dy[4] = {0, -1, 0, 1};
        int dx4[4] = {1, 1, 0, 0}, dy4[4] = {1, 0, 0, 1};
        map<P, P> graph;
        int tSize = field.size();
        assert(field[0].size() == tSize);
        // グリッド外, もしくは # マスから反時計回りに見て辺を繋ぐ
        for (int i = -1; i <= tSize; i++)
        {
            for (int j = -1; j <= tSize; j++)
            {
                if (i != -1 && i != tSize && j != -1 && j != tSize && field[i][j] != '#')
                    continue;
                for (int k = 0; k < 4; k++)
                {
                    int nx1 = i + dx[k], ny1 = j + dy[k];
                    if (nx1 < 0 || tSize <= nx1 || ny1 < 0 || tSize <= ny1)
                        continue;
                    if (field[nx1][ny1] == '#')
                        continue;
                    P st = P(i + dx4[k], j + dy4[k]), gt = P(i + dx4[(k + 1) % 4], j + dy4[(k + 1) % 4]);
                    graph[st] = gt;
                }
            }
        }
        auto itr = graph.begin();
        vector<P> ret = {(*itr).first};
        set<P> visit = {(*itr).first};
        P cur = (*itr).second;
        while (cur != ret[0])
        {
            assert(graph.find(cur) != graph.end());
            ret.emplace_back(cur);
            visit.emplace(cur);
            cur = graph[cur];
        }
        return ret;
    }

    // 生成したグリッド出力
    void printret()
    {
        for (auto v : _ret)
        {
            for (auto x : v)
                cout << x;
            cout << endl;
        }
    }

    string generate()
    {
        // 各マスは未定
        auto ret = vector<vector<char>>(tSize, vector<char>(tSize, '*'));
        // 必須マスを . に
        set<int> isx; // 必須マスの存在する行番号
        for (auto p : iSqs)
        {
            ret[p.x][p.y] = '.';
            isx.insert(p.x);
        }
        cerr << "iSqs done" << endl;
        int corner = 4, cmax = (vMin + vMax) / 2; // 暫定頂点数, 最大頂点数(頂点数を雑に数えているのでマージンを取っている)
        // 禁止マスを # に
        set<int> usedx = {-1, tSize}; // 禁止マスが存在する行番号(と番兵)
        for (auto p : oSqs)
        {
            ret[p.x][p.y] = '#';
            usedx.insert(p.x);
            // 右か左の端に繋げる
            bool l = true, r = true;
            for (int j = 0; j < tSize; j++)
            {
                if (ret[p.x][j] != '.')
                    continue;
                if (j < p.y)
                    l = false;
                else if (p.y < j)
                    r = false;
            }
            assert(l || r);
            int up_l = -1, up_r = tSize + 1, down_l = -1, down_r = tSize + 1;
            if (l && r)
            {
                // more close
                if (p.y < tSize / 2)
                {
                    for (int j = p.y; j >= 0; j--)
                    {
                        ret[p.x][j] = '#';
                    }
                }
                else
                {
                    for (int j = p.y; j < tSize; j++)
                    {
                        ret[p.x][j] = '#';
                    }
                }
            }
            else if (l)
            {
                for (int j = p.y; j >= 0; j--)
                {
                    ret[p.x][j] = '#';
                }
            }
            else // r
            {
                for (int j = p.y; j < tSize; j++)
                {
                    ret[p.x][j] = '#';
                }
            }
            corner += 2; // 怪しい
        }
        cerr << "oSqs done" << endl;
        // 角の個数を調整 適当な空間に 長い棒と1*1の突起を生やす
        // !!! 禁止マスが増える, 頂点数制約が厳しくなると壊れる
        int col = 0; // col 行目までは見た
        for (int i = 3; i < tSize - 3; i++)
        {
            auto low = usedx.lower_bound(i), up = low;
            low--;
            // 前後に禁止マスがある場合盤面を分割しかねないので作らない
            // 3 じゃない気がする
            if (abs(i - (*low)) <= 3 || abs(i - (*up)) <= 3)
                continue;
            // 必須マスがある行には作らない
            if (isx.find(i) != isx.end())
                continue;
            // 突起を一つ生やすと頂点数は4増える?
            for (int j = 0; j < tSize - 4; j++)
            {
                ret[i][j] = '#';
                if (j % 3)
                    continue;
                if (ret[i - 1][j] != '.')
                {
                    ret[i - 1][j] = '#';
                    corner += 4;
                }
                if (ret[i + 1][j] != '.')
                {
                    ret[i + 1][j] = '#';
                    corner += 4;
                }
            }
            col = i + 5;
            break;
        }
        // 600 個ぐらいまでは一本生やせばいいが...
        /* 
        for(int i=col;i<tSize-3;i++)
        {
            auto low = usedx.lower_bound(i), up = low;
            low--;
            if(abs(i-(*low))<=3 || abs(i-(*up))<=3) continue;
            if(isx.find(i)!=isx.end()) continue;
            for(int j=0;j<tSize-4;j++)
            {
                ret[i][j] = '#';
                if(j%3) continue;
                if(ret[i-1][j]!='.')
                {
                    ret[i-1][j] = '#';
                    corner+=2;
                }
                if(ret[i+1][j]!='.')
                {
                    ret[i+1][j] = '#';
                    corner += 2;
                }
            }
            break;
        }*/
        cerr << corner << endl;
        cerr << "corner done" << endl;

        // othello deleted

        // 空いてるマスにアイテムを置く
        int index = 3; // index 番目のアイテムは num[index] 個置く
        vector<vector<P>> itemPos(9);
        assert(_nums[index] != 0);
        // 未定の場所は全て空きマスにする
        for (int i = 0; i < tSize; i++)
            for (int j = 0; j < tSize; j++)
                if (ret[i][j] == '*')
                    ret[i][j] = '.';
        // 空いてたら置く
        for (int i = 1; i < tSize - 1; i++)
        {
            for (int j = 1; j < tSize - 1; j++)
            {
                if (index >= 9)
                    break;
                if (ret[i][j] != '.')
                    continue;
                ret[i][j] = items[index];
                itemPos[index].emplace_back(i, j);
                _nums[index]--;
                if (_nums[index] == 0)
                    index++;
            }
        }
        cerr << "items done" << endl;
        for (auto p : iSqs)
        {
            assert(ret[p.x][p.y] != '#');
        }
        assert(index == 9);
        _ret = ret;
        // 経路作成
        auto vp = route(ret);
        cerr << "route done" << endl;
        // デバッグ出力
        // printret();
        vector<P> used = {vp[0]};
        int sz = vp.size();
        // 一直線に並ぶ点は端点だけ出力
        for (int i = 1; i < sz - 1; i++)
        {
            auto pre = used.back(), next = vp[i + 1];
            if (pre.first == vp[i].first && vp[i].first == next.first)
                continue;
            if (pre.second == vp[i].second && vp[i].second == next.second)
                continue;
            used.emplace_back(vp[i]);
        }
        {
            int i = sz - 1;
            auto pre = used.back(), next = vp[0];
            if (pre.first == vp[i].first && vp[i].first == next.first)
            {
            }
            else if (pre.second == vp[i].second && vp[i].second == next.second)
            {
            }
            else
            {
                used.emplace_back(vp[sz - 1]);
            }
        }
        // これは頂点数と一致する??
        cerr << used.size() << endl;
        // 経路, 開始位置, アイテム位置 を出力形式に変換
        string sol = "";
        for (auto p : used)
        {
            string point = "(" + to_string(p.first) + "," + to_string(p.second) + ")" + ",";
            sol += point;
        }
        cerr << "point done" << endl;
        sol.back() = '#';
        // start point
        {
            auto p = P(iSqs[iSqs.size() / 2].x, iSqs[iSqs.size() / 2].y);
            string point = "(" + to_string(p.first) + "," + to_string(p.second) + ")" + ",";
            sol += point;
        }
        sol.back() = '#';
        sol += "#";
        cerr << "start done" << endl;
        for (int i = 3; i < 9; i++)
        {
            for (auto p : itemPos[i])
            {
                string is = "";
                is += items[i];
                string point = is + "(" + to_string(p.first) + "," + to_string(p.second) + ")" + ";";
                sol += point;
            }
        }
        cerr << "sol done" << endl;
        return sol.substr(0, sol.size() - 1);
    }
};

int main()
{
    // g++ -std=c++17 grid_generatir.cpp
    // .a.out < puzzle.cond > task.desc
    string puzzle;
    cin >> puzzle;
    PuzzleParser parser(puzzle);
    Generator gen(parser.nums, parser.iSqs, parser.oSqs);
    auto ret = gen.generate();
    cout << ret << endl;
}
