#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include "UnboundedBuffer.h"
#include <string>

class ScreenManager {
private:
    UnboundedBuffer<std::string>& buffer;
    int doneCount;

public:
    // Constructor
    ScreenManager(UnboundedBuffer<std::string>& buffer);

    // Function to print messages until 3 "Done" messages are seen
    void printMessages();
};

#endif // SCREENMANAGER_H