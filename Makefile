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
#TARGET_CFLAGS							+= -fPIC -DJSON_IS_AMALGAMATION
TARGET_CFLAGS							+= -fPIC 
#TARGET_CFLAGS							+= -fpermissive
TARGET_CFLAGS							+= $(CROSS_CFLAGS) 

TARGET_CXXFLAGS						:= -std=c++0x 
TARGET_CXXFLAGS						+= $(TARGET_CFLAGS)

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
#srcs	+= $(ROOTDIR)/src/arm.c
srcs	+= $(ROOTDIR)/src/jsoncpp.cpp
srcs	+= $(ROOTDIR)/src/armpp.cpp

srcs  := $(subst .cpp,.c,$(srcs))
objs 	:= $(subst $(ROOTDIR),$(WORKDIR), $(subst .c,.o,$(srcs)))

arm:  $(objs)
	$(MKDIR) $(WORKDIR)
	#$(GCC) 	$(TARGET_CFLAGS) $(TARGET_LDFLAGS) $(srcs) -o  $(WORKDIR)/$@
	$(GCC) 		$(TARGET_LDFLAGS) $(objs) -o  $(WORKDIR)/$@
	$(STRIP)	$(WORKDIR)/$@


$(ROOTDIR)/build/%.o : $(ROOTDIR)/%.c
	@$(MKDIR) $(dir $@)
	$(GCC) -c $< $(CFLAGS) $(TARGET_CFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -o $@

$(ROOTDIR)/build/%.o : $(ROOTDIR)/%.cpp
	@$(MKDIR) $(dir $@)
	$(CXX) -c $< $(CXXFLAGS) $(TARGET_CXXFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -o $@


clean:
	rm -rf ./tags
	rm -rf ./build

install: arm
	cp ./build/arm ./files/usr/bin/ -rf
	cp ./files ./files_install -rf
	/home/au/all/gwork/openwrt/staging_dir/host/bin/luac -o ./files_install/usr/lib/lua/luci/controller/Dusun/arm.lua ./files/usr/lib/lua/luci/controller/Dusun/arm.lua
	/home/au/all/gwork/openwrt/staging_dir/host/bin/luac -o ./files_install/usr/lib/lua/luci/model/cbi/admin_dusun/arm.lua ./files/usr/lib/lua/luci/model/cbi/admin_dusun/arm.lua
	(rm -rf /tmp/files.tar.gz; cd ./files_install; tar zcvf /tmp/files.tar.gz .; cd -; rm -rf ./files_install)
	

scp:
	#scp -P2201 $(WORKDIR)/arm $(ROOTDIR)/files/etc/config/dusun/alarm/alarm.json root@192.168.0.230:/tmp
	scp -P2201 $(WORKDIR)/arm  root@192.168.0.230:/tmp
	#scp -P2201 $(ROOTDIR)/files/etc/config/dusun/alarm/alarm.json root@192.168.0.230:/etc/config/dusun/alarm/
	scp -P2201 $(ROOTDIR)/files/usr/lib/lua/luci/model/cbi/admin_dusun/arm.lua root@192.168.0.230:/usr/lib/lua/luci/model/cbi/admin_dusun/
