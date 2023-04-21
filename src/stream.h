#ifndef STREAM
#define STREAM

#include <gst/gst.h>
#include <glib.h>
#include <gst/app/gstappsink.h>
#include "./common.cpp"
#include "./lws.h"
#include "thread.h"

using namespace std;
class Stream
{
private:
	GstElement *appsink, *pipeline;
	GMainLoop *loop;
	GstBus *bus;
	WebSocket *lws;
	Thread *thread;
	string path;
	string url;
	bool started;
	bool gst_loop_started = false;
	bool h265 = false;

	int width = 1080; // 2048
	int height = 720; // 2080

	static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data);
	bool pull_frame();

public:
	Stream(WebSocket *lws, string path, string url);
	~Stream();
	void gst_thread();
	void stream_thread();
	void start();
	void wait();
	void stop();
	void useH265();
	void setSize(int width, int height);
};

#endif