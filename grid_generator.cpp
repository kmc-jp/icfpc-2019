#include <vector>
#include <string>
#include <cassert>
#include <tuple>
#include <iostream>
#include <algorithm>

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
    // 作る盤面の座標最大値(min:= tSize - floor(0.1*tSize), area_min:= floor(0.2*tSize-2))
    int tSize;
    // 盤面の最小/最大頂点数
    int vMin, vMax;
    // ブラシ, 倍速, ドリル, ワープ, クローン, Xマス
    int mNum, fNum, dNum, rNum, cNum, xNum;
    // 必須マス, 禁止マス
    std::vector<Point> iSqs, oSqs;
    int dx[4] = {1, 0, -1, 0}, dy[4] = {0, 1, 0, -1};
    const string items = "MFDRCX";
    std::vector<int> _nums;

public:
    Generator(const vector<int> &nums, std::vector<Point> &is, std::vector<Point> &os)
    {
        iSqs = is;
        oSqs = os;
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

    vector<vector<char>> generate()
    {
        // 各マスは未定
        auto ret = vector<vector<char>>(tSize, vector<char>(tSize, '*'));
        // 必須マスを . に
        for (auto p : iSqs)
        {
            // cerr << p.x << " " << p.y << endl;
            ret[p.x - 1][p.y - 1] = '.';
        }
        int corner = 4, cmax = (vMin + vMax) / 2;
        // 禁止マスを # に
        for (auto p : oSqs)
        {
            ret[p.x - 1][p.y - 1] = '#';
            // 右か左の端に繋げる
            bool l = true, r = true;
            for (int j = 0; j < tSize; j++)
            {
                if (ret[p.x - 1][j] != '.')
                    continue;
                if (j < p.y - 1)
                    l = false;
                else
                    r = false;
            }
            assert(l || r);
            if (l)
            {
                for (int j = p.y - 1; j >= 0; j--)
                {
                    ret[p.x - 1][j] = '#';
                    if (p.x - 1 > 1 && ret[p.x - 2][j] == '#')
                        break;
                }
            }
            else
            {
                for (int j = tSize - 1; j >= p.y - 1; j--)
                {
                    ret[p.x - 1][j] = '#';
                    if (p.x - 1 > 1 && ret[p.x - 2][j] == '#')
                        break;
                }
            }
            corner += 2; // 怪しい
        }
        // 角の個数を調整
        for (int i = 0; i < tSize; i++)
        {
            for (int j = 0; j < tSize; j++)
            {
                if (0 < i && i < tSize - 1 && 0 < j && j < tSize - 1)
                    continue;
                if (corner > cmax)
                    break;
                if ((i + j) % 2)
                    continue;
                if (ret[i][j] != '*')
                    continue;
                ret[i][j] = '#';
                corner += 2;
            }
        }
        // 空いてるマスにアイテムを置く
        int index = 3;
        assert(_nums[index] != 0);
        for (int i = 0; i < tSize; i++)
            for (int j = 0; j < tSize; j++)
                if (ret[i][j] == '*')
                    ret[i][j] = '.';
        for (int i = 0; i < tSize; i++)
        {
            for (int j = 0; j < tSize; j++)
            {
                if (index >= 9)
                    break;
                if (ret[i][j] != '.')
                    continue;
                ret[i][j] = items[index];
                _nums[index]--;
                if (_nums[index] == 0)
                    index++;
            }
        }
        assert(index == 9);
        return ret;
    }
};

int main()
{
    string puzzle;
    cin >> puzzle;
    PuzzleParser parser(puzzle);
    Generator gen(parser.nums, parser.iSqs, parser.oSqs);
    auto ret = gen.generate();
    for (auto v : ret)
    {
        for (auto c : v)
        {
            cout << c;
        }
        cout << endl;
    }
}