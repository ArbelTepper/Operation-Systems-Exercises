#ifndef COEDITOR_H
#define COEDITOR_H

#include "UnboundedBuffer.h"
#include <string>

class CoEditor {
private:
    UnboundedBuffer<std::string>& topicBuffer; // Reference to an UnboundedBuffer for topics
    UnboundedBuffer<std::string>& screenManagerBuffer; // Reference to an UnboundedBuffer for the screen manager

public:
    // Constructor declaration
    CoEditor(UnboundedBuffer<std::string>& topicBuffer, UnboundedBuffer<std::string>& screenManagerBuffer);

    // Method to process and forward messages
    void processAndForward();
};

#endif // COEDITOR_H