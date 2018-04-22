#include "MPIEnv.h"

#include <vector>
#include <map>
#include <sstream>

using namespace std;

#define ROOT_RANK 0
#define TAG_MATRIX_SIZE 1
#define TAG_MATRIX 2
#define TAG_ROW 3
#define TAG_COLUMN 4
#define TAG_RESULT 5

map<int, vector<pair<size_t, size_t>>> workerEntryCoords;

MPIEnv::MPIEnv() {
  MPI_Comm_size(comm, &worldSize);
  MPI_Comm_rank(comm, &rank);
  int len;
  MPI_Get_processor_name(processorName, &len);
}

MPIEnv::~MPIEnv() {
  cleanup();
}

void MPIEnv::cleanup() {
  delete matrix;
  delete oRows;
  delete oCols;
}
string MPIEnv::getProcessorName() {
  return string(processorName);
}

int MPIEnv::getRank() {
  return rank;
}

int MPIEnv::getNumberOfProcesses() {
  return worldSize;
}

bool MPIEnv::isRootProcess() {
  return rank == ROOT_RANK;
}


int64_t MPIEnv::calculateDet(SquareMatrix& m) {
  const size_t oRow = 0; // Всегда раскладываем по первой строке
  if (m.size() == 2) {
    return m.at(0, 0) * m.at(1, 1) - m.at(1, 0) * m.at(0, 1);
  } else {
    broadcastMatrix(m);
    for (size_t oCol = 0; oCol < m.size(); ++oCol) {
      workerEntryCoords[oCol % (getNumberOfProcesses() - 1)].push_back({oRow, oCol});
    }
    int realWorkerCount = min((int) m.size() + 1, getNumberOfProcesses());
    for (int i = 1; i < realWorkerCount; ++i) {
      sendEntryCoords(i);
    }
  }
  return getDeterminant();
}

void MPIEnv::broadcastMatrix(SquareMatrix& m) {
  ostringstream ss;
  ss << m;
  string serializedMatrix = ss.str();
  size_t length = serializedMatrix.size();
  char* buf = new char[length];
  memcpy(buf, serializedMatrix.c_str(), length);
  size_t size = m.size();
  for (int i = 1; i < worldSize; ++i) {
    MPI_Send(&size, sizeof(size_t), MPI_BYTE, i, TAG_MATRIX_SIZE, comm);
    MPI_Send(buf, length, MPI_BYTE, i, TAG_MATRIX, comm);
  }
  delete buf;
}

void MPIEnv::receiveMatrix() {
  MPI_Status status;
  size_t size;
  MPI_Recv(&size, sizeof(size_t), MPI_BYTE, ROOT_RANK, TAG_MATRIX_SIZE, comm, &status);
  MPI_Probe(ROOT_RANK, TAG_MATRIX, comm, &status);
  int len;
  MPI_Get_count(&status, MPI_BYTE, &len);
  char* buf = new char[len];
  MPI_Recv(buf, len, MPI_BYTE, ROOT_RANK, TAG_MATRIX, comm, &status);
  istringstream iss(buf);
  matrix = new SquareMatrix(size);
  int val;
  for (size_t r = 0; r < matrix->size(); ++r) {
    for (size_t c = 0; c < matrix->size(); ++c) {
      iss >> val;
      matrix->operator()(r, c) = val;
    }
  }
  delete buf;
}

void MPIEnv::sendEntryCoords(int workerRank) {
  int N = workerEntryCoords.at(workerRank - 1).size();
  size_t* rows = new size_t[N];
  size_t* cols = new size_t[N];
  for (int i = 0; i < N; ++i) {
    rows[i] = workerEntryCoords.at(workerRank - 1)[i].first;
    cols[i] = workerEntryCoords.at(workerRank - 1)[i].second;
  }
  MPI_Send(rows, N*sizeof(size_t), MPI_BYTE, workerRank, TAG_ROW, comm);
  MPI_Send(cols, N*sizeof(size_t), MPI_BYTE, workerRank, TAG_COLUMN, comm);
  delete rows;
  delete cols;
}

int MPIEnv::receiveEntryCoords() {
  MPI_Status status;
  MPI_Probe(ROOT_RANK, TAG_ROW, comm, &status);
  int len;
  MPI_Get_count(&status, MPI_BYTE, &len);
  int N = len / sizeof(size_t);
  oRows = new size_t[N];
  MPI_Recv(oRows, len, MPI_BYTE, ROOT_RANK, TAG_ROW, comm, &status);
  oCols = new size_t[N];
  MPI_Recv(oCols, len, MPI_BYTE, ROOT_RANK, TAG_COLUMN, comm, &status);
  return N;
}

int64_t MPIEnv::getDeterminant() {
  MPI_Barrier(comm);
  int64_t d = 0;
  for (int i = 1; i < worldSize; ++i) {
    int64_t summand;
    MPI_Status status;
    MPI_Recv(&summand, sizeof(int64_t), MPI_BYTE, i, TAG_RESULT, comm, &status);
    d += summand;
  }
  return d;
}

void MPIEnv::doAllJobOfWorker() {
  receiveMatrix();
  int64_t res = 0;
  if (matrix->size() >= getRank()) {
    int jobsCount = receiveEntryCoords();
    for (int i = 0; i < jobsCount; ++i) {
      res += calcSummand(*matrix, oRows[i], oCols[i]);
    }
  }
  MPI_Send(&res, sizeof(int64_t), MPI_BYTE, ROOT_RANK, TAG_RESULT, comm);
  MPI_Barrier(comm);
}