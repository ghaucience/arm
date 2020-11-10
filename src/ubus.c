#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include <libubox/blobmsg_json.h>
#include <libubox/avl.h>
#include <libubus.h>
#include <jansson.h>

#include "ubus.h"
#include "simlog.h"
#include "json_parser.h"
#include "uproto.h"


#define ubus_log_info				log_info
#define ubus_log_warn				log_warn
#define ubus_log_err				log_err
#define ubus_log_debug			log_debug
#define ubus_log_debug_hex	log_debug_hex


static stUbus_t ue = {0};

static void ubus_receive_event(struct ubus_context *ctx,struct ubus_event_handler *ev,  const char *type,struct blob_attr *msg);


int ubus_init() {
	ue.ubus_ctx = ubus_connect(NULL);
	memset(&ue.listener, 0, sizeof(ue.listener));
	ue.listener.cb = ubus_receive_event;
	ubus_register_event_handler(ue.ubus_ctx, &ue.listener, LISTEN_PAT);//DS.BUL

	pthread_mutex_init(&ue.mutex, NULL);

	return 0;
}

void ubus_send(const char *dstpat, const char *smsg) {
	pthread_mutex_lock(&ue.mutex);

	blob_buf_init(&ue.b, 0);
	blobmsg_add_string(&ue.b, "PKT", smsg);
	ubus_send_event(ue.ubus_ctx, dstpat, ue.b.head);

	pthread_mutex_unlock(&ue.mutex);
}

static void ubus_receive_event(struct ubus_context *ctx,struct ubus_event_handler *ev, 
	const char *type,struct blob_attr *msg) {
	char *str;

	ubus_log_info("-----------------[ubus msg]: handler ....-----------------");
#if 0
	str = blobmsg_format_json(msg, true);
	if (str != NULL) {
		ubus_log_info("[ubus msg]: [%s]", str);

		json_error_t error;
		json_t *jmsg = json_loads(str, 0, &error);
		if (jmsg != NULL) {
			const char *spkt = json_get_string(jmsg, "PKT");
			if (spkt != NULL) {
				ubus_log_info("pks : %s", spkt);
				uproto_handler_ubus_event_general(spkt);
			} else {
				ubus_log_warn("not find 'PKT' item!");
			}
			json_decref(jmsg);
		} else {
			ubus_log_warn("error: on line %d: %s", error.line, error.text);
		}
		free(str);
	} else {
		ubus_log_warn("[ubus msg]: []");
	}
#else
	log_debug("-----------------[ubus msg]: handler ....-----------------\n");
	str = blobmsg_format_json(msg, true);
	if (str != NULL) {
		log_debug("[ubus msg]: [%s]\n", str);

		json_error_t error;
		json_t *jmsg = json_loads(str, 0, &error);
		if (jmsg != NULL) {
			int freeflag = 0;

			const char *spkt = json_get_string(jmsg, "PKT");
			if (spkt == NULL) {
				json_t *jpkt = json_object_get(jmsg, "PKT");
				if (jpkt != NULL) {
					spkt = json_dumps(jpkt, 0);
					if (spkt != NULL) {
						freeflag = 1;
					}
				}
			}

			if (spkt != NULL) {
				log_debug("pks : %s\n", spkt);
				uproto_handler_ubus_event_general(spkt);
			} else {
				log_debug("not find 'PKT' item!\n");
			}

			if (freeflag) {
				free((char *)spkt);
			}
			json_decref(jmsg);
		} else {
			log_debug("error: on line %d: %s\n", error.line, error.text);
		}
		free(str);
	} else {
		log_debug("[ubus msg]: []\n");
	}

#endif
	ubus_log_info("-----------------[ubus msg]: handler over-----------------");
}

int ubus_get_fd() {
	if (!ue.ubus_ctx || ue.ubus_ctx->sock.fd <= 0) {
		return -1;
	}

	return ue.ubus_ctx->sock.fd;
}

void uproto_tick() {
#if 0
	struct timeval tv;
	int maxfd;
	fd_set fds;

	tv.tv_sec = 0;
	tv.tv_usec  = 8000;

	FD_ZERO(&fds);
	if (!ue.ubus_ctx || ue.ubus_ctx->sock.fd <= 0) {
		return;
	}
	FD_SET(ue.ubus_ctx->sock.fd, &fds);

	maxfd = ue.ubus_ctx->sock.fd;

	int ret = select(maxfd + 1, &fds, NULL, NULL, &tv);

	if (ret <= 0) {
		return; /*nothing to do */
	}

	if (!FD_ISSET(ue.ubus_ctx->sock.fd, &fds)) {
		return; /* what happend , maybe ubus stop and restart */
	}
#endif
	ubus_handle_event(ue.ubus_ctx);
}


