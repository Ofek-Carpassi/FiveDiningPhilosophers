#include <iostream>
#include <Windows.h>
#include <ctime>

#define AMOUNT_OF_PHILOSOPHERS 5
#define AMOUNT_OF_FORKS AMOUNT_OF_PHILOSOPHERS
#define TIMES_TO_EAT 1000000

struct Philosopher {
	int id;
	CRITICAL_SECTION* leftFork;
	CRITICAL_SECTION* rightFork;
} typedef Philosopher;

DWORD WINAPI philosopher(LPVOID param) {
	Philosopher* philosopher = (Philosopher*)param;

	for (int i = 0; i < TIMES_TO_EAT; i++) {
		// Think
		//Sleep(100);

		// Pick up forks
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

		// Eat
		//Sleep(100);

		// Put down forks
		LeaveCriticalSection(philosopher->leftFork);
		LeaveCriticalSection(philosopher->rightFork);
	}

	return 0;
}

int main() {
	CRITICAL_SECTION forks[AMOUNT_OF_FORKS];
	HANDLE philosophers[AMOUNT_OF_PHILOSOPHERS];
	Philosopher philosopherData[AMOUNT_OF_PHILOSOPHERS];

	// We will only use Critical Sections in this example
	for (int i = 0; i < AMOUNT_OF_FORKS; i++) {
		InitializeCriticalSection(&forks[i]);
	}

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

	// Create philosophers
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

	// Clean up
	for (int i = 0; i < AMOUNT_OF_FORKS; i++) {
		DeleteCriticalSection(&forks[i]);
	}

	// Print the time it took
	std::cout << "Time: " << (double)(end - start) / CLOCKS_PER_SEC << std::endl;

	return 0;
}