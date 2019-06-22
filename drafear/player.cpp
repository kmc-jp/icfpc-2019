class Player {
  Point location;
  std::vector<Point> manipulators;
public:
  Player(Point initial_location)
    : location(std::move(initial_location)),
      manipulators({{1, 1}, {1, 0}, {1, -1}}) {}
  int& x() { return location.x; }
  int& y() { return location.y; }
  void rotate_right() {
    // (x, y) -> (y, -x)
    for (auto&& p : manipulators) {
      std::swap(p.x, p.y);
      p.y = -p.y;
    }
  }
  void rotate_left() {
    rotate_right();
    rotate_right();
    rotate_right();
  }
};
