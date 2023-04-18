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
};

#endif