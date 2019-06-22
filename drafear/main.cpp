#include <cctype>
#include <iostream>
#include <set>
#include <tuple>
#include <vector>
#include <complex>

#include "input.h"

// std::vector<std::string> to_table(const Input& input) {
//   auto w = 0;
//   auto h = 0;
//   for (auto&& point : input.map) {
//     w = std::max(w, point.x);
//     h = std::max(h, point.y);
//   }
//   w += 2;
//   h += 2;
//   std::vector<std::string> table(h, std::string(w, '#'));
//   std::set<std::tuple<int, int, int>> s;
//   for (std::size_t i = 0; i < input.map.size(); ++i) {
//     const auto p = input.map[i];
//     const auto q = input.map[(i+1)%input.map.size()];
//     if (p.y != q.y) continue; // continue if vertical line
//     s.emplace(p.y+1, p.x+1, q.x+1);
//   }
//   for (auto [y, sx, tx] : s) {
//     if (sx < tx) {
//       for (int i = y; i < h-1; ++i) {
//         for (int j = sx; j < tx; ++j) {
//           table[i][j] = '.';
//         }
//       }
//     } else {
//       for (int i = y; i < h-1; ++i) {
//         for (int j = tx; j < sx; ++j) {
//           table[i][j] = '#';
//         }
//       }
//     }
//   }
//   for (auto&& obstacle : input.obstacles) {
//     s.clear();
//     for (std::size_t i = 0; i < obstacle.size(); ++i) {
//       const auto p = obstacle[i];
//       const auto q = obstacle[(i+1)%obstacle.size()];
//       if (p.y != q.y) continue; // continue if vertical line
//       s.emplace(p.y+1, p.x+1, q.x+1);
//     }
//     for (auto [y, sx, tx] : s) {
//       if (sx < tx) {
//         for (int i = y; i < h-1; ++i) {
//           for (int j = sx; j < tx; ++j) {
//             table[i][j]+='#'-'.';
//           }
//         }
//       } else {
//         for (int i = y; i < h-1; ++i) {
//           for (int j = tx; j < sx; ++j) {
//             table[i][j]-='#'-'.';
//           }
//         }
//       }
//     }
//   }
//   for (auto&& booster : input.boosters) {
//     char code = 'X';
//     switch (booster.code) {
//       case BoosterCode::B: code = 'B'; break;
//       case BoosterCode::F: code = 'F'; break;
//       case BoosterCode::L: code = 'L'; break;
//       case BoosterCode::R: code = 'R'; break;
//       case BoosterCode::X: code = 'X'; break;
//       default:;
//     }
//     table[booster.point.y+1][booster.point.x+1] = code;
//   }
//   return table;
// }

// void output_table(const std::vector<std::string>& table) {
//   for (int i = table.size() - 1; i >= 0; --i) {
//     std::cout << table[i] << std::endl;
//   }
// }

int main() {
  std::string line;
  std::getline(std::cin, line);
  const auto input = parse(line);
  // auto table = to_table(input);
  // output_table(table);
  return 0;
}
