#include <string>
#include <iostream>
#include <fstream>
#include "json.h"
#include "simlog.h"
#include "jansson.h"
#include "uproto.h"

#include "armpp.h"

#include "system.h"
#include "schedule.h"

using namespace std;

#define armpp_log_debug			log_debug
#define armpp_log_warn			log_warn
#define armpp_log_info			log_info
#define armpp_log_err				log_err
#define armpp_log_debug_hex	log_debug_hex


static Json::Value root;
static string root_file = "/tmp/test.json";
static stSchduleTask_t qry_list_task;

static int armpp_write_file(Json::Value &root, string file) {
#if 1
	Json::StyledWriter sw;

	ofstream os;
	os.open(file);
	os << sw.write(root);
	os.close();
#else
	Json::StreamWriterBuilder builder;
	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	ofstream os(file);
	writer->write(root, &os);
#endif
	return 0;
}

static int armpp_read_file(Json::Value &root, string file) {
#if 1
	Json::Reader reader;

	ifstream in(file, ios::binary);
	if (!in.is_open()) {
		return -1;
	}
	if (!reader.parse(in, root)) {
		return -2;
	}
	in.close();
#else
	Json::CharReaderBulider readerBuilder;
	std::unique_ptr<Json::CharReader> const reader(readerBuilder.newCharReader());
	ifstream if(file, ios::binary);
	if (in.is_open()) {
		return -1;
	}
	bool res = reader->parse(

#endif
	return 0;
}

static int armpp_make_test_json() {
	/*
	Json::Value r;

	r["devnames"] = Json::Value("[]");
	r["devnames"][0]["modelstr"] = "Mose"
	r["devnames"][0]["type"]		 = "Mose"

	*/
	return 0;
}

static int armpp_generate_device_idx() {
	int asize		= root["devices"].size();

	for (int i  = 1 ; i <= asize; i++) {
		int find = 0;

		for (Json::Value::iterator it=root["devices"].begin(); it!=root["devices"].end(); ++it) {
			Json::Value d = *it;
			if (d["idx"].asInt() == i) {
				find = 1;
				break;
			}
		}
		if (find == 1) {
			continue;
		}
		return i;
	}

	return asize + 1;
}
static Json::Value armpp_get_device(string mac) {
	for (Json::Value::iterator it=root["devices"].begin(); it!=root["devices"].end(); ++it) {
		Json::Value d = *it;
		if (d["mac"].asString().compare(mac) != 0) {
			continue;
		}
		return d;
	}
	return Json::Value();
}
static Json::Value armpp_get_group(int idx) {
	for (Json::Value::iterator it=root["sences"].begin(); it!=root["sences"].end(); ++it) {
		Json::Value s = *it;
		if (s["idx"].asInt() != idx) {
			continue;
		}
		return s;
	}	
	return Json::Value();
}
static Json::Value armpp_get_dev_cond(int trig_idx) {
	for (Json::Value::iterator it=root["conds"].begin(); it!=root["conds"].end(); ++it) {
		Json::Value c = *it;
		if (c["idx"].asInt() != trig_idx) {
			continue;
		}
		return c;
	}
	return Json::Value();
}
static Json::Value armpp_get_dev_action(int action_idx) {
	for (Json::Value::iterator it=root["actions"].begin(); it!=root["actions"].end(); ++it) {
		Json::Value a = *it;
		if (a["idx"].asInt() != action_idx) {
			continue;
		}
		return a;
	}
	return Json::Value();
}


static void *qry_list_func(void *arg) {
	system("dusun_ucmd.sh list");
	schedue_add(&qry_list_task, 1000*60*10, (void*)qry_list_func, 0);
}

int armpp_init() {
	int ret = armpp_read_file(root, root_file);
	if (ret != 0) {
		cout << "load file filed:" << ret << endl;
		return -1;
	}
	cout << root << endl;

	schedue_add(&qry_list_task, 2000, (void*)qry_list_func, 0);
	return 0;
}


int armpp_handle_msg(char *from, char *modelstr, char *type, char *mac, char *attr, int ep, char *value) {
	log_info("%s", __func__);

	Json::Value dev = armpp_get_device(mac);
	if (dev.isNull()) {
		log_warn("dev:%s not exsit", mac);
		return 0;
	}

	if (dev["enable"].asInt() <= 0) {
		log_warn("dev:%s disabeld", mac);
		return 0;
	}
	
	if (dev["sence_idx"].asInt() <= 0) {
		log_warn("dev:%s no sence", mac);
		return 0;
	}
	
	Json::Value sence = armpp_get_group(dev["sence_idx"].asInt());
	if (sence.isNull()) {
		log_warn("dev:%s no sence", mac);
		return 0;
	}
	if (sence["enable"].asInt() <= 0) {
		log_warn("dev:%s sence disabled", mac);
		return 0;
	}
	

	Json::Value cond = armpp_get_dev_cond(dev["trig_idx"].asInt());
	if (cond.isNull()) {
		log_warn("dev:%s cond null", mac);
		return 0;
	}
	
	if (cond["attr"].asString().compare(attr) != 0) {
		log_warn("dev:%s not care attr", mac);
		return 0;
	}

	if (cond["value"].asString().compare(value) != 0) {
		log_warn("dev:%s not care attr value", mac);
		return 0;
	}


	for (Json::Value::iterator it=root["devices"].begin(); it!=root["devices"].end(); ++it) {
		Json::Value d = *it;
		if (d["enable"].asInt() <= 0) {
			continue;
		}
		if (d["sence_idx"].asInt() != sence["idx"].asInt()) {
			continue;
		}
		if (d["action_idx"].asInt() <= 0) {
			continue;
		}
		Json::Value action = armpp_get_dev_action(d["action_idx"].asInt());
		Json::FastWriter wr;
		//log_debug("Execute Action:%s, value:%s for dev:%s", action["attr"].asString().c_str(), action["value"].toStyledString().c_str(), d["mac"].asString().c_str());
		log_debug("Execute Action:%s, value:%s for dev:%s", action["attr"].asString().c_str(), wr.write(action["value"]).c_str(), d["mac"].asString().c_str());

		char uuid[64];
		sprintf(uuid, "%d", rand()%1000000);

		json_error_t error;
		json_t *jvalue = json_loads(wr.write(action["value"]).c_str(), 0, &error);
		if (jvalue == NULL) {
			log_warn("error action value");
			continue;
		}

		uproto_call("CLOUD", "GREENPOWER",
								d["mac"].asString().c_str(), 
								action["attr"].asString().c_str(), 
								"setAttribute",
								jvalue,
								0,
								uuid);
	}

	char uuid[64];
	sprintf(uuid, "%d", rand()%1000000);

	json_error_t error;
	Json::FastWriter wr;
	json_t *jvalue = json_loads(wr.write(sence).c_str(), 0, &error);
	if (jvalue == NULL) {
		return 0;
	}
	char gwmac[32];
	system_get_mac(gwmac, sizeof(gwmac));
	uproto_call("ARM", "GATEWAY",
			gwmac,
			"arm.sence.triggered", 
			"reportAttribute",
			jvalue,
			0,
			uuid);
	return 0;
}

int armpp_sync_add_device(char *from, char *mac, char *modelstr,  char *type) {
	armpp_log_info(" ");

	if (string(from).compare("GREENPOWER") == 0) {
		from = "Zigbee";
	} else if (string(from).compare("ZWAVE") == 0) {
		from = "Z-WAVE";
	} else {
		armpp_log_warn("not support source!");
		return 0;
	}

	Json::Value dev = armpp_get_device(mac);
	if (!dev.isNull()) {
		int upflag = 0;
		if (modelstr != NULL && (dev["modelstr"].isNull() || dev["modelstr"].compare(modelstr) != 0)){
			upflag = 1;
			dev["modelstr"] = modelstr;
		}
		if (type != NULL && (dev["type"].isNull() || dev["type"].compare(type) != 0)) {
			upflag = 1;
			dev["type"] = type;
		}
		if (from != NULL && (dev["source"].isNull() || dev["source"].compare(from) != 0)) {
			upflag = 1;
			dev["source"] = from;
		}
		if (upflag) {
			cout << root << endl;
			armpp_write_file(root, root_file);
		}
		return 0;
	}
	//int idx			= armpp_generate_device_idx();
	int asize		= root["devices"].size();

	//root["devices"][asize] = Json::Value('{}');
	root["devices"][asize]["action_idx"] = 0;
	root["devices"][asize]["enable"]			= 0;
	root["devices"][asize]["mac"]					= mac;
	root["devices"][asize]["modelstr"]		= modelstr;
	root["devices"][asize]["sence_idx"]		= 0;
	root["devices"][asize]["trig_idx"]		= 0;
	root["devices"][asize]["type"]				= type;
	root["devices"][asize]["source"]			= from;

	cout << root << endl;
	armpp_write_file(root, root_file);
	return 0;
}

int armpp_sync_del_device(char *from, char *mac) {
	armpp_log_info(" ");
	Json::Value dev = armpp_get_device(mac);
	if (dev.isNull()) {
		return 0;
	}
	
	int idx = 0;
	for (Json::Value::iterator it=root["devices"].begin(); it!=root["devices"].end(); ++it) {
		Json::Value d = *it;
		if (d["mac"].asString().compare(mac) != 0) {
			++idx;
			continue;
		}
		break;
	}

	root["devices"][idx] = Json::Value();
	armpp_write_file(root, root_file);
	
	return 0;
}
