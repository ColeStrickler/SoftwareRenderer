#ifndef UTIL_HPP
#define UTIL_HPP



#include <chrono>
#include <iostream>
#include <string>

class ScopeTimer {
public:
    explicit ScopeTimer(const char* func)
        : func_(func),
          start_(std::chrono::high_resolution_clock::now()) {}

    ~ScopeTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start_).count();
        std::cout << func_ << " elapsed: " << dur << "us\n";
    }

private:
    const char* func_;
    std::chrono::high_resolution_clock::time_point start_;
};

#define SCOPE_TIMER() ScopeTimer __timer(__func__)

#endif