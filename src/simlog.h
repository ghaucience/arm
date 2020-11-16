#ifndef __SIMPLE_LOG_H_
#define __SIMPLE_LOG_H_

#include <unistd.h>
#include <sys/time.h>

static char *appname = "BUL";

#define LOG_DEFAULT		39
#define LOG_RED			31
#define LOG_GREEN		32
#define LOG_YELLOW		33
#define LOG_BLUE		34
#define LOG_MAGENTA		35
#define LOG_CYAN		36
#define LOG_LIGHTGRAY		37
#define LOG_DARKGRAY		90
#define LOG_LIGHTRED		91
#define LOG_LIGHTGREEN		92
#define LOG_LIGHTYELLOW		93
#define LOG_LIGHTBLUE		94
#define LOG_LIGHTMAGENTA	95
#define LOG_LIGHTCYAN		96
#define LOG_WHITE		97

#ifndef LOG_FMT_DISABLE
#define LOG_FMT_ESC(code)	"\e[" #code "m"
#else
#define LOG_FMT_ESC(code)
#endif
#define LOG_FMT(code)		LOG_FMT_ESC(code)
#define LOG_FMT_BOLD(code)	LOG_FMT_ESC(1) LOG_FMT_ESC(code)
#define LOG_FMT_CLEAR		LOG_FMT_ESC(0)

static inline char *log_time_str() {
	struct timeval tv;
	struct tm cal;

	gettimeofday(&tv, NULL);
	gmtime_r(&tv.tv_sec, &cal);

	static char buf[64];
	int size = sizeof(buf);

	snprintf(buf, size,
	     "%04d-%02d-%02dT%02d:%02d:%02d.%03u",
	      cal.tm_year + 1900,
	      cal.tm_mon + 1,
	      cal.tm_mday,
	      cal.tm_hour,
	      cal.tm_min,
	      cal.tm_sec,
	      (unsigned)(tv.tv_usec / 1000));

	return buf;
}
#define TIME_NOW()	

#define log_info(...)			(printf(LOG_FMT_BOLD(LOG_GREEN)  "[%s][INF]" LOG_FMT_CLEAR "%s::%36s() " , log_time_str(), appname, __func__), printf(__VA_ARGS__), printf("\n"))
#define log_debug(...)		(printf(LOG_FMT_BOLD(LOG_BLUE)	 "[%s][DBG]" LOG_FMT_CLEAR "%s::%36s() " , log_time_str(), appname, __func__), printf(__VA_ARGS__), printf("\n"))
#define log_err(...)			(printf(LOG_FMT_BOLD(LOG_RED)		 "[%s][ERR]" LOG_FMT_CLEAR "%s::%36s() " , log_time_str(), appname, __func__), printf(__VA_ARGS__), printf("\n"))
#define log_warn(...)			(printf(LOG_FMT_BOLD(LOG_YELLOW) "[%s][WRN]" LOG_FMT_CLEAR "%s::%36s() " , log_time_str(), appname, __func__), printf(__VA_ARGS__), printf("\n"))

#define log_debug_hex(str, buf, len) (printf(LOG_FMT_BOLD(LOG_YELLOW)  "[%s][DBG]" LOG_FMT_CLEAR "%s::%36s() ", log_time_str(), appname, __func__), printf("%s", str), ({ \
	int i = 0; \
	printf(LOG_FMT_BOLD(LOG_CYAN) "\n[HEX]" LOG_FMT_CLEAR "%s::%36s() ", appname, __func__); \
	for (i = 0; i < len; i++) { \
		printf("[%02X] ", (buf)[i]&0xff);\
		if ( (i+1)% 20 == 0) { \
			printf(LOG_FMT_BOLD(LOG_CYAN) "\n[HEX]" LOG_FMT_CLEAR "%s::%36s() ", appname, __func__); \
		} \
	}	 \
}), printf("\n"))



#endif
