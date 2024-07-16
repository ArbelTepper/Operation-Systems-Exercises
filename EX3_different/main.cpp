#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <queue>
#include <semaphore>
#include <random>
#include <mutex>
#include <memory> // Include this header for std::unique_ptr and std::make_unique

using namespace std;

template <typename T>
class BoundedBuffer {
private:
    queue<string> buffer;
    counting_semaphore<> empty;
    counting_semaphore<> full;
    mutex mtx;
    bool finished = false;

public:
    BoundedBuffer(int n) : empty(n), full(0) {}

    void insert(string s) {
        empty.acquire();
        {
            lock_guard lock(mtx);
            buffer.push(s);
        }
        full.release();
    }

    string remove() {
        if (finished) {
            return "DONE";
        }
        full.acquire();
        string s;
        {
            lock_guard lock(mtx);
            s = buffer.front();
            if (s == "DONE") {
                finished = true;
            }
            buffer.pop();
        }
        empty.release();
        return s;
    }
};

class UnBoundedBuffer {
private:
    queue<string> buffer;
    mutex mtx;
    bool finished = false;

public:
    UnBoundedBuffer() {}

    void insert(string s) {
        lock_guard lock(mtx);
        buffer.push(s);
    }

    string remove() {
        string s = "";
        if (!buffer.empty()) {
            lock_guard lock(mtx);
            s = buffer.front();
            buffer.pop();
        }
        return s;
    }
};

class Producer {
private:
    BoundedBuffer<std::string>& buffer; // Reference to a single shared buffer
    int producerID;
    int itemsToProduce; // Number of items to produce, added as per requirement
    int sportCounter = 0;
    int newsCounter = 0;
    int weatherCounter = 0;

public:
    Producer(int id, BoundedBuffer<std::string>& buf, int numItems)
        : buffer(buf), producerID(id), itemsToProduce(numItems) {}

    void produce() {
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
			cout << message << endl;
            buffer.insert(message);
        }
        // Insert "DONE" after producing the specified number of items
        buffer.insert("DONE");
    }
};

class Dispatcher {
private:
    vector<unique_ptr<BoundedBuffer<string>>>& buffers; // Reference to an array of shared buffers
    UnBoundedBuffer& sportsBuffer;
    UnBoundedBuffer& newsBuffer;
    UnBoundedBuffer& weatherBuffer;
    int bufferLength;
    int doneCounter;

public:
    Dispatcher(vector<unique_ptr<BoundedBuffer<std::string>>>& buffs, UnBoundedBuffer& sportsBuf, UnBoundedBuffer& newsBuf, UnBoundedBuffer& weatherBuf)
        : buffers(buffs), sportsBuffer(sportsBuf), newsBuffer(newsBuf), weatherBuffer(weatherBuf) {}

    void processBuffers() {
        bool finished = false;
        while (!finished) {
            finished = true;
            for (auto& buffer : buffers) {
                string message = buffer->remove();
                if (message == "DONE") {
                    continue;
                }
                string topic = message.substr(message.find(" ") + 1, message.find(" ", message.find(" ") + 1) - message.find(" ") - 1);
                if (topic == "SPORT") {
                    sportsBuffer.insert(message);
                } else if (topic == "NEWS") {
                    newsBuffer.insert(message);
                } else if (topic == "WEATHER") {
                    weatherBuffer.insert(message);
                }
                finished = false;
            }
        }
        sportsBuffer.insert("DONE");
        newsBuffer.insert("DONE");
        weatherBuffer.insert("DONE");
    }
};

class CoEditor {
private:
    UnBoundedBuffer& inBuffer;
    UnBoundedBuffer& outBuffer;

public:
    CoEditor(UnBoundedBuffer& buffer, UnBoundedBuffer& screenBuffer) : inBuffer(buffer), outBuffer(screenBuffer) {}

    void processAndForward() {
        while (true) {
            string message = inBuffer.remove();
            if (message == "DONE") {
                outBuffer.insert("DONE");
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            outBuffer.insert(message);
        }
    }
};

class ScreenManager {
private:
    UnBoundedBuffer& screenBuffer;
    int finishedCount = 0;

public:
    ScreenManager(UnBoundedBuffer& buffer) : screenBuffer(buffer) {}
    void printMessages() {
        while (finishedCount < 3) {
            string message = screenBuffer.remove();
            if (message == "DONE") {
                finishedCount++;
                continue;
            }
            if (message != "") {
                std::cout << message << std::endl;
            }
        }
        std::cout << "DONE" << std::endl;
    }
};

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
    std::vector<unique_ptr<BoundedBuffer<std::string>>> buffers; // Array to store buffers

    for (size_t i = 0; i < productsCount.size(); i++) {
        int numProducts = productsCount[i];
        int queueSize = queueSizes[i];

        // Create a bounded buffer and store it in the buffers array
        buffers.push_back(std::make_unique<BoundedBuffer<std::string>>(queueSize));

        // Create a producer object using the numProducts and the last buffer created
        producers.emplace_back(i, *buffers.back(), numProducts);
    }

    // Create unbounded buffers for sports, news, and weather
    UnBoundedBuffer sportsBuffer;
    UnBoundedBuffer newsBuffer;
    UnBoundedBuffer weatherBuffer;

    // Create a dispatcher object
    Dispatcher dispatcher(buffers, sportsBuffer, newsBuffer, weatherBuffer);

    // Create a buffer for the screen manager
    UnBoundedBuffer screenBuffer;

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

    // Join all threads
    for (auto& thread : producerThreads) {
        thread.join();
    }

    threadDispatcher.join();
    threadSports.join();
    threadNews.join();
    threadWeather.join();
    threadScreenManager.join();

    return 0;
}
