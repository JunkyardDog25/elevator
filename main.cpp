#include "disk.hpp"

int main() {
    Disk disk(200, 50, "left");
    std::thread scanThread(&Disk::SCAN, &disk);

    // Simulate adding disk requests
    for (int i = 0; i < 10; ++i) {
        disk.addRequest(i * 20); // Example requests
    }

    scanThread.join(); // Wait for the SCAN thread to finish
    disk.printResults(); // Print the results

    return 0;
}
