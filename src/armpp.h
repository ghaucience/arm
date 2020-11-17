#ifndef __ARMPP_H_
#define __ARMPP_H_

#ifdef __cplusplus
extern "C" {
#endif


int armpp_init();
int armpp_handle_msg(char *from, char *modelstr, char *_type, char *mac, char *attr, int ep, char *value);
int armpp_sync_add_device(char *from, char *mac, char *modelstr,  char *type);
int armpp_sync_del_device(char *from, char *mac);

int armpp_add_sence(char *name, int init_enable);
int armpp_del_sence(int idx);
int armpp_lst_sence();
int armpp_eab_sence(int idx);
int armpp_dab_sence(int idx);
int armpp_clr_sence();

int armpp_lst_device();
int armpp_eab_device(char *mac);
int armpp_dab_device(char *mac);
int armpp_grp_device(char *mac, int sence_idx);
int armpp_trg_device(char *mac, int trig_idx);
int armpp_act_device(char *mac, int action_idx);

#ifdef __cplusplus
}
#endif
#endif
