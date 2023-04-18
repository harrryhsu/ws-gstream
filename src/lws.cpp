#include "lws.h"

WebSocket *WebSocket::Current = nullptr;

int WebSocket::callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	struct session_data *data = (struct session_data *)user;
	auto ws = WebSocket::Current;

	switch (reason)
	{
	case LWS_CALLBACK_ESTABLISHED:
		data->wsi = wsi;
		data->id = ws->id++;
		char path[50];
		lws_hdr_copy(wsi, path, 50, WSI_TOKEN_GET_URI);
		data->path = path;

		for (int i = 0; i < MAX_CONNECTION; i++)
		{
			if (!ws->sockets[i])
			{
				ws->sockets[i] = data;
				cout << "Connected: " << path << endl;
				return 0;
			}
		}
		return -1; // Terminate connection

	case LWS_CALLBACK_CLOSED:
		printf("Disconnected\n");
		for (int i = 0; i < MAX_CONNECTION; i++)
		{
			if (ws->sockets[i] && ws->sockets[i]->id == data->id)
				ws->sockets[i] = nullptr;
		}
		break;

	case LWS_CALLBACK_SERVER_WRITEABLE:
		if (!data->ready)
			break;

		auto config = ws->getConfig(data->path);
		if (!config)
		{
			return -1;
		}

		lws_write(wsi, config->writeBuffer, config->writeLength, LWS_WRITE_BINARY);
		data->ready = false;
	}

	return 0;
}

WebSocket *WebSocket::Singleton(int port)
{
	if (!WebSocket::Current)
		WebSocket::Current = new WebSocket(port);
	return WebSocket::Current;
}

WebSocket::WebSocket(int port)
{
	this->port = port;

	this->protocols[0].name = "ws";
	this->protocols[0].callback = callback;
	this->protocols[0].per_session_data_size = sizeof(struct session_data);
	this->protocols[0].rx_buffer_size = MAX_PAYLOAD_SIZE;

	this->protocols[1].name = NULL;
	this->protocols[1].callback = NULL;
	this->protocols[1].per_session_data_size = 0;

	for (int i = 0; i < MAX_CONNECTION; i++)
	{
		this->sockets[i] = nullptr;
	}
}

WebSocket::~WebSocket()
{
	delete this->thread;
	for (auto const &stream : this->streams)
	{
		delete stream.second;
	}
	this->streams.clear();
}

void WebSocket::addConfig(string path)
{
	stream_config *stream = new stream_config();
	stream->path = path;
	this->streams[path] = stream;
}

stream_config *WebSocket::getConfig(string path)
{
	return this->streams[path];
}

void WebSocket::write(string path)
{
	for (int i = 0; i < MAX_CONNECTION; i++)
	{
		if (this->sockets[i] && this->sockets[i]->path.compare(path) == 0)
		{
			this->sockets[i]->ready = true;
		}
	}

	lws_callback_on_writable_all_protocol(this->context, &this->protocols[0]);
}

void WebSocket::lws_thread()
{
	struct lws_context_creation_info ctx_info = {0};
	ctx_info.port = this->port;
	ctx_info.iface = NULL;
	ctx_info.protocols = this->protocols;
	ctx_info.gid = -1;
	ctx_info.uid = -1;
	ctx_info.options = LWS_SERVER_OPTION_VALIDATE_UTF8;

	this->context = lws_create_context(&ctx_info);

	lws_context_default_loop_run_destroy(context);

	printf("Websocket server exiting...\n");
}

void WebSocket::start()
{
	// this->stop();
	std::function<void(void)> lws = std::bind(&WebSocket::lws_thread, this);
	this->thread = new Thread(lws);
	this->thread->start();
}

void WebSocket::stop()
{
	if (this->thread)
	{
		this->thread->kill();
		delete this->thread;
	}
}

void WebSocket::wait()
{
	this->thread->wait();
}