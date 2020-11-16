#ifndef __ARMPP_H_
#define __ARMPP_H_

#ifdef __cplusplus
extern "C" {
#endif


int armpp_init();
int armpp_handle_msg(char *modelstr, char *_type, char *mac, char *attr, int ep, char *value);

#ifdef __cplusplus
}
#endif
#endif
