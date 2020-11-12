#include "arm.h"
#include "jansson.h"
#include "json_parser.h"
#include "simlog.h"

#define arm_log_debug			log_debug
#define arm_log_warn			log_warn
#define arm_log_err				log_err
#define arm_log_info			log_info
#define arm_log_debug_hex	log_debug_hex

static char		arm_file[128] = "/etc/config/dusun/arm.json";
static json_t	*arm_root = NULL;

char *arm_get_name(char *_modelstr, char *_type) {
	json_t *dns = json_object_get(arm_root, "devnames");
	if (dns == NULL || !json_is_array(dns)) {
		return NULL;
	}
	size_t	i;
	json_t	*v = NULL;
	json_array_foreach(dns, i, v) {
		const char *modelstr		= json_get_string(v, "modelstr");
		const char *type				= json_get_string(v, "type");
		const char *name				= json_get_string(v, "name");
		json_t		 *devconds		= json_object_get(v, "devconds");

		if (modelstr == NULL || type == NULL || name == NULL || devconds == NULL) {
			continue;
		}
		if (strcmp(modelstr, _modelstr) != 0) {
			continue;
		}
		if (strcmp(type, _type) != 0) {
			continue;
		}
		return (char *)name;
	}
	return NULL;
}
json_t *arm_get_trigger_conditions(char *_name, int *cnt) {
	json_t *dns = json_object_get(arm_root, "devnames");
	if (dns == NULL || !json_is_array(dns)) {
		return NULL;
	}
	size_t	i;
	json_t	*v = NULL;
	json_array_foreach(dns, i, v) {
		const char *modelstr		= json_get_string(v, "modelstr");
		const char *type				= json_get_string(v, "type");
		const char *name				= json_get_string(v, "name");
		json_t		 *devconds		= json_object_get(v, "devconds");

		if (modelstr == NULL || type == NULL || name == NULL || devconds == NULL) {
			continue;
		}
		if (strcmp(name, _name) != 0) {
			continue;
		}
		if (!json_is_array(devconds)) {
			continue;
		}
		return devconds;
	}
	return NULL;
}
json_t *arm_get_trigger_condition_by_idx(char *_name, int idx) {
	json_t *dns = json_object_get(arm_root, "devnames");
	if (dns == NULL || !json_is_array(dns)) {
		return NULL;
	}
	size_t	i;
	json_t	*v = NULL;
	json_array_foreach(dns, i, v) {
		const char *modelstr		= json_get_string(v, "modelstr");
		const char *type				= json_get_string(v, "type");
		const char *name				= json_get_string(v, "name");
		json_t		 *devconds		= json_object_get(v, "devconds");

		if (modelstr == NULL || type == NULL || name == NULL || devconds == NULL) {
			continue;
		}
		if (strcmp(name, _name) != 0) {
			continue;
		}
		if (!json_is_array(devconds)) {
			continue;
		}
		return json_array_get(devconds, idx);
	}
	return NULL;
}

