#include "det_maths.h"
#include "time_measurement.hpp"
#include <iomanip>

int64_t det(const SquareMatrix& m) {
  int64_t d = 0;
  const size_t oRow = 0; // Всегда раскладываем по первой строке
  if (m.size() == 2) {
    return m.at(0, 0) * m.at(1, 1) - m.at(1, 0) * m.at(0, 1);
  } else {
    for (size_t oCol = 0; oCol < m.size(); ++oCol) {
      d += calcSummand(m, oRow, oCol);
    }
  }
  return d;
}

int64_t calcSummand(const SquareMatrix& m, size_t oRow, size_t oCol) {
  SquareMatrix minor = collectMinor(m, oRow, oCol);
  int oEntry = m.at(oRow, oCol);
  return power(-1, oRow + oCol) * oEntry * det(minor);
}

SquareMatrix collectMinor(const SquareMatrix& matrix, size_t oRow, size_t oCol) {
  SquareMatrix minor(matrix.size() - 1);
  size_t minorRow = 0;
  for (size_t row = 0; row < matrix.size(); ++row) {
    if (row == oRow) continue;
    size_t minorCol = 0;
    for (size_t col = 0; col < matrix.size(); ++col) {
      if (col == oCol) continue;
      minor(minorRow, minorCol) = matrix.at(row, col);
      ++minorCol;
    }
    ++minorRow;
  }
  return minor;
}

bool operator==(const SquareMatrix& lhs, const SquareMatrix& rhs) {
  if (&lhs == &rhs) return true;
  if (lhs.size() != rhs.size()) return false;
  for (size_t r = 0; r < lhs.size(); ++r) {
    for (size_t c = 0; c < lhs.size(); ++c) {
      if (lhs.at(r, c) != rhs.at(r, c)) return false;
    }
  }
  return true;
}

ostream& operator<<(ostream& out, const SquareMatrix& m) {
  for (size_t r = 0; r < m.size(); ++r) {
    for (size_t c = 0; c < m.size(); ++c) {
      out << setw(3) << m.at(r, c) << (c == m.size() - 1 ? "" : " ");
    }
    out << endl;
  }
  return out;
}

int64_t power(int x, int p) {
  int64_t result = 1;
  for (int i = 1; i <= p; i++) {
    result = result*x;
  }
  return result;
}