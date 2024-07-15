#include "Producer.h"
#include <iostream>
#include <string>
#include <random>

Producer::Producer(int id, shared_ptr<BoundedBuffer<std::string>> buffer, int numItems)
    : producerID(id), buffer(buffer), producedCount(0), itemsToProduce(numItems), sportCounter(0), newsCounter(0), weatherCounter(0) {}

void Producer::produce() {
    std::random_device rd; // Obtain a random number from hardware
    std::mt19937 gen(rd()); // Seed the generator
    std::uniform_int_distribution<> distr(0, 2); // Define the range

    for (int i = 0; i < itemsToProduce; ++i) {
        int topicNumber = distr(gen); // Generates a random number (0, 1, or 2)
        std::string topic;
        std::string message;

        switch (topicNumber) {
            case 0:
                topic = "SPORT";
                message = "producer " + std::to_string(producerID) + " " + topic + " item " + std::to_string(++sportCounter);
                break;
            case 1:
                topic = "NEWS";
                message = "producer " + std::to_string(producerID) + " " + topic + " item " + std::to_string(++newsCounter);
                break;
            case 2:
                topic = "WEATHER";
                message = "producer " + std::to_string(producerID) + " " + topic + " item " + std::to_string(++weatherCounter);
                break;
        }

        buffer->insert(message);
    }
    // Insert "DONE" after producing the specified number of items
    buffer->insert("DONE");
}
