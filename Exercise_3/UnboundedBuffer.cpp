#include <mutex>
#include <queue>
#include <string>
#include <stdexcept>

template<typename T>
class UnboundedBuffer {
private:
    std::mutex mtx;
    std::queue<std::string> queue;

public:
    UnboundedBuffer() {}

    void insert(const std::string item) {
        std::lock_guard lock(mtx);
        queue.push(item);
    }

    std::string remove() {
        std::lock_guard lock(mtx);
        if (queue.empty()) {
            throw std::runtime_error("Attempt to remove from an empty buffer");
        }
        std::string item = queue.front();
        queue.pop();
        return item;
    }
};