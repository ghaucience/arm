#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"


static unsigned int char2hex(char x) {

	if (x >= '0' && x <= '9') {
		return (x - '0');
	}
	
	if (x >= 'A' && x <= 'F') {
		return (x - 'A' + 10);
	}

	if (x >= 'a' && x <= 'f') {
		return (x - 'a' + 10);
	}

	return 0;
}

char *util_mac2str(char *mac, char *str) {
	sprintf(str, "%02X%02X%02X%02X%02X%02X", 
		mac[0]&0xff,
		mac[1]&0xff,
		mac[2]&0xff,
		mac[3]&0xff,
		mac[4]&0xff,
		mac[5]&0xff);
	return str;
}

char *util_str2mac(char *str, char *mac) {
	mac[0] = char2hex(str[0]) * 16 + char2hex(str[1]);
	mac[1] = char2hex(str[2]) * 16 + char2hex(str[3]);
	mac[2] = char2hex(str[4]) * 16 + char2hex(str[5]);
	mac[3] = char2hex(str[6]) * 16 + char2hex(str[7]);
	mac[4] = char2hex(str[8]) * 16 + char2hex(str[9]);
	mac[5] = char2hex(str[10]) * 16 + char2hex(str[11]);
	return mac;
}
			
int util_hexstr2buf(char *str, char *buf) {
	int len  = strlen(str);
	int i = 0;
	for (i = 0; i < len/2; i++) {
		buf[i] = char2hex(str[i * 2 + 0]) * 16 + char2hex(str[i*2+1]);
	}
	return len/2;
}

char *util_buf2hexstr(char *buf, int len, char *str) {
	int i = 0;
	int xlen = 0;
	for (i = 0; i < len; i++) {
		xlen += sprintf(str + xlen, "%02X", buf[i]&0xff);
	}
	return str;
}

char *util_macswap(char *mac, char *buf) {
	buf[0] = mac[5];
	buf[1] = mac[4];
	buf[2] = mac[3];
	buf[3] = mac[2];
	buf[4] = mac[1];
	buf[5] = mac[0];
	return buf;
}

char *util_bufswap(char *buf, int len, char *out) {
	int i = 0;
	for (i = 0; i < len; i++) {
		out[i] = buf[len - i -1];
	}
	return out;
}

int		util_uuidstr2buf(char *uuid, char *buf) {
	char xbuf[128];
	int i = 0, j = 0;
	for (i = 0; i < strlen(uuid);  i++) {
		if (uuid[i] == '-') {
			continue;
		}
		xbuf[j++] = uuid[i];
	}
	xbuf[j] = 0;

	//return util_hexstr2buf(uuid, buf);
	return util_hexstr2buf(xbuf, buf);
}
char *util_buf2uuidstr(char *uuid, int ulen, char *buf) {
	if (ulen == 2) {
		//sprintf(buf, "%02X%02X", uuid[1]&0xff, uuid[0]&0xff);
		sprintf(buf, "%02X%02X", uuid[0]&0xff, uuid[1]&0xff);
	} else if (ulen == 4) {
		//sprintf(buf, "%02X%02X%02X%02X", uuid[3]&0xff, uuid[2]&0xff, uuid[1]&0xff, uuid[0]&0xff);
		sprintf(buf, "%02X%02X%02X%02X", uuid[0]&0xff, uuid[1]&0xff, uuid[2]&0xff, uuid[3]&0xff);
	} else if (ulen == 16) {
#if 0
		sprintf(buf, "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			uuid[15]&0xff, uuid[14]&0xff, uuid[13]&0xff, uuid[12]&0xff,
			uuid[11]&0xff, uuid[10]&0xff,
			uuid[9]&0xff, uuid[8]&0xff,
			uuid[7]&0xff, uuid[6]&0xff, uuid[05]&0xff, uuid[04]&0xff,
			uuid[3]&0xff, uuid[2]&0xff, uuid[01]&0xff, uuid[00]&0xff);
#else
		sprintf(buf, "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			uuid[0]&0xff, uuid[1]&0xff, uuid[2]&0xff, uuid[3]&0xff,
			uuid[4]&0xff, uuid[5]&0xff,
			uuid[6]&0xff, uuid[7]&0xff,
			uuid[8]&0xff, uuid[9]&0xff, uuid[10]&0xff, uuid[11]&0xff,
			uuid[12]&0xff, uuid[13]&0xff, uuid[14]&0xff, uuid[15]&0xff);

#endif
	} else {
		util_buf2hexstr(uuid, ulen, buf);
	}
	return buf;
}


char *util_attr_properties2str(unsigned char p, char *buf) {
	sprintf(buf, "r:%d,w:%d,n:%d", !!(p&0x02), !!(p&0x08), !!(p&0x10));
	return buf;
}

