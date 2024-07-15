#ifndef UNBOUNDEDBUFFER_H
#define UNBOUNDEDBUFFER_H

#include <mutex>
#include <queue>
#include <string>
#include <stdexcept>

template<typename T>
class UnboundedBuffer {
private:
    std::mutex mtx;
    std::queue<T> queue;

public:
    UnboundedBuffer();
    void insert(const T& item);
    T remove();
};

#endif // UNBOUNDEDBUFFER_H
