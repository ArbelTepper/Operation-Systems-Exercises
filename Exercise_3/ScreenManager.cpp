#include "UnboundedBuffer.h"
#include <iostream>
#include <string>

class ScreenManager {
private:
    UnboundedBuffer<std::string>& buffer;
    int doneCount = 0;

public:
    // Constructor
    ScreenManager(UnboundedBuffer<std::string>& buffer) : buffer(buffer) {}

    // Function to print messages until 3 "Done" messages are seen
    void printMessages() {
        while (doneCount < 3) {
            std::string message = buffer.remove(); // Assuming UnboundedBuffer has a remove method that returns a string
            std::cout << message << std::endl;
            if (message == "DONE") {
                doneCount++;
            }
        }
    }
};