#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system.h"
#include <unistd.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
#include<sys/ioctl.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<net/if.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <regex.h>
#include <time.h>

#include "schedule.h"

int system_cmd(char *cmd, char *out) {
	FILE *fp = popen(cmd, "r");
	if (fp == NULL) {
		out[0] = 0;
		return -1;
	}

	char buf[256];
	if (fgets(buf, sizeof(buf) - 1, fp) == NULL) {
		pclose(fp);
		return -2;
	}
	pclose(fp);

	int len = strlen(buf);
	if (buf[len-1] == 0x0A) {
		buf[len-1] = 0;
		len--;
	} 

	strcpy(out, buf);
	return 0;
}


int system_get_mac(char *mac, int size) {
	static char __mac[32] = {0};
	if (__mac[0] == 0) {
		char *cmd = "cat /sys/class/net/eth0/address";
		char buf[256];
		system_cmd(cmd, buf);
		strcpy(__mac, buf);
	} 
	strcpy(mac, __mac);

	return 0;
}

char * system_get_model() {
	static char model[128] = {0};
	if (model[0] != 0) {
		return model;
	}

	char *cmd = "cat /tmp/sysinfo/model";
	char buf[256];
	int ret = system_cmd(cmd, buf);
	if (ret != 0) {
		model[0] = 0;
		return model;
	}
	strcpy(model, buf);

	return model;
}



