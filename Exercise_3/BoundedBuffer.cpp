#include "BoundedBuffer.h"

template<typename T>
BoundedBuffer<T>::BoundedBuffer(size_t size)
    : size(size),
      emptySlots(size), // Initialize with the total number of slots, indicating all are empty initially
      filledSlots(0) // Initialize with zero, indicating no slots are filled initially
{}

template<typename T>
void BoundedBuffer<T>::insert(const T& item) {
    // Wait for an empty slot
    emptySlots.acquire();

    // Lock the mutex before accessing/modifying the queue
    {
        std::lock_guard<std::mutex> lock(mtx);
        queue.push(item);
    }

    // Signal that a slot has been filled
    filledSlots.release();
}

template<typename T>
T BoundedBuffer<T>::remove() {
    // Wait for a filled slot
    filledSlots.acquire();

    // Lock the mutex before accessing/modifying the queue
    T item;
    {
        std::lock_guard<std::mutex> lock(mtx);
        item = queue.front();
        queue.pop();
    }

    // Signal that a slot has been emptied
    emptySlots.release();

    return item;
}

template<typename T>
bool BoundedBuffer<T>::isEmpty() {
    std::lock_guard<std::mutex> lock(mtx);
    return queue.empty();
}
