#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include "Producer.h"
#include "BoundedBuffer.h"
#include "Dispatcher.h"
#include "CoEditor.h"
#include "ScreenManager.h"


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    std::vector<int> productsCount;
    std::vector<int> queueSizes;

    if (!file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }

    std::string line;
    while (getline(file, line)) {
        if (line.find("PRODUCER") != std::string::npos) {
            // Read number of products
            getline(file, line);
            int numProducts = std::stoi(line);
            productsCount.push_back(numProducts);

            // Read queue size
            getline(file, line);
            size_t pos = line.find("=");
            int queueSize = std::stoi(line.substr(pos + 1));
            queueSizes.push_back(queueSize);
        }
    }

    file.close();

    std::vector<Producer> producers; // Array to store producers
    std::vector<BoundedBuffer<std::string>> buffers; // Array to store buffers

    for (size_t i = 0; i < productsCount.size(); i++) {
        int numProducts = productsCount[i];
        int queueSize = queueSizes[i];

        // Create a bounded buffer and store it in the buffers array
        buffers.emplace_back(queueSize);

        // Create a producer object using the numProducts and the last buffer created
        producers.emplace_back(i, buffers.back(), numProducts);
    }

    // Create unbounded buffers for sports and weather
    UnboundedBuffer<std::string> sportsBuffer;
    UnboundedBuffer<std::string> weatherBuffer;
    UnboundedBuffer<std::string> newsBuffer;

    // Create a dispatcher object
    Dispatcher dispatcher(buffers, sportsBuffer, newsBuffer ,weatherBuffer);

    // Create a buffer for the screen manager
    UnboundedBuffer<std::string> screenBuffer;

    // Create a screen manager object using the screen buffer
    ScreenManager screenManager(screenBuffer);

    // Create co-editors for each buffer
    CoEditor coEditorSports(sportsBuffer, screenBuffer);
    CoEditor coEditorNews(newsBuffer, screenBuffer);
    CoEditor coEditorWeather(weatherBuffer, screenBuffer);

    // Create threads for each producer
    std::vector<std::thread> producerThreads;
    for (auto& producer : producers) {
        producerThreads.emplace_back([&producer] { producer.produce(); });
    }
    
    // Create a thread for the dispatcher.
    std::thread threadDispatcher([&](){ dispatcher.processBuffers(); });

    // Create threads for each co-editor.
    std::thread threadSports([&](){ coEditorSports.processAndForward(); });
    std::thread threadNews([&](){ coEditorNews.processAndForward(); });
    std::thread threadWeather([&](){ coEditorWeather.processAndForward(); });

    // Create a thread for the screen manager.
    std::thread threadScreenManager([&](){ screenManager.printMessages(); });

    // Join all producer threads
    for (auto& producerThread : producerThreads) {
        producerThread.join();
    }

    // Join the dispatcher thread
    threadDispatcher.join();

    // Join the co-editor threads
    threadSports.join();
    threadNews.join();
    threadWeather.join();

    // Join the screen manager thread
    threadScreenManager.join();

    return 0;
}