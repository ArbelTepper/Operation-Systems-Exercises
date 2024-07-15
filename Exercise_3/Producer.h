#ifndef PRODUCER_H
#define PRODUCER_H

#include <string>
#include <mutex>
#include <memory>
#include "BoundedBuffer.h"

using namespace std;

class Producer {
private: 
    shared_ptr<BoundedBuffer<std::string>> buffer; // Reference to a single shared buffer
    int producerID;
    int producedCount; // Tracks the number of items this producer has produced
    int itemsToProduce; // Number of items to produce
    int sportCounter;
    int newsCounter;
    int weatherCounter;
    std::mutex countMutex;

public:
    Producer(int id, shared_ptr<BoundedBuffer<std::string>> buffer, int numItems);

    void produce();
};

#endif // PRODUCER_H
