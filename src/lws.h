#ifndef LWS
#define LWS

#include "common.cpp"
#include "libwebsockets.h"
#include "thread.h"

using namespace std;

#define MAX_PAYLOAD_SIZE 10 * 1024
#define MAX_CONNECTION 10
#define WRITE_BUFFER_SIZE 1000000

struct session_data
{
	int id;
	lws *wsi;
	bool ready;
	string path;
};

struct stream_config
{
	unsigned char writeBuffer[WRITE_BUFFER_SIZE];
	int writeLength;
	string path;
};

class WebSocket
{
public:
	struct lws_context *context;
	int id = 0;
	int port;
	session_data *sockets[MAX_CONNECTION];
	struct lws_protocols protocols[2];
	map<string, stream_config *> streams;
	Thread *thread;

	void write(string path);
	void lws_thread();
	void addConfig(string path);
	stream_config *getConfig(string path);
	void start();
	void wait();
	void stop();
	static WebSocket *Singleton(int port);
	static int callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);

private:
	static WebSocket *Current;
	WebSocket(int port);
	~WebSocket();
};

#endif