

#include "Account.h"
#include "Atm.h"
#include <map>
#include <stack>

using namespace std;

const int threadSuccess = 0;

/************** Declare Globals ****************/
map<string, Account> accountsMap;
bool commissionDone;
int mapReadersNum;
pthread_mutex_t readersMutex;
pthread_mutex_t writersMutex;
pthread_mutex_t logMutex;
pthread_mutex_t bankMutex;

static void mainDestroy(pthread_mutex_t* readersMutex, pthread_mutex_t* writersMutex,
		pthread_mutex_t* logMutex, pthread_mutex_t* bankMutex, pthread_t * atmThreads);


int main(int argc, char *argv[]) {

	pthread_mutex_init(&readersMutex, NULL);
	pthread_mutex_init(&writersMutex, NULL);
	pthread_mutex_init(&logMutex, NULL);
	pthread_mutex_init(&bankMutex, NULL);
	mapReadersNum = 0;
	commissionDone = false;
	stack<Atm> atmsStack;
	// if there are no arguments at all
	if (argc <= 1) {
		cerr << "illegal arguments" << endl;
		return -1;
	}

	int atmNum = stoi(string(argv[1]));
	if (argc - 2 != atmNum) {
		cerr << "illegal arguments" << endl;
		return -1;
	}
	ofstream{"log.txt"};

	/************** Create Print Thread ****************/
	pthread_t printThread;
	int threadCreateError = pthread_create(&printThread, NULL, printAnimation, NULL);
	if(threadCreateError != threadSuccess) {
		cerr << "Error: thread create had failed" <<endl;
		mainDestroy(&readersMutex, &writersMutex,
					&logMutex, &bankMutex, nullptr);
				return -1;
	}

	/************** Create Commission Thread ****************/
	pthread_t commissionThread;
	threadCreateError = pthread_create(&commissionThread, NULL, takeComission,
	NULL);
	if (threadCreateError != threadSuccess) {
		cerr << "Error: thread create had failed" << endl;
		mainDestroy(&readersMutex, &writersMutex,
				&logMutex, &bankMutex, nullptr);
		return -1;
	}

	/************** Create ATMs Threads ****************/
	pthread_t *atmThreads = new pthread_t[atmNum];
	string currFileName = "";
	for (int i = 0; i < atmNum; i++) {
		currFileName = string(argv[2 + i]);
		//construct and push
		atmsStack.push(Atm(i + 1, currFileName));

		// Create ATM threads
		threadCreateError = pthread_create(&atmThreads[i], NULL,
				atmExeCommandsFunc, static_cast<void*>(&(atmsStack.top())));
		if (threadCreateError != threadSuccess) {
			cerr << "Error: thread create had failed" << endl;
			mainDestroy(&readersMutex, &writersMutex, &logMutex, &bankMutex,
					atmThreads);

			return -1;
		}
	}

	int threadJoinError;

	/************** Join ATMs Threads ****************/
	for (int i = 0; i < atmNum; ++i) {
		threadJoinError = pthread_join(atmThreads[i], NULL);
		if (threadJoinError != threadSuccess) {
			cerr << "Error: thread join had failed" << endl;
			mainDestroy(&readersMutex, &writersMutex, &logMutex, &bankMutex,
					atmThreads);
			return -1;
		}
	}

	/************** Join Print Thread ****************/
	commissionDone = true;

	threadJoinError = pthread_join(printThread, NULL);
	if (threadJoinError != threadSuccess) {
		cerr << "Error: thread join had failed" << endl;
		mainDestroy(&readersMutex, &writersMutex, &logMutex, &bankMutex,
				atmThreads);
		return -1;
	}

	/************** Join Commission Thread ****************/
	threadJoinError = pthread_join(commissionThread, NULL);
	if (threadJoinError != threadSuccess) {
		cerr << "Error: thread join had failed" << endl;
		mainDestroy(&readersMutex, &writersMutex, &logMutex, &bankMutex,
				atmThreads);
		return -1;
	}
	mainDestroy(&readersMutex, &writersMutex, &logMutex, &bankMutex,
					atmThreads);
	return 0;

}

static void mainDestroy(pthread_mutex_t* readersMutex,
		pthread_mutex_t* writersMutex, pthread_mutex_t* logMutex,
		pthread_mutex_t* bankMutex, pthread_t * atmThreads) {
	pthread_mutex_destroy(readersMutex);
	pthread_mutex_destroy(writersMutex);
	pthread_mutex_destroy(logMutex);
	pthread_mutex_destroy(bankMutex);
	if (atmThreads == nullptr) {
		return;
	} else {
		delete[] atmThreads;
	}
	return;
}

