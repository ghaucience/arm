#ifndef UBUS_H_
#define UBUS_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <libubox/blobmsg_json.h>
#include <libubox/avl.h>
#include <libubus.h>
#include <jansson.h>

#define LISTEN_PAT "DS.GATEWAY"
#define SEND_PAT "DS.GREENPOWER"


typedef struct stUbus {
        struct ubus_context *ubus_ctx;
        struct ubus_event_handler listener;
        struct blob_buf b;

        pthread_mutex_t mutex;
} stUbus_t;



int  ubus_init();
void uproto_tick();
int  ubus_get_fd();

void ubus_send(const char *dstpat, const char *smsg);



#endif /* UBUS_H_ */

