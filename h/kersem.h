#ifndef _kersem_h_
#define _kersem_h_

class PCB;
class Semaphore;
class List;
class SemList;

typedef unsigned int Time;

class KernelSem {
public:
	List* getBlockedThreadsList();
	List* getTimeBlockedThreadsList();
	static SemList* getSemaphoresList();
	static void deleteSemaphoresList();

	static void updateTimeOnSemaphores();

	static SemList* semaphoresList;

protected:
	KernelSem(int init);
	~KernelSem();

private:
	int value;
	List* blockedThreadsList;
	List* timeBlockedThreadsList;

	void block(Time maxTimeToWait);
	void deblock(int timeDeblock=0);

	int wait(Time maxTimeToWait);
	int signal(int n=0, int timeDeblock=0);

	int val() const;

	friend class Semaphore;
	friend class List;
	friend class SemList;

	static int semaphoreId;
};

#endif
