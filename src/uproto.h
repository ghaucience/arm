#ifndef __UPROTO_H_
#define __UPROTO_H_

#ifdef __cplusplus
extern "C" {
#endif








int uproto_handler_ubus_event_general(const char *msg);
int uproto_call(const char *src, const char *dst, const char *mac, const char *attr, const char *operation, void *jvalue, int timeout, const char *uuid);





#ifdef __cplusplus
}
#endif
#endif
