#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>

class Disk {
public:
    Disk(int size, int head, std::string direction)
        : size_(size), head_(head), direction_(direction), requests_(), seek_count_(0) {}

    void addRequest(int request) {
        std::unique_lock<std::mutex> lock(mutex_);
        requests_.push_back(request);
        cv_.notify_one(); // Notify the SCAN thread that a new request has been added
    }

    void SCAN() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (true) {
            cv_.wait(lock, [this] { return !requests_.empty(); }); // Wait for a request to be added

            // Separate requests into left and right based on their position relative to the head
            std::vector<int> left, right;
            for (int request : requests_) {
                if (request < head_) {
                    left.push_back(request);
                } else if (request > head_) {
                    right.push_back(request);
                }
            }

            // Sort left and right vectors
            std::sort(left.begin(), left.end());
            std::sort(right.begin(), right.end());

            // Process left and right requests
            if (direction_ == "left") {
                for (int i = left.size() - 1; i >= 0; --i) {
                    seek_count_ += abs(left[i] - head_);
                    head_ = left[i];
                }
                direction_ = "right";
            }

            if (direction_ == "right") {
                for (int request : right) {
                    seek_count_ += abs(request - head_);
                    head_ = request;
                }
                direction_ = "left";
            }

            // Clear processed requests
            requests_.clear();
        }
    }

    void printResults() {
        std::cout << "Total number of seek operations = " << seek_count_ << std::endl;
        std::cout << "Seek Sequence is: ";
        for (int request : requests_) {
            std::cout << request << " ";
        }
        std::cout << std::endl;
    }

private:
    int size_;
    int head_;
    std::string direction_;
    std::vector<int> requests_;
    int seek_count_;
    std::mutex mutex_;
    std::condition_variable cv_;
};
