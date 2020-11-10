

class ArmDeviceType {
	private:
		strcut stArmDeviceNameMapItem_t {
			char *modelstr;
			char *type;	
			char *name;
		};
		struct stArmDeviceConditionMapItem_t {
			char *name;
			int		idx;	
			char *value;
		};
	
		static map<<string,string>, string>	devs;
		static map<<string,idx>, string> conds;
	public:
		int   load();

		char *get_name(char *modelstr, char *type);

		

		get_trigger_conditions(char *name, int *cnt);
		char * get_trigger_condition_by_idx(char *name, int idx);
		char * get_trigger_condition_value(char *name, int idx);
};

class ArmDevice {
	private:
		char	modelstr[32];
		char	type[8];
		char	mac[8];
		char	wireless[16];

		char	trig_idx;
		
		int		enable;
		int		sence_idx;
};

class ArmSence {
	private:
		char	name[64];
		int		enable;
		char	action_json_str[256];
		int		idx;
	
	private:
		int		execnt;
};
