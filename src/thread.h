#ifndef THREAD
#define THREAD

#include "./common.cpp"

class Thread
{
public:
	Thread(std::function<void(void)> process)
	{
		this->process = process;
	}
	virtual ~Thread()
	{
	}

	bool start()
	{
		return (pthread_create(&_thread, NULL, startInternal, this) == 0);
	}

	void wait()
	{
		(void)pthread_join(_thread, NULL);
	}

	void kill()
	{
		pthread_kill(_thread, SIGUSR1);
	}

private:
	static void *startInternal(void *This)
	{
		((Thread *)This)->internalProcess();
		return NULL;
	}

	void internalProcess()
	{
		process();
	}

	pthread_t _thread;
	std::function<void(void)> process;
};

#endif