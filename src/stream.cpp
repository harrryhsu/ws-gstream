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

	std::string pipelineStr = "rtspsrc location=rtsp://admin:903fjjjjj@192.168.1.203/Streaming/Channels/201 name=src latency=0 buffer-mode=none !\
											decodebin ! videoscale ! video/x-raw,width=2048,height=1080 !\
											x264enc bitrate=500000 bframes=0 key-int-max=100 weightb=false speed-preset=ultrafast cabac=false tune=zerolatency !\
											appsink name=sink";

	// std::string pipelineStr = "rtspsrc location=rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mp4 name=src !\
	// 										rtph264depay ! h264parse ! avdec_h264 ! queue !\
	// 										x264enc bitrate=1000000 bframes=0 key-int-max=10 weightb=false speed-preset=1 cabac=false tune=zerolatency !\
	// 										appsink name=sink";

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

bool pull_frame()
{
	if (!appsink)
		return false;

	GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(appsink));

	if (sample == NULL)
		return false;

	GstBuffer *buffer = gst_sample_get_buffer(sample);
	GstMapInfo map;
	gst_buffer_map(buffer, &map, GST_MAP_READ);

	memmove(writeBuffer, map.data, map.size);
	writeLength = map.size;

	gst_buffer_unmap(buffer, &map);
	gst_sample_unref(sample);

	return true;
}

void *stream_thread(void *ptr)
{
	gst_init(nullptr, nullptr);
	pthread_t gst = run_thread(gst_thread);

	while (1)
	{
		if (pull_frame())
			write();
	}

	pthread_join(gst, nullptr);
}

#endif