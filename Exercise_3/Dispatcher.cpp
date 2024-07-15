#include <vector>
#include "BoundedBuffer.h"
#include "UnboundedBuffer.h"
#include <iostream>
#include "Dispatcher.h"
using namespace std;


Dispatcher::Dispatcher(vector<shared_ptr<BoundedBuffer<string>>>& bufferArray,
            UnboundedBuffer<std::string>& sportBuf,
            UnboundedBuffer<std::string>& newsBuf,
            UnboundedBuffer<std::string>& weatherBuf)
    : buffers(bufferArray),
        bufferLength(bufferArray.size()),  // Initialize bufferLength correctly
        doneCounter(0),  // Initialize doneCounter to 0
        sportBuffer(sportBuf),
        newsBuffer(newsBuf),
        weatherBuffer(weatherBuf) {}

void Dispatcher::processBuffers() {
    while (doneCounter < bufferLength) {
        for (auto& buffer : buffers) {
            std::string message = buffer->remove();
            if (message == "DONE") {
                doneCounter++;
                break;
            }
            if (message.find("SPORT") != std::string::npos) {
                sportBuffer.insert(message);
            } else if (message.find("NEWS") != std::string::npos) {
                newsBuffer.insert(message);
            } else if (message.find("WEATHER") != std::string::npos) {
                weatherBuffer.insert(message);
            }
        }
    }
    sportBuffer.insert("DONE");
    newsBuffer.insert("DONE");
    weatherBuffer.insert("DONE");
}
