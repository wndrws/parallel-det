#pragma once

#include <iostream>
#include <valarray>

using namespace std;

class SquareMatrix {
    valarray<int64_t> data;
    size_t dim;
public:
    explicit SquareMatrix(size_t d) : data(d * d), dim(d) {}
    
    int64_t& operator()(size_t r, size_t c) { return data[r * dim + c]; }
    
    int64_t at(size_t r, size_t c) const { return data[r * dim + c]; }
    
    size_t size() const { return dim; }
};

int64_t det(const SquareMatrix& m);

SquareMatrix collectMinor(const SquareMatrix& matrix, size_t row, size_t col);

int64_t calcSummand(const SquareMatrix& m, size_t oRow, size_t oCol);

ostream& operator<<(ostream& out, SquareMatrix const& m);

bool operator==(const SquareMatrix& lhs, const SquareMatrix& rhs);

int64_t power(int x, int p);
