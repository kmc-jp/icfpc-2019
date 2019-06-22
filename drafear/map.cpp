class Cell {
  const bool is_obstacle;
  const bool can_dig;
  const std::vector<BoosterCode> boosters;
  int max_teleid; // 訪れたときに持っていたテレポータidの最大値
};

class Map {
  std::vector<std::vector<Cell>> board;
public:
  Map(std::vector<std::vector<Cell>> board) : board(std::move(board)) {}
  std::vector<Cell>& operator[](size_t idx) {
    return board[idx];
  }
  static Map from_input() {
  }
};
