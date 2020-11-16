#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>


#include <pthread.h>
#include <sys/time.h>
#include <signal.h>

#include "common.h"
#include "system.h"
#include "ubus.h"
#include "schedule.h"
#include "simlog.h"

#include "armpp.h"

static int  use_cmd		= 0;
static char *version_path = "/etc/config/dusun/arm/version";

static int parse_args(int argc, char *argv[]);
static int usage(char *app);
static int write_pid();
static int write_version(const char *verfile);
static void run_loop();

int main(int argc, char *argv[]) {
	if (parse_args(argc, argv) != 0) {
		usage(argv[0]);
		return -1;
	}

	write_pid();

	if (ubus_init() != 0) {
		log_err("ubus init error");
		return -1;
	}
	log_info("ubus init ok!");

	system("mkdir -p /tmp/arm");
	
	write_version(version_path);
	
	run_loop();

	return 0;
}

static int parse_args(int argc, char *argv[]) {
	int ch = 0;
	while((ch = getopt(argc,argv,"d:b:C"))!= -1){
		switch(ch){
			case 'C':
				use_cmd = 1;
				break;
			default:
				return -1;
				break;
		}
	}
	return 0;
}

static int usage(char *app) {
	printf(	"Usage: %s [options] ...\n"
					"Options:\n"
					"  -C                       if use command line.\n"
					"For more infomation, please mail to dlauciende@gmail.com\n", app);
	return 0;
}

static int write_pid() {
	char out[256];
	system_cmd("ps | grep arm | grep -v \"grep\" | wc -l", out);
	if (atoi(out) <= 1) {
		system("rm -rf /var/run/arm.pid");
	} 

	char *pid_file = "/var/run/arm.pid";
	int fd = -1;
	if (access(pid_file, F_OK) != 0) {
		fd = open(pid_file, O_WRONLY | O_CREAT, 0644);
	} else {
		fd = open(pid_file, O_WRONLY);
	}
	
	if (fd < 0) {
		return -1;
	}
	
	if (flock(fd, LOCK_EX | LOCK_NB) < 0) {
		return -2;
	}
	
	char buf[64];
	
	sprintf(buf, "%d\n", (int)getpid());
	if (write(fd, buf, strlen(buf)) != strlen(buf)) {
		return -3;
	}
	
	return 0;
}

static int  write_version(const char *verfile) {
	FILE *fp = fopen(verfile, "w");
	if (fp == NULL) {
		return -1;
	}

	char buf[256];
	sprintf(buf, "Version:%d.%d.%d, DateTime:%s %s\n", MAJOR, MINOR, PATCH, TIME, DATE);
	fwrite(buf, strlen(buf), 1, fp);
	fclose(fp);
	return 0;
}

static void run_loop() {
	log_info(" ");

	armpp_init();

	while (1) {
		/**> loop */
		fd_set	fds;
		FD_ZERO(&fds);
		int maxfd = 0;

#if 0
		/**> command line */
		FD_SET(0, &fds);
		maxfd = 0;
#endif

		/**> ubus */
		int ufd = ubus_get_fd();
		if (ufd > 0) {
			FD_SET(ufd, &fds);
			if (ufd > maxfd) {
				maxfd = ufd;
			}
		}
	
		struct timeval	tv;
		_S64_t next = schedue_first_task_delay();
		if (next < 0) {
			next = 4 * 1000 + 80;
		} else if (next == 0) {
			schedue_exec();	
		}
		
		//log_debug("gfd:%d, ufd:%d\n", gfd, ufd);
		tv.tv_sec = next / 1000;
		tv.tv_usec = (next % 1000) * 1000;
		int ret = select(maxfd + 1, &fds, NULL, NULL, &tv);
		if (ret <= 0) {
			continue;
		}

#if 0
		/**> command line */
		if (FD_ISSET(0, &fds)) { // command line
			extern void cmd_in(void *arg, int fd);
			cmd_in(NULL, 0);
		} 
#endif

		/**> ubus */
		if (FD_ISSET(ufd, &fds)) {
			uproto_tick();
		}
	}
}