int arm_dev_foreach(int (*cb)(char *modelstr, char *type, char *mac, int trig_idx, int enable, int sence_idx)) {
	json_t *devs = json_object_get(arm_root, "devices");
	if (devs == NULL || !json_is_array(devs)) {
		return -1;
	}
	size_t	i;
	json_t	*v = NULL;
	json_array_foreach(devs, i, v) {
		const char *modelstr		= json_get_string(v, "modelstr");
		const char *type				= json_get_string(v, "type");
		const char *mac 				= json_get_string(v, "mac");
		int		trig_idx = -1; 		  json_get_int(v, "trig_idx", &trig_idx);
		int		enable = -1;			  json_get_int(v, "enable", &enable);
		int		sence_idx	= -1;			json_get_int(v, "sence_idx", &sence_idx);

		if (modelstr == NULL || type == NULL || mac == NULL || trig_idx < 0 || enable < 0 || sence_idx < 0) {
			continue;
		}
		if (cb == NULL) {
			continue;
		}
		if (cb != NULL) {
			cb((char *)modelstr, (char *)type, (char *)mac, trig_idx, enable, sence_idx);
		}
	}
	return 0;
}
int arm_sch_device(char *_mac) {
	json_t *devs = json_object_get(arm_root, "devs");
	if (devs == NULL || !json_is_array(devs)) {
		return -1;
	}
	size_t	i;
	json_t	*v = NULL;
	json_array_foreach(devs, i, v) {
		const char *modelstr		= json_get_string(v, "modelstr");
		const char *type				= json_get_string(v, "type");
		const char *mac 				= json_get_string(v, "mac");
		int		trig_idx = -1; 		  json_get_int(v, "trig_idx", &trig_idx);
		int		enable = -1;			  json_get_int(v, "enable", &enable);
		int		sence_idx	= -1;			json_get_int(v, "sence_idx", &sence_idx);

		if (modelstr == NULL || type == NULL || mac == NULL || trig_idx < 0 || enable < 0 || sence_idx < 0) {
			continue;
		}
	
		if (strcmp(mac, _mac) != 0) {
			continue;
		}
		return i;
	}
	return -1;
}
int arm_add_device(char *mac, char *modelstr, char *type, int enable, int trig_idx, int sence_idx) {
	if (arm_sch_device(mac) >= 0) {
		return -1;
	}
	
	json_t *devs = json_object_get(arm_root, "devs");
	if (devs == NULL || !json_is_array(devs)) {
		return -2;
	}

	json_t *dev = json_object();
	json_object_set_new(dev, "mac",				json_string(mac));
	json_object_set_new(dev, "modelstr",	json_string(modelstr));
	json_object_set_new(dev, "type",			json_string(type));
	json_object_set_new(dev, "trig_idx",  json_integer(trig_idx));
	json_object_set_new(dev, "enable",		json_integer(enable));
	json_object_set_new(dev, "sence_idx",				json_integer(sence_idx));

	json_array_append_new(devs, dev);

	return 0;

}
int arm_del_device(char *mac) {
	int idx = arm_sch_device(mac);
	if (idx < 0) {
		return -1;
	}

	json_t *devs = json_object_get(arm_root, "devs");
	if (devs == NULL || !json_is_array(devs)) {
		return -2;
	}

	json_array_remove(devs, idx);

	return 0;
}
int arm_eab_device(char *mac) {
	int idx = arm_sch_device(mac);
	if (idx < 0) {
		return -1;
	}

	json_t *devs = json_object_get(arm_root, "devs");
	if (devs == NULL || !json_is_array(devs)) {
		return -2;
	}

	json_t *dev = json_array_get(devs, idx);
	if (dev == NULL) {
		return -3;
	}
	json_object_del(dev, "enable");
	json_object_set_new(dev, "enable", json_integer(1));
	return 0;

}
int arm_dab_device(char *mac) {
	int idx = arm_sch_device(mac);
	if (idx < 0) {
		return -1;
	}

	json_t *devs = json_object_get(arm_root, "devs");
	if (devs == NULL || !json_is_array(devs)) {
		return -2;
	}

	json_t *dev = json_array_get(devs, idx);
	if (dev == NULL) {
		return -3;
	}
	json_object_del(dev, "enable");
	json_object_set_new(dev, "enable", json_integer(0));
	return 0;
}
int arm_grp_device(char *mac, int _idx) {
	int idx = arm_sch_device(mac);
	if (idx < 0) {
		return -1;
	}

	json_t *devs = json_object_get(arm_root, "devs");
	if (devs == NULL || !json_is_array(devs)) {
		return -2;
	}

	json_t *dev = json_array_get(devs, idx);
	if (dev == NULL) {
		return -3;
	}
	json_object_del(dev, "trig_idx");
	json_object_set_new(dev, "trig_idx", json_integer(_idx));
	return 0;
}

