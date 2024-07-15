#ifndef BOUNDEDBUFFER_H
#define BOUNDEDBUFFER_H

#include <mutex>
#include <queue>
#include <semaphore>
#include <string>

template<typename T>
class BoundedBuffer {
private:
    std::counting_semaphore<> emptySlots; // Semaphore to track empty slots
    std::counting_semaphore<> filledSlots; // Semaphore to track filled slots
    std::mutex mtx; // Mutex for mutual exclusion
    std::queue<T> queue; // Queue to store the elements
    size_t size; // Size of the bounded buffer

public:
    BoundedBuffer(size_t size);

    void insert(const T& item);

    T remove();

    bool isEmpty();
};

#endif // BOUNDEDBUFFER_H
