#include <stdio.h>
#include <stdlib.h>

#include "ubus.h"
#include "uproto.h"
#include "simlog.h"
#include "schedule.h"

#include "jansson.h"
#include "json_parser.h"
#include "system.h"
#include "util.h"

#define uproto_log_warn				log_warn
#define uproto_log_err				log_err
#define uproto_log_info				log_info
#define uproto_log_debug			log_debug
#define uproto_log_debug_hex	log_debug_hex



typedef int (*UHANDLER)(const char *from, const char *uuid, const char *cmdmac, const char *attr, json_t *value); 
typedef struct stUHandler {
	const char *from;
	const char *to;
	const char *type;
	const char *attr;
	UHANDLER		func;
} stUHandler_t;
#define DEF_UHANDLER(x) static int x(const char *from, const char *uuid, const char *cmdmac, const char *attr, json_t*value)

DEF_UHANDLER(set_add_sence);
DEF_UHANDLER(set_del_sence);
DEF_UHANDLER(set_lst_sence);
DEF_UHANDLER(set_eab_sence);
DEF_UHANDLER(set_dab_sence);
DEF_UHANDLER(set_clr_sence);

DEF_UHANDLER(set_lst_device);
DEF_UHANDLER(set_eab_device);
DEF_UHANDLER(set_dab_device);
DEF_UHANDLER(set_grp_device);

static stUHandler_t uhs[] = {
	{"CLOUD",	"ARM",	"setAttribute",	"arm.add_sence",						set_add_sence},
	{"CLOUD",	"ARM",	"setAttribute",	"arm.del_sence",						set_del_sence},
	{"CLOUD",	"ARM",	"setAttribute",	"arm.lst_sence",						set_lst_sence},
	{"CLOUD",	"ARM",	"setAttribute",	"arm.eab_sence",						set_eab_sence},
	{"CLOUD",	"ARM",	"setAttribute",	"arm.dab_sence",						set_dab_sence},
	{"CLOUD",	"ARM",	"setAttribute",	"arm.clr_sence",						set_clr_sence},

	{"CLOUD",	"ARM",	"setAttribute",	"arm.lst_device",						set_lst_device},
	{"CLOUD",	"ARM",	"setAttribute",	"arm.eab_device",						set_eab_device},
	{"CLOUD",	"ARM",	"setAttribute",	"arm.dab_device",						set_dab_device},
	{"CLOUD",	"ARM",	"setAttribute",	"arm.grp_device",						set_grp_device},

	{"GREENPOWR", "GATEWAY", "reportAttribute", "...",						NULL},
	{"ZWAVE",			"GATEWAY", "reportAttribute", "...",						NULL},

};

static int _uproto_handler_cmd(const char *from, 
		const char *to,
		const char *ctype,
		const char *mac, 
		int dtime, 
		const char *uuid,
		const char *command,	
		const char *cmdmac,
		const char *attr,
		json_t *value) {
	uproto_log_info("from:%s, to:%s, ctype:%s, mac:%s, dtime:%d, uuid:%s, command:%s, cmdmac:%s, attr:%s, value:%p",
			from, to, ctype, mac, dtime, uuid, command, cmdmac, attr, value);

	int cnt = sizeof(uhs)/sizeof(uhs[0]);
	int i = 0;

	stUHandler_t *_uh = NULL;
	for (i = 0; i < cnt; i++) {
		stUHandler_t *uh = &uhs[i];
		uproto_log_debug("from:%s, to:%s, type:%s, attr:%s, func:%p", uh->from, uh->to, uh->type, uh->attr, uh->func);
		if (strcmp(uh->from, from) != 0) {
			uproto_log_warn("from not same");
			continue;
		}
		if (strcmp(uh->to, to) != 0) {
			uproto_log_warn("to not same");
			continue;
		}
	#if 0
		if (strcmp(uh->type, ctype) != 0) {
			uproto_log_warn("type not same");
			continue;
		}
	#else
		if (strcmp(uh->type, command) != 0) {
			uproto_log_warn("type not same");
			continue;
		}
	#endif
		if (uh->attr != NULL) {
			if (strcmp(uh->attr, attr) != 0) {
				uproto_log_warn("attr not same");
				continue;
			}
		} else {
			;
		}
		_uh = uh;
		break;
	}
	if (_uh == NULL) {
		uproto_log_warn("not support from:%s, to:%s, type:%s, attr:%s", from, to, ctype, attr);
		return -1;
	}
	uproto_log_info("handler from:%s, to:%s, type:%s, attr:%s, %p", from, to, ctype, attr, _uh->func);
	return _uh->func(from, uuid, cmdmac, attr, value);
}


