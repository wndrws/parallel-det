#include <fstream>
#include "det_maths.h"
#include "time_measurement.hpp"

using namespace std;

#define FILE_LOGGING

#if(TESTS_ENABLED == 1)
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#endif

#if(PARALLELISM_ENABLED == 1)
#include "det_parallel.hpp"
#define calculateDet(argMatrix) detParallel(argMatrix)
#else
#define calculateDet(argMatrix) det(argMatrix)
#endif

SquareMatrix readMatrix(char* argv[]);

int main(int argc, char* argv[]) {
  if (argc < 2) {
    cout << "No file with matrix provided." << endl;
#if(TESTS_ENABLED == 1)
    cout << "Performing tests..." << endl;
    return Catch::Session().run(argc, argv);
#else
    return -1;
#endif
  } else {
    Stopwatch stopwatchTotal("log-total-times.txt");
    Stopwatch stopwatchOnlyDetCalc("log-calc-times.txt");
    
    stopwatchTotal.start();
    SquareMatrix matrix = readMatrix(argv);
    
    stopwatchOnlyDetCalc.start();
    const int64_t result = calculateDet(matrix);
    stopwatchOnlyDetCalc.finish();
    
    cout << "The matrix is: " << endl << matrix << endl;
    cout << "Its determinant is: " << result << endl;
    stopwatchTotal.finish();
    
    cout << "Total elapsed time: " + stopwatchTotal.getSummary() << endl;
    cout << "Det calculation time: " + stopwatchOnlyDetCalc.getSummary() << endl;
#ifdef FILE_LOGGING
    stopwatchTotal.writeLogEntry();
    stopwatchOnlyDetCalc.writeLogEntry();
#endif
    return 0;
  }
}

SquareMatrix readMatrix(char* argv[]) {
  ifstream file;
  file.open(argv[1], ios_base::in);
  size_t size;
  file >> size;
  
  int val;
  SquareMatrix matrix(size);
  for (size_t r = 0; r < matrix.size(); ++r) {
    for (size_t c = 0; c < matrix.size(); ++c) {
      file >> val;
      matrix(r, c) = val;
    }
  }
  file.close();
  return matrix;
}

#if(TESTS_ENABLED == 1)

SCENARIO("Матричные операции") {
  GIVEN("Матрица размером 3x3") {
    SquareMatrix matrix(3);
    long n = 0;
    for (size_t r = 0; r < matrix.size(); ++r) {
      for (size_t c = 0; c < matrix.size(); ++c) {
        matrix(r, c) = ++n;
      }
    }
    
    WHEN("сравниваем матрицу с самой собой") {
      bool truth = matrix == matrix;
      
      THEN("получаем истину") {
        REQUIRE(truth);
      }
    }
  
    WHEN("сравниваем матрицу с другой такой же") {
      SquareMatrix sameMtrix(3);
      for (size_t r = 0; r < sameMtrix.size(); ++r) {
        for (size_t c = 0; c < sameMtrix.size(); ++c) {
          sameMtrix(r, c) = matrix.at(r, c);
        }
      }
      bool truth = matrix == sameMtrix;
    
      THEN("получаем истину") {
        REQUIRE(truth);
      }
    }
  }
}

SCENARIO("Выбор миноров") {
  GIVEN("Матрица размером 3x3") {
    SquareMatrix matrix(3);
    int n = 0;
    for (size_t r = 0; r < matrix.size(); ++r) {
      for (size_t c = 0; c < matrix.size(); ++c) {
        matrix(r, c) = ++n;
      }
    }

    WHEN("собираем минор относительно элемента (0,0)") {
      const SquareMatrix minor = collectMinor(matrix, 0, 0);
      SquareMatrix expectedMinor(2);
      expectedMinor(0,0) = matrix(1,1);
      expectedMinor(0,1) = matrix(1,2);
      expectedMinor(1,0) = matrix(2,1);
      expectedMinor(1,1) = matrix(2,2);

      THEN("получаем ту же матрицу без первой строки и первого столбца") {
        REQUIRE(minor.size() == 2);
        REQUIRE(minor == expectedMinor);
      }
    }
  
    WHEN("собираем минор относительно элемента (0,1)") {
      const SquareMatrix minor = collectMinor(matrix, 0, 1);
      SquareMatrix expectedMinor(2);
      expectedMinor(0,0) = matrix(1,0);
      expectedMinor(0,1) = matrix(1,2);
      expectedMinor(1,0) = matrix(2,0);
      expectedMinor(1,1) = matrix(2,2);
      
      THEN("получаем ту же матрицу без первой строки и второго столбца") {
        REQUIRE(minor.size() == 2);
        REQUIRE(minor == expectedMinor);
      }
    }
  
    WHEN("собираем минор относительно элемента (0,2)") {
      const SquareMatrix minor = collectMinor(matrix, 0, 2);
      SquareMatrix expectedMinor(2);
      expectedMinor(0,0) = matrix(1,0);
      expectedMinor(0,1) = matrix(1,1);
      expectedMinor(1,0) = matrix(2,0);
      expectedMinor(1,1) = matrix(2,1);
    
      THEN("получаем ту же матрицу без первой строки и третьего столбца") {
        REQUIRE(minor.size() == 2);
        REQUIRE(minor == expectedMinor);
      }
    }
  }
}

SCENARIO("Вычисление определителя") {
  GIVEN("Матрица размером 3x3") {
    SquareMatrix m(3);
    m(0,0) =  1; m(0,1) = -2; m(0, 2) =  3;
    m(1,0) =  4; m(1,1) =  0; m(1, 2) =  6;
    m(2,0) = -7; m(2,1) =  8; m(2, 2) =  9;
    const int64_t expectedDeterminant = 204;
    
    WHEN("находим определитель") {
      const int64_t determinant = calculateDet(m);
      
      THEN("определитель вычислен верно") {
        REQUIRE(determinant == expectedDeterminant);
      }
    }
  }
}

#endif