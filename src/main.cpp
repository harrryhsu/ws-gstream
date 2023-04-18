#include "stream.h"

int main(int argc, char *argv[])
{
	WebSocket *ws = WebSocket::Singleton(8080);
	Stream stream1(ws, "/test1", "rtsp://host.docker.internal:8554/test");
	Stream stream2(ws, "/test2", "rtsp://host.docker.internal:8554/test");
	stream1.start();
	stream2.start();
	ws->start();
	ws->wait();

	return 0;
}