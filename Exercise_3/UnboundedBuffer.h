#ifndef UNBOUNDEDBUFFER_H
#define UNBOUNDEDBUFFER_H

#include <mutex>
#include <queue>
#include <string>

template<typename T>
class UnboundedBuffer {
private:
    std::mutex mtx;
    std::queue<std::string> queue;

public:
    void insert(const std::string item);
    std::string remove();
};

#endif // UNBOUNDEDBUFFER_H