#ifndef LWS
#define LWS

#include "common.cpp"
#include "libwebsockets.h"

#define MAX_PAYLOAD_SIZE 10 * 1024
#define MAX_CONNECTION 10
#define WRITE_BUFFER_SIZE 1000000

struct session_data
{
	int id;
	lws *wsi;
	bool ready;
};

struct lws_context *context;
static int id = 0;
static session_data *sockets[MAX_CONNECTION];
unsigned char writeBuffer[WRITE_BUFFER_SIZE];
int writeLength;

static int callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	struct session_data *data = (struct session_data *)user;

	switch (reason)
	{
	case LWS_CALLBACK_ESTABLISHED:
		printf("Connected\n");
		data->wsi = wsi;
		data->id = id++;
		for (int i = 0; i < MAX_CONNECTION; i++)
		{
			if (!sockets[i])
			{
				sockets[i] = data;
				break;
			}
		}
		// TODO Disconnect
		break;

	case LWS_CALLBACK_CLOSED:
		printf("Disconnected\n");
		for (int i = 0; i < MAX_CONNECTION; i++)
		{
			if (sockets[i] && sockets[i]->id == data->id)
				sockets[i] = nullptr;
		}
		break;

	case LWS_CALLBACK_SERVER_WRITEABLE:
		if (!data->ready)
			break;
		lws_write(wsi, writeBuffer, writeLength, LWS_WRITE_BINARY);
		data->ready = false;
	}

	return 0;
}

struct lws_protocols protocols[] = {
		{
				"ws",
				callback,
				sizeof(struct session_data),
				MAX_PAYLOAD_SIZE,
		},
		{NULL, NULL, 0}};

void write()
{
	for (int i = 0; i < MAX_CONNECTION; i++)
	{
		if (sockets[i])
			sockets[i]->ready = true;
	}
	lws_callback_on_writable_all_protocol(context, &protocols[0]);
}

void *lws_thread(void *ptr)
{
	struct lws_context_creation_info ctx_info = {0};
	ctx_info.port = 8080;
	ctx_info.iface = NULL;
	ctx_info.protocols = protocols;
	ctx_info.gid = -1;
	ctx_info.uid = -1;
	ctx_info.options = LWS_SERVER_OPTION_VALIDATE_UTF8;
	context = lws_create_context(&ctx_info);

	lws_context_default_loop_run_destroy(context);

	printf("Websocket server exiting...\n");

	return nullptr;
}

#endif
