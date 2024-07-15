#include "UnboundedBuffer.h"
#include <iostream>
#include <string>
#include "ScreenManager.h"

    ScreenManager::ScreenManager(UnboundedBuffer<std::string>& buffer) : buffer(buffer) {}

    // Function to print messages until 3 "Done" messages are seen
    void ScreenManager::printMessages() {
        while (doneCount < 3) {
            std::string message = buffer.remove(); // Assuming UnboundedBuffer has a remove method that returns a string
            std::cout << message << std::endl;
            if (message == "DONE") {
                doneCount++;
            }
        }
    };