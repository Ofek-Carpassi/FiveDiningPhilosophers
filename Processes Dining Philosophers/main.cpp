#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

#define AMOUNT_OF_PHILOSOPHERS 5
#define AMOUNT_OF_FORKS AMOUNT_OF_PHILOSOPHERS

int main() {
    // Create an array of mutexes that represent forks
    HANDLE forkMutexes[AMOUNT_OF_FORKS];

    // Create a mutex for each fork
    for (int i = 0; i < AMOUNT_OF_FORKS; i++) {
        // Each fork will have unique name (Fork_0, Fork_1, etc.)
        std::string mutexName = "Fork_" + std::to_string(i);
        forkMutexes[i] = CreateMutexA(
            NULL,               
            FALSE,             
            mutexName.c_str()  
        );
        
        // Check if mutex creation was successful
        if (forkMutexes[i] == NULL) {
            std::cout << "CreateMutex error: " << GetLastError() << std::endl;
            return 1;
        }
    }

    // Create an array of STARTUPINFO and PROCESS_INFORMATION structures for each philosopher
    STARTUPINFO si[AMOUNT_OF_PHILOSOPHERS];
    PROCESS_INFORMATION pi[AMOUNT_OF_PHILOSOPHERS];
    HANDLE processHandles[AMOUNT_OF_PHILOSOPHERS];  // Array to store process handles
    
    /*
     * EVERY COMMENTED LINE OF CODE FROM HERE THAT INCLUDES LOG FILE IS JUST TO CREATE FILE TO MAKE SURE EVERYTHING WORKS!
     * 
    std::string logFileName = "philosophers.log";
    std::ofstream logFile(logFileName, std::ios::out | std::ios::app);
    if (!logFile.is_open()) {
        std::cout << "Failed to open log file" << std::endl;
        return 1;
    }
    */

    // Start a clock to measure the time it takes to run the program
    clock_t start = clock();

    // Create a process for each philosopher
    for (int i = 0; i < AMOUNT_OF_PHILOSOPHERS; i++) {
        // Create a command line string for each philosopher
        ZeroMemory(&si[i], sizeof(si[i]));
        si[i].cb = sizeof(si[i]);
        ZeroMemory(&pi[i], sizeof(pi[i]));

        //std::string command = "philosopher.exe " + std::to_string(i) + " " + logFileName;
        std::string command = "philosopher.exe " + std::to_string(i); // Command (the philosopher program) to be executed
        
        // Create the process using the command line string and the correct STARTUPINFO and PROCESS_INFORMATION structures
        if (!CreateProcessA(
            NULL,                   
            (LPSTR)command.c_str(), 
            NULL,                   
            NULL,                   
            FALSE,                  
            0,                      
            NULL,                   
            NULL,                   
            &si[i],                 
            &pi[i]                  
        )) {
            //logFile << "CreateProcess failed for philosopher " << i << std::endl;
            std::cout << "CreateProcess failed for philosopher " << i << std::endl;
            return 1;
        }
        processHandles[i] = pi[i].hProcess;  // Store the process handle
    }

    // Wait for all philosophers to finish using the process handles
    WaitForMultipleObjects(AMOUNT_OF_PHILOSOPHERS, processHandles, TRUE, INFINITE);

    // Stop the clock and calculate the time it took to run the program
    clock_t end = clock();

    // Clean up (Close all handles) - we can do amount of philosophers and forks because they are the same
    for (int i = 0; i < AMOUNT_OF_PHILOSOPHERS; i++) {
        CloseHandle(pi[i].hProcess);
        CloseHandle(pi[i].hThread);
        CloseHandle(forkMutexes[i]);
    }

    // Print the time it took to run the program
    std::cout << "Time: " << (double)(end - start) / CLOCKS_PER_SEC << std::endl;
    //logFile << "Time: " << (double)(end - start) / CLOCKS_PER_SEC << std::endl;

    //logFile.close();
    return 0;
}