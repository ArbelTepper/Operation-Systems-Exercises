#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <string>
#include <vector>
#include "BoundedBuffer.h"
#include "UnboundedBuffer.h"

class Dispatcher {
private:
    std::vector<BoundedBuffer<std::string>> buffers; // Array of BoundedBuffer objects
    int doneCounter = 0; // Counter for tracking "DONE" messages
    UnboundedBuffer<std::string>& sportBuffer;
    UnboundedBuffer<std::string>& newsBuffer;
    UnboundedBuffer<std::string>& weatherBuffer;

public:
    Dispatcher(const std::vector<BoundedBuffer<std::string>>& bufferArray,
               UnboundedBuffer<std::string>& sportBuf,
               UnboundedBuffer<std::string>& newsBuf,
               UnboundedBuffer<std::string>& weatherBuf); // Constructor declaration
    void processBuffers(); // Method to start the dispatch process
    // Other methods as necessary
};

#endif // DISPATCHER_H