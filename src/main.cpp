#include "stream.cpp"
#include "lws.cpp"

void __main()
{
	pthread_t stream = run_thread(stream_thread);
	pthread_t lws = run_thread(lws_thread);
	pthread_join(stream, nullptr);
	pthread_join(lws, nullptr);
}

int main(int argc, char *argv[])
{
	__main();
	return 0;
}