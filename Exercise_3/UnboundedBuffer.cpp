#include <mutex>
#include <queue>
#include <string>
#include <stdexcept>
#include "UnboundedBuffer.h"
using namespace std;

template <typename T>
UnboundedBuffer<T>::UnboundedBuffer()
{
}

template <typename T>
void UnboundedBuffer<T>::insert(const T &item)
{
	std::lock_guard<std::mutex> lock(mtx);
    queue.push(item);
}

template<typename T>
T UnboundedBuffer<T>::remove() {
    std::lock_guard<std::mutex> lock(mtx);
    if (queue.empty()) {
        throw std::runtime_error("Attempt to remove from an empty buffer");
    }
    T item = queue.front();
    queue.pop();
    return item;
}