project ?= CpuCardCalculator
objs = des.o package.o main.o


dep_files := $(patsubst %,.%.d, $(objs))
dep_files := $(wildcard $(dep_files))

using_gdb ?= 0

INCLUDEDIR 	:= $(shell pwd)/inc
CFLAGS := -Werror -I$(INCLUDEDIR)
OFLAGS := -lpthread -lm # -lmysqlclient
ifeq ($(using_gdb),1)
CFLAGS := $(CFLAGS) -g
OFLAGS := $(OFLAGS) -g
endif

# MYSQLINCLUDEDIR := /data/JS7628/openwrt_CC_mt76xx_source/staging_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/usr/include

ifndef ARCH
ARCH = x64
endif

ifndef CROSS_COMPILE
ifeq ($(ARCH),x64)
CROSS_COMPILE = 
endif
ifeq ($(ARCH),x86)
CROSS_COMPILE = 
CFLAGS := $(CFLAGS) -m32
OFLAGS := $(OFLAGS) -m32
endif
ifeq ($(ARCH),ppc)
CROSS_COMPILE = ppc_8xx-
endif
ifeq ($(ARCH),arm)
CROSS_COMPILE = arm-linux-gnueabihf-
endif
ifeq ($(ARCH),i386)
CROSS_COMPILE = i386-linux-
endif
ifeq ($(ARCH),mips)
CROSS_COMPILE = mipsel-linux-
endif
ifeq ($(ARCH),mips_openwrt)
CROSS_COMPILE = mipsel-openwrt-linux-
CFLAGS := $(CFLAGS) -I$(MYSQLINCLUDEDIR)
endif
ifeq ($(ARCH),nios)
CROSS_COMPILE = nios-elf-
endif
ifeq ($(ARCH),nios2)
CROSS_COMPILE = nios2-elf-
endif
ifeq ($(ARCH),m68k)
CROSS_COMPILE = m68k-elf-
endif
ifeq ($(ARCH),microblaze)
CROSS_COMPILE = mb-
endif
endif


CC      = $(CROSS_COMPILE)gcc
CXX     = $(CROSS_COMPILE)g++
LINK    = $(CROSS_COMPILE)g++
LD      = $(CROSS_COMPILE)ld
AR      = $(CROSS_COMPILE)ar
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump


export INCLUDEDIR CFLAGS OFLAGS ARCH CROSS_COMPILE CC CXX LD AR OBJCOPY OBJDUMP



all : $(objs)
	$(CC) -o $(project) $^ $(OFLAGS)
	$(OBJDUMP) -D $(project) > $(project).dis
	# $(LD) -Ttext 0 $^ -o $(project).elf
	# $(OBJCOPY) -O binary -S $(project).elf $(project).bin
	# $(OBJDUMP) -D $(project).elf > $(project).dis
	
ifneq ($(dep_files),)
    include $(dep_files)
endif

%.o : %.c
	$(CC) -c $(CFLAGS) $< -MD -MF .$@.d -o $@

%.o : %.cpp
	$(CXX) -c $(CFLAGS) $< -MD -MF .$@.d -o $@

%.o : %.S
	$(CC) -c $(CFLAGS) $< -o $@

clean :
	$(RM) *.o *.dis $(dep_files) $(project)
	
.PHONY: clean
