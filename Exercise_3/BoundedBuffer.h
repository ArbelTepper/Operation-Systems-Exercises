#ifndef BOUNDEDBUFFER_H
#define BOUNDEDBUFFER_H

#include <mutex>
#include <queue>
#include <string>
#include <semaphore>

template<typename T>
class BoundedBuffer {
private:
    std::counting_semaphore<size_t> emptySlots; // Semaphore to track empty slots
    std::counting_semaphore<size_t> filledSlots; // Semaphore to track filled slots
    std::mutex mtx; // Mutex for mutual exclusion
    std::queue<T> queue; // Queue to store the elements
    size_t size; // Size of the bounded buffer

public:
    BoundedBuffer(size_t size);

    void insert(const T& item);

    T remove();

    bool isEmpty();
};

// Include the template function definitions
#include "BoundedBuffer.cpp"

#endif // BOUNDEDBUFFER_H
