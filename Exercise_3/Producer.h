#ifndef PRODUCER_H
#define PRODUCER_H

#include <string>
#include <mutex>
#include "BoundedBuffer.h"

class Producer {
private:
    BoundedBuffer<std::string>& buffer; // Reference to a single shared buffer
    int producerID;
    int producedCount; // Tracks the number of items this producer has produced
    std::mutex countMutex;

public:
    Producer(int id, BoundedBuffer<std::string>& buffer, int numItems);

    void produce();
};

#endif // PRODUCER_H