#ifndef THREADS_H
#define THREADS_H

#ifndef WIN32

#include <unistd.h>

#ifdef __linux__
#	define __USE_GNU // defines recursive mutex'es in pthreads.h
#endif
#include <pthread.h>

#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <cstring>

#define ETIMEDOUT 110

inline void relax_ms(unsigned long microseconds)
{
	usleep(microseconds*1000); 
}

inline void relax(double seconds)
{
	relax_ms((unsigned long)(seconds*1000)); 
}

#ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
// Linux supports recursive locks, use them directly, with some cheating:
class Lock {
public:
	Lock() {
	    pthread_mutex_t m=PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
	    memcpy(&mutex, &m, sizeof(pthread_mutex_t));
	};
	~Lock() { pthread_mutex_destroy(&mutex); };
	void lock() { pthread_mutex_lock(&mutex); };
	void unlock() { pthread_mutex_unlock(&mutex); };
private:
	pthread_mutex_t mutex;
	static pthread_mutexattr_t mutex_attrib;
private:
	Lock(const Lock &) {}; // disable!
	void operator=(const Lock &) {}; // disable!
};

#else

class Lock {
public:
	Lock(): counter(0) { pthread_mutex_init(&mutex, 0); };
	~Lock() { pthread_mutex_destroy(&mutex); };
	void lock() {
		if(!counter || owner != pthread_self()) {
			pthread_mutex_lock(&mutex);
			owner = pthread_self();
		}
		counter++;
		pthread_mutex_lock(&mutex);
	};
	void unlock() {
		if(!--counter) {
			pthread_mutex_unlock(&mutex);
		}
	};
private:
	pthread_mutex_t mutex;
	pthread_t owner;
	int counter;
private:
	Lock(const Lock &) {}; // disable!
	void operator=(const Lock &) {}; // disable!
};
#endif

class Trigger {
public:
	Trigger() : is_up(true) {
		pthread_cond_init(&cond, 0);
		pthread_mutex_init(&mutex, 0);
	};
	~Trigger() {
		pthread_cond_destroy(&cond);
		pthread_mutex_destroy(&mutex);
	};

	bool up() { return is_up; };

	void wait() {
		pthread_mutex_lock(&mutex);
		if(up()) {
			pthread_cond_wait(&cond, &mutex);
		}
		pthread_mutex_unlock(&mutex);
	};
	
	bool wait_ms(unsigned long microseconds) {
		struct timeval now;
		struct timespec timeout;
		pthread_mutex_lock(&mutex);
		int result=0;
		if(up()) {
			gettimeofday(&now, NULL);
			timeout.tv_sec = now.tv_sec + microseconds/1000;
			timeout.tv_nsec = now.tv_usec*1000+(microseconds%1000);
			result=pthread_cond_timedwait(&cond, &mutex, &timeout);
		}
		pthread_mutex_unlock(&mutex);
		return (result!=ETIMEDOUT); // it returns false on timeout
	};

	bool wait(double seconds) { return wait_ms((unsigned long)(seconds*1000)); };
	
	void toggle() {
		pthread_mutex_lock(&mutex);
		if(up()) {
			pthread_cond_broadcast(&cond);
		}
		is_up=!is_up;
		pthread_mutex_unlock(&mutex);
	};

private:
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	bool is_up;
private:
	Trigger(const Trigger &) {}; // disable!
	void operator=(const Trigger &) {}; // disable!
};

class Signal {
public:
	Signal() {
		pthread_cond_init(&cond, 0);
		pthread_mutex_init(&mutex, 0);
	};
	~Signal() {
		pthread_cond_destroy(&cond);
		pthread_mutex_destroy(&mutex);
	};

	void wait() {
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&cond, &mutex);
		pthread_mutex_unlock(&mutex);
	};
	
	bool wait_ms(unsigned long microseconds) {
		struct timeval now;
		struct timespec timeout;
		pthread_mutex_lock(&mutex);
		int result=0;
		gettimeofday(&now, NULL);
		timeout.tv_sec = now.tv_sec + microseconds/1000;
		timeout.tv_nsec = now.tv_usec*1000+(microseconds%1000);
		result=pthread_cond_timedwait(&cond, &mutex, &timeout);
		pthread_mutex_unlock(&mutex);
		return (result!=ETIMEDOUT); // it returns false on timeout
	};

	bool wait(double seconds) { return wait_ms((unsigned long)(seconds*1000)); };

	void notify() {
		pthread_mutex_lock(&mutex);
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
	};

private:
	pthread_cond_t cond;
	pthread_mutex_t mutex;
private:
	Signal(const Signal &) {}; // disable!
	void operator=(const Signal &) {}; // disable!
};

class Thread {
public:
	Thread(): priority(0.5f), thread_id(0) {};
	virtual ~Thread() { };

	bool create() {
		if(thread_id) {
			return false;
		}
		pthread_create(&thread_id, 0, (void*(*)(void*))thread_routine, this);
		return pthread_detach(thread_id)==0;
	};
	// 0.0f-idle, 0.5f-normal, 1.0f-realtime
	bool create(float prio) {
		if(thread_id) {
			return false;
		}
		priority=prio;
		pthread_create(&thread_id, 0, (void*(*)(void*))thread_routine, this);
		return pthread_detach(thread_id)==0;
	}

