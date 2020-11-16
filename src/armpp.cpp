#include <string>
#include <iostream>
#include <fstream>
#include "json.h"

#include "armpp.h"

using namespace std;

#define armpp_log_debug			log_debug
#define armpp_log_warn			log_warn
#define armpp_log_info			log_info
#define armpp_log_err				log_err
#define armpp_log_debug_hex	log_debug_hex


static Json::Value root;

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

#if 0
{
        "devices" : 
        [
                {
                        "action_idx" : 1,
                        "enable" : 1,
                        "mac" : "0102030405060708",
                        "modelstr" : "Mose",
                        "sence_idx" : 1,
                        "trig_idx" : 1,
												"action_idx" : 1,
                        "type" : "1212"
                }
        ],
        "devnames" : 
        [
                {
                        "actions" : 
                        [
                                {
                                        "attr" : "device.light.onoff",
                                        "value" : "1",
																				"idx": 1
                                }
                        ],
                        "conds" : 
                        [
                                {
                                        "attr" : "device.onoff",
                                        "value" : "0",
																				"idx": 1
                                }
                        ],
                        "modelstr" : "Mose",
                        "name" : "Motion Sensor",
                        "type" : "1212"
                }
        ],
        "sences" : 
        [
                {
                        "enable" : 1,
                        "idx" : 1,
                        "name" : "KitchAlarm"
                }
        ]
}
#endif

int armpp_init() {
	int ret = armpp_read_file(root, "/root/test.json");
	if (ret != 0) {
		cout << "load file filed:" << ret << endl;
		return -1;
	}
	cout << root << endl;
	return 0;
}

Json::Value armpp_get_device(string mac) {
	for (Json::Value::iterator it=root["devices"].begin(); it!=root["devices"].end(); ++it) {
		Json::Value d = *it;
		if (d["mac"].asString().compare(mac) != 0) {
			continue;
		}
		return d;
	}
	return 0;
}
Json::Value armpp_get_group(int idx) {
	for (Json::Value::iterator it=root["sences"].begin(); it!=root["sences"].end(); ++it) {
		Json::Value s = *it;
		if (s["idx"].asInt() != idx) {
			continue;
		}
		return s;
	}	
	return 0;	
}
Json::Value armpp_get_dev_cond(string modelstr, string type, int trig_idx) {
	for (Json::Value::iterator it=root["devnames"].begin(); it!=root["devnames"].end(); ++it) {
		Json::Value dn = *it;
		if (dn["modelstr"].asString().compare(modelstr) != 0) {
			continue;
		}
		if (dn["type"].asString().compare(type) != 0) {
			continue;
		}
		for (Json::Value::iterator it=dn["conds"].begin(); it!=dn["conds"].end(); ++it) {
			Json::Value c = *it;
			if (c["idx"].asInt() != trig_idx) {
				continue;
			}
			return c;
		}
	}
	return 0;
}
Json::Value armpp_get_dev_action(string modelstr, string type, int action_idx) {
	for (Json::Value::iterator it=root["devnames"].begin(); it!=root["devnames"].end(); ++it) {
		Json::Value dn = *it;
		if (dn["modelstr"].asString().compare(modelstr) != 0) {
			continue;
		}
		if (dn["type"].asString().compare(type) != 0) {
			continue;
		}
		for (Json::Value::iterator it=dn["actions"].begin(); it!=dn["actions"].end(); ++it) {
			Json::Value a = *it;
			if (a["idx"].asInt() != action_idx) {
				continue;
			}
			return a;
		}
	}
	return 0;
}
int armpp_handler_msg(char *modelstr, char *type, char *mac, char *attr, int ep, char *value) {
	Json::Value devices		= root["devices"];
	Json::Value devnames	= root["devnames"];

	Json::Value dev = armpp_get_device(mac);
	if (dev.isNull()) {
		cout << "dev :" << mac << " not exsit!" << endl;
		return 0;
	}

	if (dev["enable"].asInt() <= 0) {
		cout << "dev :" << mac << " disabled!" << endl;
		return 0;
	}
	
	if (dev["sence_idx"].asInt() <= 0) {
		cout << "dev :" << mac << " no sence" << endl;
		return 0;
	}
	
	Json::Value sence = armpp_get_group(dev["sence_idx"].asInt());
	if (sence.isNull()) {
		cout << "dev :" << mac << "'s sence not exsit" << endl;
		return 0;
	}
	if (sence["enable"].asInt() <= 0) {
		cout << "dev :" << mac << "'s sence disabled!" << endl;
		return 0;
	}
	

	Json::Value cond = armpp_get_dev_cond(modelstr, type, dev["trig_idx"].asInt());
	if (cond.isNull()) {
		cout << "dev :" << mac << "'s cond null!" << endl;
		return 0;
	}
	
	if (cond["attr"].asString().compare(attr) != 0) {
		cout << "dev :" << mac << "'s not care attr!" << endl;
		return 0;
	}

	if (cond["value"].asString().compare(value) != 0) {
		cout << "dev :" << mac << "'s not care value!" << endl;
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
		Json::Value action = armpp_get_dev_action(d["modelstr"].asString(), d["type"].asString(), d["action_idx"].asInt());

		if (action.isNull()) {
			continue;
		}
		

		#if 0
			//uproto_call(const char *_mac, const char *attr, const char *operation, void *jvalue, int timeout, const char *uuid);
			uproto_call(d["mac"].asString().c_str(), 
								action["attr"].asString().c_str(), 
								"setAttribute",
								action["value"].asString().c_str(),
								0,
								"X_");
		#endif
	}

	return 0;
}
