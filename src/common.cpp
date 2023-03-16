#ifndef COMMON
#define COMMON

#include <iostream>
#include <memory>
#include <pthread.h>
#include <unistd.h>
#include <chrono>

pthread_t run_thread(void *(*cb)(void *))
{
	pthread_t pthread;
	pthread_attr_t GtkAttr;

	// Start thread
	int result = pthread_attr_init(&GtkAttr);
	if (result != 0)
	{
		fprintf(stderr, "pthread_attr_init returned error %d\n", result);
		return 0;
	}

	void *pParam = NULL;
	result = pthread_create(&pthread, &GtkAttr, cb, pParam);
	if (result != 0)
	{
		fprintf(stderr, "pthread_create returned error %d\n", result);
		return 0;
	}

	return pthread;
}

#endif