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
static string root_file = "/etc/config/dusun/alarm/alarm.json";
static stSchduleTask_t qry_list_task;


/**
 * Init Read  /  Write
 * ============================================================================= */
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

/**
 * Actrue Devices
 * ============================================================================= */
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
static Json::Value armpp_get_device(string mac, int &ix) {
	ix = 0;
	for (Json::Value::iterator it=root["devices"].begin(); it!=root["devices"].end(); ++it) {
		Json::Value d = *it;
		if (d["mac"].asString().compare(mac) != 0) {
			ix++;
			continue;
		}
		return d;
	}
	return Json::Value();
}

/**
 * Actrue Devices
 * ============================================================================= */
static int armpp_generate_vdevice_idx() {
	int asize		= root["sendevs"].size();

	for (int i  = 1 ; i <= asize; i++) {
		int find = 0;

		for (Json::Value::iterator it=root["sendevs"].begin(); it!=root["sendevs"].end(); ++it) {
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
static Json::Value armpp_get_vdevice(int idx, int &ix) {
	ix = 0;
	for (Json::Value::iterator it=root["sendevs"].begin(); it!=root["sendevs"].end(); ++it) {
		Json::Value d = *it;
		if (d["idx"].asInt() != idx) {
			ix++;
			continue;
		}
		return d;
	}
	return Json::Value();
}


/**
 * Sences
 * ============================================================================= */
static Json::Value armpp_get_group(int idx, int &ix) {
	ix = 0;
	for (Json::Value::iterator it=root["sences"].begin(); it!=root["sences"].end(); ++it) {
		Json::Value s = *it;
		if (s["idx"].asInt() != idx) {
			ix++;
			continue;
		}
		return s;
	}	
	return Json::Value();
}

static Json::Value armpp_get_group_by_name(char *name, int &ix) {
	ix = 0;
	for (Json::Value::iterator it=root["sences"].begin(); it!=root["sences"].end(); ++it) {
		Json::Value s = *it;
		if (s["name"].asString().compare(name) != 0) {
			ix++;
			continue;
		}
		return s;
	}	
	return Json::Value();
}

static int armpp_generate_group_idx() {
	int asize		= root["sences"].size();

	for (int i  = 1 ; i <= asize; i++) {
		int find = 0;

		for (Json::Value::iterator it=root["sences"].begin(); it!=root["sences"].end(); ++it) {
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

/**
 * Conditions
 * ============================================================================= */
static Json::Value armpp_get_dev_cond(int trig_idx, int &ix) {
	ix = 0;
	for (Json::Value::iterator it=root["conds"].begin(); it!=root["conds"].end(); ++it) {
		Json::Value c = *it;
		if (c["idx"].asInt() != trig_idx) {
			ix++;
			continue;
		}
		return c;
	}
	return Json::Value();
}

/**
 * Actions
 * ============================================================================= */
static Json::Value armpp_get_dev_action(int action_idx, int &ix) {
	ix = 0;
	for (Json::Value::iterator it=root["actions"].begin(); it!=root["actions"].end(); ++it) {
		Json::Value a = *it;
		if (a["idx"].asInt() != action_idx) {
			ix++;
			continue;
		}
		return a;
	}
	return Json::Value();
}



/**
 * Body &  Api
 * ============================================================================= */
static void qry_list_func(void *arg) {
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

static map<string, map<unsigned int, int>> armpp_delay_action_map_list;
void armpp_delay_action_func(void *arg) {
	Json::Value *delay_item = (Json::Value*)arg;
	stSchduleTask_t *task = (stSchduleTask_t*)(*delay_item)["task"].asInt();

	/*
	delay_item["sen_mac"] = mac;
	delay_item["attr"]		= attr;
	delay_item["value"]		= value;
	delay_item["zone"]		= zone;
	delay_item["act_mac"] = d["mac"].asString();
	delay_item["act_idx"] = d["action_idx"].asInt();
	*/

	char uuid[64];
	sprintf(uuid, "%d", rand()%1000000);

	int ix;
	Json::Value action = armpp_get_dev_action((*delay_item)["act_idx"].asInt(), ix);
	Json::FastWriter wr;

	json_error_t error;
	json_t *jvalue = json_loads(wr.write(action["value"]).c_str(), 0, &error);
	if (jvalue == NULL) {
		log_warn("error action value");
		armpp_delay_action_map_list[(*delay_item)["attr"].asString()].erase((unsigned int)task);
		delete delay_item;
		delete task;
		return;
	}

	log_info("real do action from %s, value:%s", (*delay_item)["sen_mac"].asString().c_str(), (*delay_item)["value"].asString().c_str());
	log_debug("Execute Action:%s, value:%s for dev:%s", action["attr"].asString().c_str(), wr.write(action["value"]).c_str(), (*delay_item)["act_mac"].asString().c_str());

	uproto_call("CLOUD", "GREENPOWER",
			(*delay_item)["act_mac"].asString().c_str(), 
			action["attr"].asString().c_str(), 
			"setAttribute",
			jvalue,
			0,
			uuid);

	armpp_delay_action_map_list[(*delay_item)["attr"].asString()].erase((unsigned int)task);
	delete delay_item;
	delete task;
}

int armpp_handle_msg(char *from, char *modelstr, char *type, char *mac, char *attr, int ep, char *value, char *zone) {
	log_info("%s", __func__);

#if 0
	int ix = 0;
	Json::Value dev = armpp_get_device(mac, ix);
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
	
	Json::Value sence = armpp_get_group(dev["sence_idx"].asInt(), ix);
	if (sence.isNull()) {
		log_warn("dev:%s no sence", mac);
		return 0;
	}
	if (sence["enable"].asInt() <= 0) {
		log_warn("dev:%s sence disabled", mac);
		return 0;
	}
	

	Json::Value cond = armpp_get_dev_cond(dev["trig_idx"].asInt(), ix);
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
	if (!cond["zone"].isNull()) {
			if (zone == NULL || cond["zone"].asString().compare(zone) != 0) {
				log_warn("dev:%s not care zone value", mac);
				return 0;
			}
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
		Json::Value action = armpp_get_dev_action(d["action_idx"].asInt(), ix);
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
#else
	
	for (map<unsigned int, int>::iterator iti = armpp_delay_action_map_list[attr].begin();
			iti != armpp_delay_action_map_list[attr].end();) {
		map<unsigned int, int>::iterator it = iti;
		iti++;

		stSchduleTask_t *st = (stSchduleTask_t*)it->first;
		Json::Value *delay_item = (Json::Value*)st->arg;
		if ( (*delay_item)["sen_mac"].asString().compare(mac) != 0) {
			it = iti;
			continue;
		}
		if ( (*delay_item)["value"].asString().compare(value) == 0) {
			it = iti;
			continue;
		}
		
		log_info("delete trigger's action from %s, value:%s", (*delay_item)["sen_mac"].asString().c_str(), (*delay_item)["value"].asString().c_str());
		schedue_del(st);
		delete delay_item;
		delete st;
		armpp_delay_action_map_list[attr].erase(it);
		it = iti;
	}

	int ix = 0;
	map<int,int> exe_stack;
	for (Json::Value::iterator it=root["sendevs"].begin(); it!=root["sendevs"].end(); ++it) {
		Json::Value dev = *it;
		if (dev.isNull()) {
			log_warn("dev:%s not exsit", mac);
			continue;
		}

		if (dev["enable"].asInt() <= 0) {
			log_warn("dev:%s disabeld", mac);
			continue;
		}

		if (dev["sence_idx"].asInt() <= 0) {
			log_warn("dev:%s no sence", mac);
			continue;
		}

		Json::Value sence = armpp_get_group(dev["sence_idx"].asInt(), ix);
		if (sence.isNull()) {
			log_warn("dev:%s no sence", mac);
			continue;
		}
		if (sence["enable"].asInt() <= 0) {
			log_warn("dev:%s sence disabled", mac);
			continue;
		}


		Json::Value cond = armpp_get_dev_cond(dev["trig_idx"].asInt(), ix);
		if (cond.isNull()) {
			log_warn("dev:%s cond null", mac);
			continue;
		}

		if (cond["attr"].asString().compare(attr) != 0) {
			log_warn("dev:%s not care attr", mac);
			continue;
		}

		if (cond["value"].asString().compare(value) != 0) {
			log_warn("dev:%s not care attr value", mac);
			continue;
		}
		if (!cond["zone"].isNull()) {
			if (zone == NULL || cond["zone"].asString().compare(zone) != 0) {
				log_warn("dev:%s not care zone value", mac);
				continue;
			}
		}

		if (exe_stack.find(dev["sence_idx"].asInt()) != exe_stack.end()) {
			log_debug("Ignore Executed Sence_idx:%d", dev["sence_idx"].asInt());
			continue;
		}
		exe_stack[dev["sence_idx"].asInt()] = 1;

		for (Json::Value::iterator it=root["sendevs"].begin(); it!=root["sendevs"].end(); ++it) {
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

			Json::Value action = armpp_get_dev_action(d["action_idx"].asInt(), ix);
			Json::FastWriter wr;
			//log_debug("Execute Action:%s, value:%s for dev:%s", action["attr"].asString().c_str(), action["value"].toStyledString().c_str(), d["mac"].asString().c_str());
			log_debug("Execute Action:%s, value:%s for dev:%s", action["attr"].asString().c_str(), wr.write(action["value"]).c_str(), d["mac"].asString().c_str());

			if (!action["delay"].isNull() && action["delay"].asInt() > 0) { // delay
				Json::Value *delay_item = new Json::Value();
				(*delay_item)["sen_mac"] = mac;
				(*delay_item)["attr"]		= attr;
				(*delay_item)["value"]		= value;
				(*delay_item)["zone"]		= zone;
				(*delay_item)["act_mac"] = d["mac"].asString();
				(*delay_item)["act_idx"] = d["action_idx"].asInt();

				stSchduleTask_t *task = new stSchduleTask_t;
				(*delay_item)["task"] = (unsigned int)task;
				schedue_add(task, action["delay"].asInt() * 1000, (void *)armpp_delay_action_func, (void *)delay_item);

				// map<sen_mac, attr, value, act_mac, action>
				// map<attr, vector<stSchduleTask_t*>>
				// static map<string, list<stSchduleTask_t*>> armpp_delay_action_map_list;
				
				log_info("push action from %s, value:%s after %d to exec.", (*delay_item)["sen_mac"].asString().c_str(), (*delay_item)["value"].asString().c_str(), action["delay"].asInt());
				armpp_delay_action_map_list[attr][(unsigned int)task] = 0;
			} else {
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
	}
#endif
	return 0;
}


/**> Devies Api */
int armpp_sync_add_device(char *from, char *mac, char *modelstr,  char *type) {
	armpp_log_info(" ");

	if (string(from).compare("GREENPOWER") == 0) {
		from = (char *)"Zigbee";
	} else if (string(from).compare("ZWAVE") == 0) {
		from = (char *)"Z-WAVE";
	} else {
		armpp_log_warn("not support source!");
		return 0;
	}

	int ix;
	Json::Value dev = armpp_get_device(mac, ix);
	if (!dev.isNull()) {
		int upflag = 0;
		if (modelstr != NULL && (dev["modelstr"].isNull() || dev["modelstr"].compare(modelstr) != 0)){
			upflag = 1;
			root["devices"][ix]["modelstr"] = modelstr;
		}
		if (type != NULL && (dev["type"].isNull() || dev["type"].compare(type) != 0)) {
			upflag = 1;
			root["devices"][ix]["type"] = type;
		}
		if (from != NULL && (dev["source"].isNull() || dev["source"].compare(from) != 0)) {
			upflag = 1;
			root["devices"][ix]["source"] = from;
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
	int ix;
	Json::Value dev = armpp_get_device(mac, ix);
	if (dev.isNull()) {
		return 0;
	}
	
	//root["devices"][ix] = Json::Value();
	root["devices"].removeIndex(ix, NULL);
	armpp_write_file(root, root_file);
	
	return 0;
}


/**> Sence Api */
int armpp_add_sence(char *name, int init_enable) {
	armpp_log_info(" %s, %d", name, init_enable);
	int ix;
	Json::Value sence = armpp_get_group_by_name(name, ix);
	if (!sence.isNull()) {
		armpp_log_warn("has exsit sence!");
		return -1;
	}

	int idx		= armpp_generate_group_idx();
	int asize	= root["sences"].size();

	root["sences"][asize]["enable"] = (int)!!init_enable;
	root["sences"][asize]["idx"] = idx;
	root["sences"][asize]["name"] = name;

	armpp_write_file(root, root_file);
	return 0;
}
int armpp_del_sence(int idx) {
	armpp_log_info(" %d", idx);

	int ix;
	Json::Value sence = armpp_get_group(idx, ix);
	if (sence.isNull()) {
		armpp_log_warn("not exsit sence:%d", idx);
		return -1;
	}

	armpp_log_info("ix:%d", ix);
	//root["sences"][ix] = Json::Value();
	root["sences"].removeIndex(ix, NULL);
	
	ix = 0;
	for (Json::Value::iterator it=root["devices"].begin(); it!=root["devices"].end(); ++it, ++ix) {
		Json::Value d = *it;
		if (d["sence_idx"].asInt() != idx) {
			continue;
		}
		root["devices"][ix]["sence_idx"] = 0;
	}

	armpp_write_file(root, root_file);

	return 0;
}
int armpp_lst_sence() {
	armpp_log_info(" ");

	json_t *ja = json_array();

	for (Json::Value::iterator it=root["sences"].begin(); it!=root["sences"].end(); ++it) {
		Json::Value s = *it;
		
		json_t *ji = json_object();
		json_object_set_new(ji, "name",		json_string(s["name"].asString().c_str()));
		json_object_set_new(ji, "idx",		json_integer(s["idx"].asInt()));
		json_object_set_new(ji, "enable", json_integer(s["enable"].asInt()));
		json_array_append_new(ja, ji);
	}	

	return (int)ja;
}
int armpp_eab_sence(int idx) {
	armpp_log_info(" %d", idx);

	int ix;
	Json::Value sence = armpp_get_group(idx, ix);
	if (sence.isNull()) {
		armpp_log_warn("not exsit sence:%d", idx);
		return -1;
	}

	if (!sence["enable"].isNull() && sence["enable"].asInt() == 1) {
		armpp_log_warn("sence has enabled");
		return 0;
	}
	root["sences"][ix]["enable"] = 1;

	armpp_write_file(root, root_file);
	
	return 0;
}
int armpp_dab_sence(int idx) {
	armpp_log_info(" %d", idx);

	int ix;
	Json::Value sence = armpp_get_group(idx, ix);
	if (sence.isNull()) {
		armpp_log_warn("not exsit sence:%d", idx);
		return -1;
	}

	if (!sence["enable"].isNull() && sence["enable"].asInt() == 0) {
		armpp_log_warn("sence has disabled");
		return 0;
	}
	root["sences"][ix]["enable"] = 0;

	armpp_write_file(root, root_file);
	
	return 0;
}
int armpp_clr_sence() {
	armpp_log_info(" ");

#if 0
	int size = root["sences"].size();
	int i = 0;
	for (i = 0; i < size; i++) {
		//root["sences"][i] = Json::Value();
		root["sences"].removeIndex(i, NULL);
	}
#else
	root["sences"].clear();
#endif

	armpp_write_file(root, root_file);

	return 0;
}


/**> acture device api */
int armpp_lst_device() {
	armpp_log_info(" ");

	json_t *ja = json_array();

	for (Json::Value::iterator it=root["devices"].begin(); it!=root["devices"].end(); ++it) {
		Json::Value s = *it;
		
		json_t *ji = json_object();
		json_object_set_new(ji, "mac",				json_string(s["mac"].asString().c_str()));
		json_object_set_new(ji, "modelstr",		json_string(s["modelstr"].asString().c_str()));
		json_object_set_new(ji, "type",				json_string(s["type"].asString().c_str()));
		json_object_set_new(ji, "source",			json_string(s["source"].asString().c_str()));
		
		json_object_set_new(ji, "enable",			json_integer(s["enable"].asInt()));
		json_object_set_new(ji, "action_idx", json_integer(s["action_idx"].asInt()));
		json_object_set_new(ji, "trig_idx",		json_integer(s["trig_idx"].asInt()));
		json_object_set_new(ji, "sence_idx",	json_integer(s["sence_idx"].asInt()));

		json_array_append_new(ja, ji);
	}	

	return (int)ja;
}
int armpp_eab_device(char *mac) {
	armpp_log_info(" %s", mac);

	int ix;
	Json::Value dev = armpp_get_device(mac, ix);
	if (dev.isNull()) {
		armpp_log_warn("not exsit devcie:%s", mac);
		return -1;
	}
	if (!dev["enable"].isNull() && dev["enable"].asInt() == 1) {
		armpp_log_warn("device %s has enabled!", mac);
		return 0;
	}
	root["devices"][ix]["enable"] = 1;

	armpp_write_file(root, root_file);

	return 0;
}
int armpp_dab_device(char *mac) {
	armpp_log_info(" %s", mac);

	int ix;
	Json::Value dev = armpp_get_device(mac, ix);
	if (dev.isNull()) {
		armpp_log_warn("not exsit devcie:%s", mac);
		return -1;
	}
	if (!dev["enable"].isNull() && dev["enable"].asInt() == 0) {
		armpp_log_warn("device %s has disabled!", mac);
		return 0;
	}
	root["devices"][ix]["enable"] = 0;

	armpp_write_file(root, root_file);

	return 0;
}
int armpp_grp_device(char *mac, int sence_idx) {
	armpp_log_info(" %s %d", mac, sence_idx);

	int ix;
	Json::Value dev = armpp_get_device(mac, ix);
	if (dev.isNull()) {
		armpp_log_warn("not exsit devcie:%s", mac);
		return -1;
	}
	int iy;
	Json::Value sence = armpp_get_group(sence_idx, iy);
	if (sence.isNull() && sence_idx != 0) {
		armpp_log_warn("not exsit sence:%s", mac);
		return -2;
	}
	if (!dev["sence_idx"].isNull() && dev["sence_idx"].asInt() == sence_idx) {
		armpp_log_warn("device %s has in this group!", mac);
		return 0;
	}
	root["devices"][ix]["sence_idx"] = sence_idx;

	armpp_write_file(root, root_file);

	return 0;

}
int armpp_trg_device(char *mac, int trig_idx) {
	armpp_log_info(" %s %d", mac, trig_idx);

	int ix;
	Json::Value dev = armpp_get_device(mac, ix);
	if (dev.isNull()) {
		armpp_log_warn("not exsit devcie:%s", mac);
		return -1;
	}
	if (!dev["trig_idx"].isNull() && dev["trig_idx"].asInt() == trig_idx) {
		armpp_log_warn("device %s has in this trig_idx!", mac);
		return 0;
	}
	root["devices"][ix]["trig_idx"] = trig_idx;

	armpp_write_file(root, root_file);

	return 0;
}
int armpp_act_device(char *mac, int action_idx) {
	armpp_log_info(" %s %d", mac, action_idx);

	int ix;
	Json::Value dev = armpp_get_device(mac, ix);
	if (dev.isNull()) {
		armpp_log_warn("not exsit devcie:%s", mac);
		return -1;
	}
	if (!dev["action_idx"].isNull() && dev["action_idx"].asInt() == action_idx) {
		armpp_log_warn("device %s has in this act_idx!", mac);
		return 0;
	}
	root["devices"][ix]["action_idx"] = action_idx;

	armpp_write_file(root, root_file);

	return 0;
}


/**> vitual device api */
int armpp_lst_vdevice() {
	armpp_log_info(" ");

	json_t *ja = json_array();

	for (Json::Value::iterator it=root["sendevs"].begin(); it!=root["sendevs"].end(); ++it) {
		Json::Value s = *it;
		
		json_t *ji = json_object();
		json_object_set_new(ji, "mac",				json_string(s["mac"].asString().c_str()));
		json_object_set_new(ji, "modelstr",		json_string(s["modelstr"].asString().c_str()));
		json_object_set_new(ji, "type",				json_string(s["type"].asString().c_str()));
		json_object_set_new(ji, "source",			json_string(s["source"].asString().c_str()));
		
		json_object_set_new(ji, "enable",			json_integer(s["enable"].asInt()));
		json_object_set_new(ji, "action_idx", json_integer(s["action_idx"].asInt()));
		json_object_set_new(ji, "trig_idx",		json_integer(s["trig_idx"].asInt()));
		json_object_set_new(ji, "sence_idx",	json_integer(s["sence_idx"].asInt()));
		json_object_set_new(ji, "idx",				json_integer(s["idx"].asInt()));

		json_array_append_new(ja, ji);
	}	

	return (int)ja;
}

int armpp_add_vdevcie(char *mac) {
	armpp_log_info(" %s", mac);

	int ix;
	Json::Value dev = armpp_get_device(mac, ix);
	if (dev.isNull()) {
		armpp_log_warn("not exsit devcie:%s", mac);
		return -1;
	}

	ix = armpp_generate_vdevice_idx();
	int asize = root["sendevs"].size();
	root["sendevs"][asize] = dev;
	root["sendevs"][asize]["idx"] = ix;

	armpp_write_file(root, root_file);
	
	return 0;
}

int armpp_del_vdevice(int idx) {
	armpp_log_info(" %d", idx);

	int ix;
	Json::Value dev = armpp_get_vdevice(idx, ix);
	if (dev.isNull()) {
		armpp_log_warn("not exsit devcie:%d", idx);
		return -1;
	}

	root["sendevs"].removeIndex(ix, NULL);

	armpp_write_file(root, root_file);
	return 0;
}
int armpp_eab_vdevice(int idx) {
	armpp_log_info(" %d", idx);

	int ix;
	Json::Value dev = armpp_get_vdevice(idx, ix);
	if (dev.isNull()) {
		armpp_log_warn("not exsit devcie:%d", idx);
		return -1;
	}
	if (!dev["enable"].isNull() && dev["enable"].asInt() == 1) {
		armpp_log_warn("device %s has enabled!", dev["mac"].asString().c_str());
		return 0;
	}
	root["sendevs"][ix]["enable"] = 1;

	armpp_write_file(root, root_file);

	return 0;
}
int armpp_dab_vdevice(int idx) {
	armpp_log_info(" %d", idx);

	int ix;
	Json::Value dev = armpp_get_vdevice(idx, ix);
	if (dev.isNull()) {
		armpp_log_warn("not exsit devcie:%d", idx);
		return -1;
	}
	if (!dev["enable"].isNull() && dev["enable"].asInt() == 0) {
		armpp_log_warn("device %s has disabled!", dev["mac"].asString().c_str());
		return 0;
	}
	root["sendevs"][ix]["enable"] = 0;

	armpp_write_file(root, root_file);

	return 0;
}
int armpp_grp_vdevice(int idx, int sence_idx) {
	armpp_log_info(" %d %d",idx, sence_idx);

	int ix;
	Json::Value dev = armpp_get_vdevice(idx, ix);
	if (dev.isNull()) {
		armpp_log_warn("not exsit devcie:%d", idx);
		return -1;
	}
	int iy;
	Json::Value sence = armpp_get_group(sence_idx, iy);
	if (sence.isNull()  && sence_idx != 0) {
		armpp_log_warn("not exsit sence:%s", dev["mac"].asString().c_str());
		return -2;
	}
	if (!dev["sence_idx"].isNull() && dev["sence_idx"].asInt() == sence_idx) {
		armpp_log_warn("device %s has in this group!", dev["mac"].asString().c_str());
		return 0;
	}
	root["sendevs"][ix]["sence_idx"] = sence_idx;

	armpp_write_file(root, root_file);

	return 0;

}
int armpp_trg_vdevice(int idx, int trig_idx) {
	armpp_log_info(" %d %d", idx, trig_idx);

	int ix;
	Json::Value dev = armpp_get_vdevice(idx, ix);
	if (dev.isNull()) {
		armpp_log_warn("not exsit devcie:%d", idx);
		return -1;
	}
	if (!dev["trig_idx"].isNull() && dev["trig_idx"].asInt() == trig_idx) {
		armpp_log_warn("device %s has in this trig_idx!", dev["mac"].asString().c_str());
		return 0;
	}
	root["sendevs"][ix]["trig_idx"] = trig_idx;

	armpp_write_file(root, root_file);

	return 0;
}
int armpp_act_vdevice(int idx, int action_idx) {
	armpp_log_info(" %d %d", idx, action_idx);

	int ix;
	Json::Value dev = armpp_get_vdevice(idx, ix);
	if (dev.isNull()) {
		armpp_log_warn("not exsit devcie:%d", idx);
		return -1;
	}
	if (!dev["action_idx"].isNull() && dev["action_idx"].asInt() == action_idx) {
		armpp_log_warn("device %s has in this act_idx!", dev["mac"].asString().c_str());
		return 0;
	}
	root["sendevs"][ix]["action_idx"] = action_idx;

	armpp_write_file(root, root_file);

	return 0;
}


