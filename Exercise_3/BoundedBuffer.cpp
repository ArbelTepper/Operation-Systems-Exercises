#include "BoundedBuffer.h"

// Template member function definitions

template<typename T>
BoundedBuffer<T>::BoundedBuffer(size_t size)
    : emptySlots(size), filledSlots(0), size(size) {}

template<typename T>
void BoundedBuffer<T>::insert(const T& item) {
    emptySlots.acquire();
    {
        std::lock_guard<std::mutex> lock(mtx);
        queue.push(item);
    }
    filledSlots.release();
}

template<typename T>
T BoundedBuffer<T>::remove() {
    filledSlots.acquire();
    std::lock_guard<std::mutex> lock(mtx);
    T item = queue.front();
    queue.pop();
    emptySlots.release();
    return item;
}

template<typename T>
bool BoundedBuffer<T>::isEmpty() {
    std::lock_guard<std::mutex> lock(mtx);
    return queue.empty();
}

// Explicitly instantiate the template for std::string
template class BoundedBuffer<std::string>;
