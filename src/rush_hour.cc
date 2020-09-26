#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <cassert>

using namespace std;

const char kEmptySpot = '.';
const char kOutsideBoard = '\0';

bool IsValidCarPart(char c) {
  return c != kEmptySpot || c != kOutsideBoard;
}

enum class Orientation {
  kUnknown,
  kHorizontal,
  kVertical
};

struct CarDimensions {
  int i1, j1, i2, j2;

  bool IsValid() { return i1 >= 0 && j1 >= 0 && i2 >= 0 && j2 >= 0 && i1 <= i2 && j1 <= j2; }
  static CarDimensions Invalid() { return { -1, -1, -1, -1 }; }
  int Length() { return max(i2 - i1 + 1, j2 - j1 + 1); }
  Orientation Orientation() { return i1 == i2 ? Orientation::kHorizontal : Orientation::kVertical; }

  string ToString() {
    stringstream ss;
    ss << "{" << i1 << ", " << j1 << ", " << i2 << ", " << j2 << "}";
    return ss.str();
  }

  void FillVisited(vector<vector<bool>>* v) {
    if (IsValid()) {
      for (int i = i1; i <= i2; ++i) {
        for (int j = j1; j <= j2; ++j) {
          assert(!(*v)[i][j]);
          (*v)[i][j] = true;
        }
      }
    }
  }

};

class BoardState {
 public:
  BoardState(size_t m, size_t n) {
    rows_.resize(m);
    for (size_t i = 0; i < m; ++i) {
      rows_[i] = string(n, kEmptySpot);
    }
  }
  BoardState(const BoardState& other) = default;

  explicit BoardState(vector<string> v) : rows_(v) {
  }

  size_t width() { return rows_[0].size(); }
  size_t height() { return rows_.size(); }

  char ElementAt(int i, int j) {
    if (0 <= i && i < width() && 0 <= j && j < height()) {
      return rows_[i][j];
    }
    return kOutsideBoard;
  }

  Orientation CarOrientation(int i, int j) {
    char c = ElementAt(i, j);
    if (c == kEmptySpot || c == kOutsideBoard) {
      return Orientation::kUnknown;
    }
    if (c == ElementAt(i - 1, j) || c == ElementAt(i + 1, j)) {
      return Orientation::kVertical;
    }
    return Orientation::kHorizontal;
  }

  CarDimensions LocateCar(const int i, const int j) {
    auto orientation = CarOrientation(i, j);
    if (orientation == Orientation::kUnknown) {
      return CarDimensions::Invalid();
    }
    int lowest_i = i;
    int lowest_j = j;
    int highest_i = i;
    int highest_j = j;
    const char c = ElementAt(i, j);
    if (orientation == Orientation::kVertical) {
      while (ElementAt(lowest_i - 1, j) == c) lowest_i--;
      while (ElementAt(highest_i + 1, j) == c) highest_i++;
    } else {
      while (ElementAt(i, lowest_j - 1) == c) lowest_j--;
      while (ElementAt(i, highest_j + 1) == c) highest_j++;
    }
    return {lowest_i, lowest_j, highest_i, highest_j};
  }

  void Erase(CarDimensions d) {
    for (int i = d.i1; i <= d.i2; ++i) {
      for (int j = d.j1; j <= d.j2; ++j) {
        rows_[i][j] = kEmptySpot;
      }
    }
  }

  vector<BoardState> Neighbors() {
    const size_t m = width();
    const size_t n = height();

    vector<vector<bool>> visited(m);
    for (size_t i = 0; i < m; ++i) {
      visited[i] = vector<bool>(n, false);
    }

    for (size_t i = 0; i < m; ++i) {
      for (size_t j = 0; j < n; ++j) {
        if (visited[i][j]) {
          continue;
        }
        const char c = rows_[i][j];
        auto dimensions = LocateCar(i, j);
        if (dimensions.IsValid()) {
          dimensions.FillVisited(&visited);

          cout << "Found car: " << dimensions.ToString() << endl;
        }
      }
    }
    return {};
  }

  string ToString() {
    ostringstream ss;
    for (const auto& row : rows_) {
      for (char c : row) {
        ss << c << " ";
      }
      ss << endl;
    }
    return ss.str();
  }

 private:
  vector<string> rows_;
};


int main() {
  BoardState initial_state{{
    ".11222",
    "333456",
    "RR7456",
    "8.7DDC",
    "8AA..C",
    "99BBBC"
  }};
  cout << initial_state.ToString() << endl;
  initial_state.Neighbors();
  return 0;
}