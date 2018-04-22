#include <fstream>
#include "det_maths.h"
#include "time_measurement.hpp"
#include "MPIEnv.h"

using namespace std;

#define FILE_LOGGING

SquareMatrix readMatrix(char* argv[]);

int main(int argc, char* argv[]) {
  if (argc < 2) {
    cout << "No file with matrix provided." << endl;
    return -1;
  } else {
    Stopwatch stopwatchTotal("log-total-times.txt");
    Stopwatch stopwatchOnlyDetCalc("log-calc-times.txt");
  
    stopwatchTotal.start();
    MPI_Init(nullptr, nullptr);
    MPIEnv mpi;
    if (mpi.isRootProcess()) {
      SquareMatrix matrix = readMatrix(argv);
  
      stopwatchOnlyDetCalc.start();
      const int64_t result = mpi.calculateDet(matrix);
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
    } else {
      cout << "Processor " << mpi.getProcessorName() << " works at rank " << mpi.getRank() << endl;
      mpi.doAllJobOfWorker();
    }
    MPI_Finalize();
  }
  return 0;
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