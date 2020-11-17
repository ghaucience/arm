#ifndef __ARMPP_H_
#define __ARMPP_H_

#ifdef __cplusplus
extern "C" {
#endif


int armpp_init();
int armpp_handle_msg(char *from, char *modelstr, char *_type, char *mac, char *attr, int ep, char *value);
int armpp_sync_add_device(char *from, char *mac, char *modelstr,  char *type);
int armpp_sync_del_device(char *from, char *mac);

#ifdef __cplusplus
}
#endif
#endif
