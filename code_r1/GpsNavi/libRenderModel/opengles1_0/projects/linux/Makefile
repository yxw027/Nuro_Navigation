#CROSS = arm-linux-
CROSS = 
CC = $(CROSS)gcc
CXX = $(CROSS)g++

VPATH = ../../src \
		../../src/linux \
		../../src/arm \
		../../src/codegen

CFLAGS = -O3 -fPIC -I../../src
CFLAGS += -I../../src/linux
CFLAGS += -I../../include
CFLAGS += -I../../src/arm
CFLAGS += -I../../src/codegen
CFLAGS += -I../../include

LD_FLAGS = -lX11 -lXext

DEFS = -DEGL_ON_LINUX

#ARM 平台加
#DEFS += -DARM
#DEFS += -DDENGXU_LOG #为记录LOG 打开会生成opengles_log.txt

SKIPS = %GenScanline.cpp
SKIPS_C =

SRCS = $(wildcard ../../src/*.cpp)
SRCS += $(wildcard ../../src/linux/*.cpp)
SRCS += $(wildcard ../../src/arm/*.cpp)

SRCS_C = $(wildcard ../../src/codegen/*.c)

# 过滤
SRCS := $(filter-out $(SKIPS),$(SRCS))
SRCS_C := $(filter-out $(SKIPS_C),$(SRCS_C))

# 生成OBJ
OBJS = $(foreach s,$(SRCS:%.cpp=%.o),$(shell basename $(s)))
OBJS += $(foreach s,$(SRCS_C:%.c=%.o),$(shell basename $(s)))

# OBJS += main.o

# 生成库
# main:$(OBJS)
# $(CXX) $^ -o $@ $(LD_FLAGS)
APP_SRCS = $(wildcard ./main.cpp)
APP_OBJS = $(foreach s,$(APP_SRCS:%.cpp=%.o),$(shell basename $(s)))
# main.o libRenderModel.o  main.cpp  
# OpenGLProcessZK.cpp  Read3DRModelZK.cpp  ReadBaseZK.cpp  ReadObjModelZK.cpp  

default:main

main:$(APP_OBJS) $(OBJS)
	$(CXX) $^ -o $@ $(LD_FLAGS)

lib:libOGLES.so

libOGLES.so:$(OBJS)
	$(CXX) -shared $^ -o $@ 

%.o:%.c
	@echo [$<]
	@$(CC) $(CFLAGS) $(DEFS) -c $< -o $@

%.o:%.cpp
	@echo [$<]
	@$(CXX) $(CFLAGS) $(DEFS) -c $< -o $@

clean:
	@rm *.o main 2>/dev/null;nothing=s;

.PHONY: clean
