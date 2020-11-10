#ifndef __SYSTEM_H_
#define __SYSTEM_H_

	int system_cmd(char *cmd, char *out);
	char * system_get_model();
	int system_get_mac(char *mac, int size);

#endif