int uproto_handler_ubus_event_general(const char *msg) {
	uproto_log_info("-");

	json_error_t error;
	json_t *jpkt = json_loads(msg, 0, &error);
	if (jpkt == NULL) {
		uproto_log_warn("error: on line %d: %s", error.line, error.text);
		return -1;
	}


	/* registerReq, registerRsp, reportAttribute, reportAttributeResp cmd cmdResult */
	const char *from	= json_get_string(jpkt, "from");
	const char *to		= json_get_string(jpkt, "to");
	const char *ctype = json_get_string(jpkt, "type");
	const char *mac		=	json_get_string(jpkt, "mac");
	int   dtime		= 0;	json_get_int(jpkt, "time", &dtime);

	json_t	*jdata = json_object_get(jpkt, "data");
	if (jdata == NULL) {
		uproto_log_warn("not find data item!");
		json_decref(jpkt);
		return -2;
	}

	if (strcmp(ctype, "cmd") != 0) {
		uproto_log_warn("not support command type:%s", ctype);
		json_decref(jpkt);
		return -3;
	}

	const char *command = json_get_string(jdata, "command");
	if (command == NULL) {
		uproto_log_warn("no command seg!");
		json_decref(jpkt);
		return -4;
	}

	const char *id			= json_get_string(jdata, "id");
	if (id == NULL) {
		uproto_log_warn("no id seg!");
		json_decref(jpkt);
		return -5;
	}

	json_t		 *jarg		= json_object_get(jdata, "arguments");
	if (jarg == NULL) {
		uproto_log_warn("no arguments seg!");
		json_decref(jpkt);
		return -5;
	}

	const char *attribute = json_get_string(jarg, "attribute");
	if (attribute == NULL) {
		uproto_log_warn("no attribute !");
		json_decref(jpkt);
		return -6;
	}

	const char *cmdmac = json_get_string(jarg, "mac");
	if (cmdmac == NULL) {
		uproto_log_warn("no mac !");
		json_decref(jpkt);
		return -7;
	}

	json_t *jval = json_object_get(jarg, "value");
	if (jval == NULL) {
		uproto_log_warn("no value !");
		json_decref(jpkt);
		return -8;
	}

	uproto_log_info("from:%s,to:%s,type:%s,time:%d,uuid:%s,cmdmac:%s, attr:%s", from, to, ctype, dtime, id, cmdmac, attribute);
	_uproto_handler_cmd(from, to, ctype, mac, dtime, id, command, cmdmac, attribute, jval);

	if (jpkt != NULL) {
		json_decref(jpkt);
		jpkt = NULL;
	}
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
[recv]:
{
	"data": {
		"arguments": {
			"attribute": "device.lock.del_password",
			"ep": 1,
			"value": {
				"passType": "0",
				"passId": 490
			},
			"mac": "00158d00026c5444"
		},
		"id": "e5a9e4cb-981a-4d63-8415-4a661709d802",
		"command": "setAttribute"
	},
	"deviceCode": "010006a3-05eb-4879-800158d00026c5444",
	"from": "CLOUD",
	"mac": "30:ae:7b:2b:ff:81",
	"messageId": 3761,
	"time": 1554182691,
	"to": "NXP",
	"type": "cmd"
}
[send]:
{
	"from": "GREENPOWER",
	"to": "CLOUD",
	"type": "reportAttribute",
	"deviceCode": "010006a3-05eb-4879-819e-ad3501579fbc",
	"time": 1554182710,
	"mac": "30:ae:7b:2b:ff:81",
	"data": {
		"mac": "00158d00026c5444",
		"attribute": "device.lock.password_adddel_result",
		"ep": 1,
		"value": {
			"ep": 1,
			"operation": "del",
			"passId": 490,
			"code": 0,
			"time": 1554182510,
			"ModelStr": "unknow"
		}
	}
}
#define DEF_UHANDLER(x) static int x(const char *from, const char *uuid, const char *cmdmac, const char *attr, json_t*value)
_uproto_handler_cmd(from, to, ctype, mac, dtime, id, command, cmdmac, attribute, jval);
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
/**> scan  */
DEF_UHANDLER(set_add_sence) {
	uproto_log_info(" ");
	return 0;
}
DEF_UHANDLER(set_del_sence) {
	uproto_log_info(" ");
	return 0;
}
DEF_UHANDLER(set_lst_sence) {
	uproto_log_info(" ");
	return 0;
}
DEF_UHANDLER(set_eab_sence) {
	uproto_log_info(" ");
	return 0;
}
DEF_UHANDLER(set_dab_sence) {
	uproto_log_info(" ");
	return 0;
}
DEF_UHANDLER(set_clr_sence) {
	uproto_log_info(" ");
	return 0;
}

DEF_UHANDLER(set_lst_device) {
	uproto_log_info(" ");
	return 0;
}
DEF_UHANDLER(set_eab_device) {
	uproto_log_info(" ");
	return 0;
}
DEF_UHANDLER(set_dab_device) {
	uproto_log_info(" ");
	return 0;
}
DEF_UHANDLER(set_grp_device) {
	uproto_log_info(" ");
	return 0;
}


