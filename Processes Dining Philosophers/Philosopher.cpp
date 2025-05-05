#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

#define TIMES_TO_EAT 1000000
#define AMOUNT_OF_PHILOSOPHERS 5

int main(int argc, char* argv[]) {
    /*
    if (argc != 3) {
        std::cout << "Usage: philosopher.exe <philosopher_number> <log_file>" << std::endl;
        return 1;
    }
        */

    // check if we got the philosopher id
    if(argc != 2) {
        std::cout << "Usage: philosopher.exe <philosopher_number>" << std::endl;
        return 1;
    }

    // Get philosopher ID and log file name from command line arguments
    int philosopherId = std::stoi(argv[1]);
    //std::string logFileName = argv[2];
    
    /*
    std::ofstream logFile(logFileName, std::ios::out | std::ios::app);
    if (!logFile.is_open()) {
        std::cout << "Failed to open log file" << std::endl;
        return 1;
    }
    */

    // Get handles to the mutexes for left and right forks
    std::string leftForkName = "Fork_" + std::to_string(philosopherId);
    std::string rightForkName = "Fork_" + std::to_string((philosopherId + 1) % AMOUNT_OF_PHILOSOPHERS);

    // Open mutex handles
    HANDLE leftFork = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, leftForkName.c_str());
    HANDLE rightFork = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, rightForkName.c_str());

    // Check if mutex handles were opened successfully
    if (leftFork == NULL || rightFork == NULL) {
        //logFile << "Failed to open mutex handles" << std::endl;
        std::cout << "Failed to open mutex handles" << std::endl;
        return 1;
    }

    // Eat a certain amount of times
    for (int i = 0; i < TIMES_TO_EAT; i++) {
        //logFile << "Philosopher " << philosopherId << " is thinking." << std::endl;

        // Pick up forks
        if (philosopherId == AMOUNT_OF_PHILOSOPHERS - 1) {
            // Last philosopher picks up right fork first
            WaitForSingleObject(rightFork, INFINITE);
            WaitForSingleObject(leftFork, INFINITE);
        }
        else {
            // Others pick up left fork first
            WaitForSingleObject(leftFork, INFINITE);
            WaitForSingleObject(rightFork, INFINITE);
        }

        //logFile << "Philosopher " << philosopherId << " is eating." << std::endl;

        // Release forks
        ReleaseMutex(leftFork);
        ReleaseMutex(rightFork);
    }

    // Clean up
    CloseHandle(leftFork);
    CloseHandle(rightFork);
    //logFile.close();

    return 0;
}