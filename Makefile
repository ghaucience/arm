ROOTDIR=$(shell pwd)
WORKDIR=$(ROOTDIR)/build

ARCH								:= MT7620
USE_GM							:= 0

ifeq ($(ARCH),MT7620)
#CROSSTOOLDIR 			:= /home/ztp/work/MTK_APSoC_SDK/blebeacon-new/OpenWrt-SDK-ramips-mt7688_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64
CROSSTOOLDIR 				:= /home/au/all/gwork/openwrt
CROSS   						:= mipsel-openwrt-linux-
export  STAGING_DIR	:= $(CROSSTOOLDIR)/staging_dir
export  PATH				:= $(PATH):$(STAGING_DIR)/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin

CROSS_CFLAGS				:= -I$(CROSSTOOLDIR)/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/usr/include
CROSS_CFLAGS				+= -I$(CROSSTOOLDIR)/staging_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/usr/include
CROSS_LDFLAGS				:= -L$(CROSSTOOLDIR)/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/usr/lib
CROSS_LDFLAGS				+= -L$(CROSSTOOLDIR)/staging_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/usr/lib/ 
endif

GCC 		:= $(CROSS)gcc
CXX			:= $(CROSS)g++
AR			:= $(CROSS)ar
AS			:= $(CROSS)gcc
RANLIB	:= $(CROSS)ranlib
STRIP 	:= $(CROSS)strip
OBJCOPY	:= $(CROSS)objcopy
OBJDUMP := $(CROSS)objdump
SIZE		:= $(CROSS)size
LD			:= $(CROSS)ld
MKDIR		:= mkdir -p


TARGET_CFLAGS							:= -Wall -g -O2  
TARGET_CFLAGS							+= -I$(ROOTDIR)/gecko -I$(ROOTDIR)/ble/include/security
TARGET_CFLAGS							+= -I$(ROOTDIR)/src
TARGET_CFLAGS							+= -fPIC
#TARGET_CFLAGS							+= -fpermissive
TARGET_CFLAGS							+= $(CROSS_CFLAGS) 

TARGET_CXXFLAGS						:= -std=c++0x 
TARGET_CXXFLAGS						+= $(CFLAGS)

TARGET_LDFLAGS						:= -lm -ljansson -lrt -ldl -lpthread -ljson-c -lubox -lblobmsg_json -lubus -lcares -lssl -lcrypto
TARGET_LDFLAGS						+= -lstdc++
TARGET_LDFLAGS						+= $(CROSS_LDFLAGS)

ifeq ($(USE_GM),1)
TARGET_LDFLAGS						+= -DUSE_SM
endif

srcs	+= $(ROOTDIR)/main.c
srcs	+= $(ROOTDIR)/src/schedule.c
srcs	+= $(ROOTDIR)/src/system.c
srcs	+= $(ROOTDIR)/src/util.c
srcs 	+= $(ROOTDIR)/src/json_parser.c
srcs 	+= $(ROOTDIR)/src/uproto.c
srcs	+= $(ROOTDIR)/src/ubus.c


srcs  := $(subst .cpp,.c,$(srcs))

arm:  $(srcs)
	$(MKDIR) $(WORKDIR)
	$(GCC) 		$(TARGET_CFLAGS) $(TARGET_LDFLAGS) $(srcs) -o  $(WORKDIR)/$@
	$(STRIP)	$(WORKDIR)/$@

clean:
	rm -rf ./tags
	rm -rf ./build

scp:
	scp -P2204 $(WORKDIR)/arm root@192.168.0.230:/tmp