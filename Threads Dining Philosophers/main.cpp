#include <iostream>
#include <Windows.h>
#include <ctime>

// Define the amount of philosophers, forks and how many times each philosopher will eat
#define AMOUNT_OF_PHILOSOPHERS 5
#define AMOUNT_OF_FORKS AMOUNT_OF_PHILOSOPHERS
#define TIMES_TO_EAT 1000000

// Create a philosopher stucture so we could save for each philosopher the id and the forks he can use
struct Philosopher {
	int id;
	CRITICAL_SECTION* leftFork;
	CRITICAL_SECTION* rightFork;
} typedef Philosopher;

// The philosopher function that will be called when creating a thread
DWORD WINAPI philosopher(LPVOID param) {
	// Cast the parameter to a Philosopher pointer
	Philosopher* philosopher = (Philosopher*)param;

	for (int i = 0; i < TIMES_TO_EAT; i++) {
		// Think (supposed to sleep but we don't want to wait in our check)
		//Sleep(100);

		// Pick up forks (critical section so only one philosopher can pick up a fork at a time)
		if (philosopher->id == AMOUNT_OF_PHILOSOPHERS - 1) {
			// Last philosopher picks up right fork first
			EnterCriticalSection(philosopher->rightFork);
			EnterCriticalSection(philosopher->leftFork);
		}
		else {
			// Others pick up left fork first
			EnterCriticalSection(philosopher->leftFork);
			EnterCriticalSection(philosopher->rightFork);
		}

		// Eat (supposed to sleep but we don't want to wait in our check)
		//Sleep(100);

		// Put down forks
		LeaveCriticalSection(philosopher->leftFork);
		LeaveCriticalSection(philosopher->rightFork);
	}

	return 0;
}

int main() {
	// Save a critical section for each fork and a thread for each philosopher
	CRITICAL_SECTION forks[AMOUNT_OF_FORKS];
	HANDLE philosophers[AMOUNT_OF_PHILOSOPHERS];

	// Save the data for each philosopher
	Philosopher philosopherData[AMOUNT_OF_PHILOSOPHERS];

	// Initialize all of the critical sections for the forks
	for (int i = 0; i < AMOUNT_OF_FORKS; i++)
		InitializeCriticalSection(&forks[i]);

	// Create philosophers
	for (int i = 0; i < AMOUNT_OF_PHILOSOPHERS; i++) {
		philosopherData[i] = {
			i,
			&forks[i],
			&forks[(i + 1) % AMOUNT_OF_FORKS]
		};
	}

	// Start the timer
	clock_t start = clock();

	// Create philosopher threads
	for (int i = 0; i < AMOUNT_OF_PHILOSOPHERS; i++) {
		int id = i;
		philosophers[i] = CreateThread(
			NULL,
			0,
			philosopher,
			&philosopherData[i],
			0,
			NULL
		);
	}

	// Wait for all philosophers to finish
	WaitForMultipleObjects(AMOUNT_OF_PHILOSOPHERS, philosophers, TRUE, INFINITE);

	// Stop the timer
	clock_t end = clock();

	// Clean up the critical sections
	for (int i = 0; i < AMOUNT_OF_FORKS; i++) {
		DeleteCriticalSection(&forks[i]);
	}

	// Print the time it took to run the program
	std::cout << "Time: " << (double)(end - start) / CLOCKS_PER_SEC << std::endl;

	return 0;
}