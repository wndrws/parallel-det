#pragma once

#include <chrono>
#include <string>
#include <sstream>
#include <fstream>

class Stopwatch {
    std::chrono::time_point<std::chrono::high_resolution_clock> time_begin;
    std::chrono::time_point<std::chrono::high_resolution_clock> time_end;
    std::string logFileName;
public:
    explicit Stopwatch(const std::string& filename) : logFileName(filename) {}
    
    inline void start() {
      time_begin = std::chrono::high_resolution_clock::now();
    }
    
    inline void finish() {
      time_end = std::chrono::high_resolution_clock::now();
    }
    
    inline int64_t getMicros() {
      return std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_begin).count();
    }
    
    inline int64_t getMillis() {
      return std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_begin).count();
    }
    
    inline int64_t getSeconds() {
      return std::chrono::duration_cast<std::chrono::seconds>(time_end - time_begin).count();
    }
    
    std::string getSummary() {
      std::ostringstream oss;
      int64_t seconds = getSeconds();
      if (seconds == 0) {
        oss << getMillis() << '.' << getMicros() % 1000 << " ms";
      } else {
        oss << seconds << '.' << getMillis() % 1000 << " s";
      }
      return oss.str();
    }
    
    void writeLogEntry() {
      std::ofstream logfile;
      logfile.open(logFileName, ios::app);
      logfile << getMicros() << endl;
      logfile.close();
    }
};