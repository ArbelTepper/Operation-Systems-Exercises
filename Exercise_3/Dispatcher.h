#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <string>
#include <vector>
#include <memory>
#include "BoundedBuffer.h"
#include "UnboundedBuffer.h"

using namespace std;

class Dispatcher {
private:
    vector<shared_ptr<BoundedBuffer<string>>> buffers; // Array of BoundedBuffer objects
    int doneCounter; // Counter for tracking "DONE" messages
    int bufferLength;
    UnboundedBuffer<std::string>& sportBuffer;
    UnboundedBuffer<std::string>& newsBuffer;
    UnboundedBuffer<std::string>& weatherBuffer;

public:
    Dispatcher(vector<shared_ptr<BoundedBuffer<string>>>& bufferArray,
               UnboundedBuffer<std::string>& sportBuf,
               UnboundedBuffer<std::string>& newsBuf,
               UnboundedBuffer<std::string>& weatherBuf); // Constructor declaration

    void processBuffers(); // Method to start the dispatch process
    // Other methods as necessary
};

#endif // DISPATCHER_H