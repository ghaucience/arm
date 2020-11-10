#ifndef __UTIL_H_
#define __UTIL_H_


#ifdef __cplusplus
extern "C" {
#endif



	char *util_mac2str(char *mac, char *str);
	char *util_str2mac(char *str, char *mac);
			
	int   util_hexstr2buf(char *str, char *buf);
	char *util_buf2hexstr(char *buf, int len, char *str);

	char *util_macswap(char *mac, char *buf);
	char *util_bufswap(char *buf, int len, char *out);

	int		util_uuidstr2buf(char *uuid, char *buf);
	char *util_buf2uuidstr(char *uuid, int ulen, char *buf);

	char *util_attr_properties2str(unsigned char p, char *buf);

#ifdef __cplusplus
}
#endif



#endif
