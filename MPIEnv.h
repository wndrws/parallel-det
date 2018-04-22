#pragma once

#include <string>
#include "mpi.h"
#include "det_maths.h"

class MPIEnv {
    int rank;
    int worldSize;
    char processorName[MPI_MAX_PROCESSOR_NAME];
    MPI_Comm comm = MPI_COMM_WORLD;
    
    SquareMatrix *matrix;
    size_t* oRows;
    size_t* oCols;
    
public:
    MPIEnv();
    ~MPIEnv();
    
    std::string getProcessorName();
    int getRank();
    int getNumberOfProcesses();
    bool isRootProcess();
    int64_t calculateDet(SquareMatrix& m);
    void doAllJobOfWorker();
private:
    void initializeMPI();
    void cleanup();
    void broadcastMatrix(SquareMatrix& m);
    void sendEntryCoords(int workerRank);
    void receiveMatrix();
    int receiveEntryCoords();
    int64_t getDeterminant();
};


