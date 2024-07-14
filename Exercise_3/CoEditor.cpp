#include "UnboundedBuffer.h"
#include <iostream>
#include <thread> // Add the necessary include directive for the <thread> header

class CoEditor {
private:
    UnboundedBuffer<std::string>& topicBuffer; // Reference to an UnboundedBuffer
    UnboundedBuffer<std::string>& screenManagerBuffer; // Reference to an UnboundedBuffer

public:
    // Constructor that initializes the references
    CoEditor(UnboundedBuffer<std::string>& topicBuffer, UnboundedBuffer<std::string>& screenManagerBuffer)
        : topicBuffer(topicBuffer), screenManagerBuffer(screenManagerBuffer) {}

    void processAndForward() {
    std::string message;
    while (true) {
            message = topicBuffer.remove(); // Assuming UnboundedBuffer has a remove method that returns a string
            if (message == "DONE") {
                screenManagerBuffer.insert(message); // Assuming UnboundedBuffer has an insert method
                break; // Exit the loop if the message is "DONE"
            } else {
                screenManagerBuffer.insert(message); // Forward the message to the screen manager buffer
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Pause for 0.1 seconds
            }
        }
    }   
};