int arm_sce_foreach(int (*cb)(char *name, int enable, int idx, char *action)) {
	json_t *sences = json_object_get(arm_root, "sences");
	if (sences == NULL || !json_is_array(sences)) {
		return -1;
	}
	size_t	i;
	json_t	*v = NULL;
	json_array_foreach(sences, i, v) {
		const char *name		= json_get_string(v, "name");
		int		enable = -1;		json_get_int(v, "enable", &enable);
		int		idx		 = -1;		json_get_int(v, "idx", &idx);
		json_t	*action			= json_object_get(v, "action");
		if (name == NULL || enable < 0 || idx < 0 || action == NULL) {
			continue;
		}
		if (cb == NULL) {
			continue;
		}
		const char *saction = json_dumps(action, 0);
		if (saction != NULL) {
			cb((char *)name, enable, idx, (char *)action);
			free((char *)saction);
		}
	}
	return 0;
}
int arm_sch_sence(char *_name) {
	json_t *sences = json_object_get(arm_root, "sences");
	if (sences == NULL || !json_is_array(sences)) {
		return -1;
	}
	size_t	i;
	json_t	*v = NULL;
	json_array_foreach(sences, i, v) {
		const char *name		= json_get_string(v, "name");
		int		enable = -1;		json_get_int(v, "enable", &enable);
		int		idx		 = -1;		json_get_int(v, "idx", &idx);
		json_t	*action			= json_object_get(v, "action");
		if (name == NULL || enable < 0 || idx < 0 || action == NULL) {
			continue;
		}
		if (strcmp(name, _name) != 0) {
			continue;
		}
		return i;
	}
	return -1;
}
int arm_add_sence(char *name, int idx, int enable, char *action) {
	if (arm_sch_sence(name) >= 0) {
		return -1;
	}
	
	json_t *sences = json_object_get(arm_root, "sences");
	if (sences == NULL || !json_is_array(sences)) {
		return -2;
	}

	json_error_t error;
	json_t *jaction = json_loads(action, 0,  &error);
	if (jaction == NULL) {
		return -3;
	}

	json_t *sence = json_object();
	json_object_set_new(sence, "name",		json_string(name));
	json_object_set_new(sence, "enable",	json_integer(enable));
	json_object_set_new(sence, "idx",			json_integer(idx));
	json_object_set_new(sence, "action",  jaction);
	
	json_array_append_new(sences, sence);

	return 0;
}
int arm_del_sence(char *name) {
	int idx = arm_sch_sence(name);
	if (idx < 0) {
		return -1;
	}

	json_t *sences = json_object_get(arm_root, "sences");
	if (sences == NULL || !json_is_array(sences)) {
		return -2;
	}

	json_array_remove(sences, idx);

	return 0;
}
int arm_eab_sence(char *name) {
	int idx = arm_sch_sence(name);
	if (idx < 0) {
		return -1;
	}

	json_t *sences = json_object_get(arm_root, "sences");
	if (sences == NULL || !json_is_array(sences)) {
		return -2;
	}

	json_t *sence = json_array_get(sences, idx);
	if (sence == NULL) {
		return -3;
	}
	json_object_del(sence, "enable");
	json_object_set_new(sence, "enable", json_integer(1));
	return 0;
}
int arm_dab_sence(char *name) {
	int idx = arm_sch_sence(name);
	if (idx < 0) {
		return -1;
	}

	json_t *sences = json_object_get(arm_root, "sences");
	if (sences == NULL || !json_is_array(sences)) {
		return -2;
	}

	json_t *sence = json_array_get(sences, idx);
	if (sence == NULL) {
		return -3;
	}
	json_object_del(sence, "enable");
	json_object_set_new(sence, "enable", json_integer(0));
	return 0;
}
int arm_clr_sence() {
	json_t *sences = json_object_get(arm_root, "sences");
	if (sences == NULL || !json_is_array(sences)) {
		return -2;
	}
	json_array_clear(sences);
	return 0;
}

int arm_load() {
	json_t *root;
	json_error_t error;
	root = json_load_file(arm_file, 0, &error);
	if(root == NULL) {
		arm_log_warn("load %s failed!", arm_file);
		return -1;
	}
	arm_root = root;
	return 0;
}
int arm_save() {
	return json_dump_file(arm_root, arm_file, 1);
}

int arm_init(const char *afile) {
	strcpy(arm_file, afile);
	
	return arm_load();
}

int arm_handler_msg(int type, char *msg) {
	return 0;
}
