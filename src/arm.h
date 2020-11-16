#ifndef _ARM_H_
#define _AMM_H_

#include <stdio.h>
#include <stdlib.h>
#include "jansson.h"

typedef struct stArmDeviceNameItem {
	char	*modelstr;
	char	*type;
	char	*name;
} stArmDeviceNameItem_t;


typedef struct stArmDeviceConditionItem {
	char	*name;
	int		idx;
	char	*value;
} stArmDeviceConditionItem_t;

typedef struct stArmDevice {
	char	modelstr[32];
	char	type[8];
	char	mac[8];
	int		trig_idx;
	int		enable;
	int		sence_idx;
} stArmDevice_t;

typedef struct stArmSence {
	char	name[32];
	int		enable;
	char	action_json_string[256];
	int		idx;
} stArmSence_t;



char *arm_get_name(char *_modelstr, char *_type) ;
json_t* arm_get_trigger_conditions(char *name, int *cnt) ;
json_t *arm_get_trigger_condition_by_idx(char *_name, int idx) ;
json_t *arm_get_action_by_idx(char *name, int idx);

int arm_dev_foreach(int (*cb)(char *modelstr, char *type, char *mac, int trig_idx, int enable, int sence_idx, void *arg), void *arg);
int arm_sch_device(char *_mac) ;
int arm_add_device(char *mac, char *modelstr, char *type, int enable, int trig_idx, int sence_idx) ;
int arm_del_device(char *mac) ;
int arm_eab_device(char *mac) ;
int arm_grp_device(char *mac, int idx) ;
int arm_get_device(char *mac, char *modelstr, char *type, int *enable, int *trig_idx, int *sence_idx) ;

int arm_sce_foreach(int (*cb)(char *name, int enable, int idx, char *action) );
int arm_sch_sence(char *_name) ;
int arm_add_sence(char *name, int idx, int enable, char *action) ;
int arm_del_sence(char *name) ;
int arm_eab_sence(char *name) ;
int arm_dab_sence(char *name) ;
int arm_clr_sence() ;

int arm_load() ;
int arm_save() ;
int arm_init(const char *afile) ;

int arm_handler_msg(int type, char *modelstr, char *_type, char *mac, char *attr, int ep, char *value);

/*
{
	"devnames": [
		{"modelstr": "Mose", "type":"1212", "name":"Montion Sensor", "devconds":[{"attr":"devce.onoff", "value":"0"}, {"attr":"device.light.onoff", "value":1"}], "actions":[{"attr":"device.off", "value":2}]},
	],
	"devices": [
		{"modelstr": "Mose", "type":"1212", "mac":"0102030405060708", "trig_idx":1, "action_idx": 1, "enable":1, "sence_idx":1},
	],
	"sences":	[
		{"name":"KitchAlarm", "enable":1, "idx":1}
	],
}
*/
#endif


