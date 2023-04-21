#include "stream.h"

Stream::Stream(WebSocket *lws, string path, string url)
{
	this->lws = lws;
	this->path = path;
	this->url = url;
	lws->addConfig(path);
}

Stream::~Stream()
{
	if (this->thread)
		delete this->thread;
}

void Stream::setSize(int width, int height)
{
	this->width = width;
	this->height = height;
}

void Stream::gst_thread()
{
	guint bus_watch_id;

	this->loop = g_main_loop_new(NULL, FALSE);

	ostringstream ss;
	ss << "rtspsrc location="
		 << this->url
		 << " name=src latency=0 buffer-mode=none is-live=true ! decodebin ! videoscale ! video/x-raw,width="
		 << this->width << ",height=" << this->height << " ! "
		 << (this->h265 ? "x265enc" : "x264enc") << " bitrate=1000 bframes=0 key-int-max=30 weightb=false speed-preset=ultrafast cabac=false tune=zerolatency ! "
		 << "appsink name=sink";

	string pipelineStr = ss.str();

	this->pipeline = gst_parse_launch(pipelineStr.c_str(), nullptr);
	this->appsink = gst_bin_get_by_name((GstBin *)this->pipeline, "sink");

	/* we add a message handler */
	this->bus = gst_pipeline_get_bus(GST_PIPELINE(this->pipeline));
	bus_watch_id = gst_bus_add_watch(this->bus, this->bus_call, this->loop);
	gst_object_unref(this->bus);
	this->bus = nullptr;

	/* Set the pipeline to "playing" state*/
	g_print("Now set pipeline in state playing\n");
	gst_element_set_state(this->pipeline, GST_STATE_PLAYING);
	this->gst_loop_started = true;

	/* Iterate */
	g_print("Running...\n");
	g_main_loop_run(this->loop);

	/* Out of the main loop, clean up nicely */
	g_print("Returned, stopping playback\n");
	gst_element_set_state(this->pipeline, GST_STATE_NULL);

	g_print("Deleting pipeline\n");
	gst_object_unref(GST_OBJECT(this->pipeline));
	this->pipeline = nullptr;

	g_source_remove(bus_watch_id);
	g_main_loop_unref(this->loop);
	this->loop = nullptr;
	this->appsink = nullptr;
}

gboolean Stream::bus_call(GstBus *bus, GstMessage *msg, gpointer data)
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

bool Stream::pull_frame()
{
	if (!this->appsink)
		return false;

	GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(this->appsink));

	if (sample == NULL)
		return false;

	GstBuffer *buffer = gst_sample_get_buffer(sample);
	GstMapInfo map;
	gst_buffer_map(buffer, &map, GST_MAP_READ);

	auto config = this->lws->getConfig(this->path);
	if (config->first)
	{
		memmove(config->writeBufferFirst, map.data, map.size);
		config->writeLengthFirst = map.size;
		config->first = false;
	}
	else
	{
		memmove(config->writeBuffer, map.data, map.size);
		config->writeLength = map.size;
	}
	lws->write(this->path);

	gst_buffer_unmap(buffer, &map);
	gst_sample_unref(sample);

	return true;
}

void Stream::stream_thread()
{
	this->started = true;
	gst_init(nullptr, nullptr);

	std::function<void(void)> gstf = std::bind(&Stream::gst_thread, this);
	Thread gst(gstf);
	gst.start();

	while (this->started)
	{
		if (this->gst_loop_started)
			pull_frame();
	}

	gst.wait();
}

void Stream::start()
{
	// this->stop();
	this->thread = new Thread(std::bind(&Stream::stream_thread, this));
	this->thread->start();
}

void Stream::stop()
{
	this->started = false;
	if (this->thread)
	{
		this->thread->kill();
		delete this->thread;
	}
	if (this->loop)
		g_main_loop_quit(this->loop);
}

void Stream::wait()
{
	this->thread->wait();
}

void Stream::useH265()
{
	this->h265 = true;
}
