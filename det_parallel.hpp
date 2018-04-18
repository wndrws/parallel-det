#pragma once

#include "det_maths.h"
#include <windows.h>
#include <vector>
#include <map>

namespace DetParallel {

  const unsigned int maxThreadCount = 4;
  CRITICAL_SECTION accumCriticalSection;
  int64_t accum = 0;
  
  struct WorkerData {
      const SquareMatrix& matrix;
      size_t oRow;
      size_t oCol;
  };
  std::map<size_t, std::vector<WorkerData>> workersData;
  
  DWORD doAllJobOfWorker(LPVOID workerNum) {
    for (const auto& data : workersData.at((size_t) workerNum)) {
      const int64_t d = calcSummand(data.matrix, data.oRow, data.oCol);
      EnterCriticalSection(&accumCriticalSection);
      accum += d;
      LeaveCriticalSection(&accumCriticalSection);
    }
    return 0;
  }
  
  class DetWorkersPool {
      size_t jobsCount = 0;
      HANDLE hThreadArray[maxThreadCount];
  public:
      DetWorkersPool() {
        InitializeCriticalSection(&accumCriticalSection);
      }
      
      ~DetWorkersPool() {
        DeleteCriticalSection(&accumCriticalSection);
        for (int i = 0; i < workersData.size(); ++i) {
          CloseHandle(hThreadArray[i]);
        }
      }
      
      void submitMinorCalculation(const SquareMatrix& matrix, size_t oRow, size_t oCol) {
        const size_t workerNum = jobsCount++ % maxThreadCount;
        workersData[workerNum].push_back({matrix, oRow, oCol});
      }
      
      int64_t getDeterminant() {
        startCalculationsInThreads();
        switch (WaitForMultipleObjects(workersData.size(), hThreadArray, TRUE, INFINITE)) {
          case WAIT_OBJECT_0: return accum;
          default:
            printf("WaitForMultipleObjects failed (%d)\n", GetLastError());
            exit(1);
        }
      }
      
  private:
      void startCalculationsInThreads() {
        for (size_t i = 0; i < workersData.size(); ++i) {
          hThreadArray[i] = CreateThread(NULL, 0, doAllJobOfWorker, (LPVOID) i, 0, NULL);
          if (hThreadArray[i] == NULL) {
            printf("CreateThread failed (%d)\n", GetLastError());
            exit(1);
          }
        }
      }
  };
}

int64_t detParallel(const SquareMatrix& m) {
  int64_t d = 0;
  const size_t oRow = 0; // Всегда раскладываем по первой строке
  if (m.size() == 2) {
    return m.at(0, 0) * m.at(1, 1) - m.at(1, 0) * m.at(0, 1);
  } else {
    DetParallel::DetWorkersPool pool;
    for (size_t oCol = 0; oCol < m.size(); ++oCol) {
      pool.submitMinorCalculation(m, oRow, oCol);
    }
    d = pool.getDeterminant();
  }
  return d;
}