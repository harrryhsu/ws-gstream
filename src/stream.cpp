#ifndef STREAM
#define STREAM

#include <gst/gst.h>
#include <glib.h>
#include <gst/app/gstappsink.h>
#include "./common.cpp"
#include "./lws.cpp"

static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data);

static GstElement *appsink, *pipeline;
static GMainLoop *loop;
static GstBus *bus;

void *gst_thread(void *ptr)
{
	guint bus_watch_id;

	loop = g_main_loop_new(NULL, FALSE);

	auto rtsp =
			"rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mp4";
	// "rtsp://admin:903fjjjjj@192.168.1.203/Streaming/Channels/201";

	std::string pipelineStr = "rtspsrc location=rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mp4 name=src !\
									rtph264depay ! h264parse ! avdec_h264 !\
									queue ! videoconvert !\
									x264enc bitrate=1000000 bframes=0 key-int-max=10 weightb=false speed-preset=1 cabac=false tune=zerolatency !\
									appsink name=sink";

	pipeline = gst_parse_launch(pipelineStr.c_str(), nullptr);
	appsink = gst_bin_get_by_name((GstBin *)pipeline, "sink");

	/* we add a message handler */
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	bus_watch_id = gst_bus_add_watch(bus, bus_call, loop);
	gst_object_unref(bus);

	/* Set the pipeline to "playing" state*/
	g_print("Now set pipeline in state playing\n");
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	/* Iterate */
	g_print("Running...\n");
	g_main_loop_run(loop);

	/* Out of the main loop, clean up nicely */
	g_print("Returned, stopping playback\n");
	gst_element_set_state(pipeline, GST_STATE_NULL);

	g_print("Deleting pipeline\n");
	gst_object_unref(GST_OBJECT(pipeline));
	g_source_remove(bus_watch_id);
	g_main_loop_unref(loop);

	return nullptr;
}

static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data)
{
	GMainLoop *loop = (GMainLoop *)data;

	switch (GST_MESSAGE_TYPE(msg))
	{

	case GST_MESSAGE_EOS:
		g_print("End of stream\n");
		g_main_loop_quit(loop);
		break;

	case GST_MESSAGE_ERROR:
	{
		gchar *debug;
		GError *error;

		gst_message_parse_error(msg, &error, &debug);
		g_free(debug);

		g_printerr("Error: %s\n", error->message);
		g_error_free(error);

		g_main_loop_quit(loop);
		break;
	}
	default:
		break;
	}

	return TRUE;
}

char *pull_frame(int *outlen)
{
	if (!appsink)
		return NULL;

	GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(appsink));

	if (sample == NULL)
		return NULL;

	GstBuffer *buffer = gst_sample_get_buffer(sample);
	GstMapInfo map;
	gst_buffer_map(buffer, &map, GST_MAP_READ);

	char *pRet = new char[map.size];
	memmove(pRet, map.data, map.size);

	gst_buffer_unmap(buffer, &map);
	gst_sample_unref(sample);

	*outlen = map.size;

	return pRet;
}

void *stream_thread(void *ptr)
{
	gst_init(nullptr, nullptr);
	pthread_t gst = run_thread(gst_thread);
	int len = 0;
	char *frame;

	while (1)
	{
		frame = pull_frame(&len);
		if (frame != NULL)
			write((unsigned char *)frame, len);

		usleep(20 * 1000);
	}

	pthread_join(gst, nullptr);
}

#endif