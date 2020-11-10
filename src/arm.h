#ifndef _ARM_H_
#define _AMM_H_


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


char *arm_get_name(char *modelstr, char *type);
char **arm_get_trigger_conditions(char *name, int *cnt);
char *arm_get_trigger_condition_by_idx(char *name, int idx);
char *arm_get_trigger_condition_value(char *name, int idx);


typedef struct stArmDevice {
	char	modelstr[32];
	char	type[8];
	char	mac[8];
	int		trig_idx;
	int		enable;
	int		sence_idx;
} stArmDevice_t;

int arm_load_device();
int arm_add_device(char *mac, char *modelstr, char *type);
int arm_del_device(char *mac);
int arm_eab_device(char *mac);
int arm_dab_device(char *mac);
int arm_grp_device(char *mac, int idx);
int arm_dev_foreach(int (*cb)(char *modelstr, char *type, char *mac, int trig_idx, int enable, int sence_idx));

typedef struct stArmSence {
	char	name[32];
	int		enable;
	char	action_json_string[256];
	int		idx;
} stArmSence_t;


int arm_add_sence(char *name)
int arm_del_sence(char *name)
int arm_eab_sence(char *name)
int arm_dab_sence(char *name)
int arm_clr_sence(char *name)
int arm_sce_foreach(int (*cb)(char *name, int enable, int idx, char *action);


#endif