	void kill() { if(thread_id) pthread_kill(thread_id, SIGTERM); };

	virtual void thread()=0; // main routine.

	// 0.0f-idle, 0.5f-normal, 1.0f-realtime
	float rank() {
		return (thread_id)?
			(20-getpriority(PRIO_PROCESS,thread_id))/40.0f:-1.f;
	};
	void rank(float priority) {
		if(thread_id) setpriority(PRIO_PROCESS, thread_id, (int)(20-priority*40));
	};
	
private:
	static void* thread_routine(void* v) {
		Thread* thread=(Thread*)v;
		thread->rank(thread->priority);
		thread->thread();
		thread->thread_id=0;
		return 0;
	}
	float priority; // used only in thread_routine()
	pthread_t thread_id;
private:
	Thread(const Thread &) {}; // disable!
	void operator=(const Thread &) {}; // disable!
};

#undef ETIMEDOUT

#else

#include <windows.h>
#include <process.h>

inline void relax_ms(unsigned long microseconds)
{
	Sleep(microseconds); 
}

inline void relax(double seconds)
{
	relax_ms((unsigned long)(seconds*1000)); 
}

class Lock {
public:
	Lock() {
		InitializeCriticalSection(&cs);
	};
	~Lock() {
		DeleteCriticalSection(&cs);
	};
	void lock() {
		EnterCriticalSection(&cs);
	};
	void unlock() {
		LeaveCriticalSection(&cs);
	};
private:
	CRITICAL_SECTION cs;
private:
	Lock(const Lock &) {}; // disable!
	void operator=(const Lock &) {}; // disable!
};

class Trigger {
public:
	Trigger():is_up(true){  event=CreateEvent(NULL, FALSE, FALSE, NULL);  };
	~Trigger() { CloseHandle(event); };

	bool up() { return is_up; };

	void wait() {
		if(up()) {
			WaitForSingleObject(event, INFINITE);
		}
	};
	
	bool wait_ms(unsigned long microseconds) {
		DWORD result=0;
		if(up()) {
			result=WaitForSingleObject(event, (DWORD)microseconds);
		}
		return (result!=WAIT_TIMEOUT); // it returns false on timeout
	};

	bool wait(double seconds) { return wait_ms((unsigned long)(seconds*1000)); };
	
	void toggle() {
		if(is_up=!is_up) {
			ResetEvent(event);
		} else {
			SetEvent(event);
		}
	};

private:
	HANDLE event;
	bool is_up;
private:
	Trigger(const Trigger &) {}; // disable!
	void operator=(const Trigger &) {}; // disable!
};

class Signal {
public:
	Signal() { event=CreateEvent(NULL, FALSE, FALSE, NULL); };
	~Signal() { CloseHandle(event); };

	void wait() { WaitForSingleObject(event, INFINITE); };
	
	bool wait_ms(unsigned long microseconds) {
		// it returns false on timeout
		return (WaitForSingleObject(event, (DWORD)microseconds)!=WAIT_TIMEOUT);
	};

	bool wait(double seconds) { return wait_ms((unsigned long)(seconds*1000)); };

	void notify() {
		SetEvent(event);
	};

private:
	HANDLE event;
private:
	Signal(const Signal &) {}; // disable!
	void operator=(const Signal &) {}; // disable!
};

class Thread {
public:
	Thread(): thread_id(0) {};
	virtual ~Thread() { };

	// Hope that you are smart enough not to call create() on a
	// running thread!
	bool create() {
		if(thread_id) {
			return false;
		}
		unsigned dummy;
		thread_id=(HANDLE)_beginthreadex(0, 0, 
			(unsigned (__stdcall *) (void *))thread_routine, this, 0, &dummy);
		return true;
	};
	// 0.0f-idle, 0.5f-normal, 1.0f-realtime
	bool create(float prio) {
		if(thread_id) {
			return false;
		}
		priority=prio;
		unsigned dummy;
		thread_id=(HANDLE)_beginthreadex(0, 0,
			(unsigned (__stdcall *)(void *))thread_routine, this, 0, &dummy);
		return true;
	};

	void kill() { if(thread_id) TerminateThread(thread_id, 0); };

	virtual void thread()=0; // main routine.

	// 0.0f-idle, 0.5f-normal, 1.0f-realtime
	float rank() {
		return (thread_id)?(GetThreadPriority(thread_id)+2)/4.0f:-1.f;
	};
	void rank(float priority) {
		if(thread_id) SetThreadPriority(thread_id, (int)(4*priority-2));
	};
	
private:
	static unsigned __stdcall thread_routine(void* v) {
		Thread* thread=(Thread*)v;
		thread->rank(thread->priority);
		thread->thread();
		CloseHandle(thread->thread_id);
		thread->thread_id=0;
		return 0;
	};
	HANDLE thread_id;
	float priority;
private:
	Thread(const Thread &) {}; // disable!
	void operator=(const Thread &) {}; // disable!
};

#endif

// Usefull thing
class Guard {
public:
	Guard(Lock &m): l(m) {
		l.lock();
	};
	~Guard() {
		l.unlock();
	};
private:
	Lock &l;
private:
	Guard(const Guard&); // disable!
	Guard& operator=(const Guard&); // disable!
};

#endif
