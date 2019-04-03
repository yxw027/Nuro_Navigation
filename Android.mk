#############################################################
## Author:dengxu 
## Date: 2011-11-03
#############################################################

BUILD_PATH_PRI=./jni

DIR_INC = \
		  $(BUILD_PATH_PRI)/code_r1/baselib \
		  $(BUILD_PATH_PRI)/code_r1/baselib/NuroOsApi/src \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/nuroInclude \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libInNavi \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libFileSystem \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libFileMap \
		  $(BUILD_PATH_PRI)/code_r1/NuroApi \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libRoute \
		  $(BUILD_PATH_PRI)/code_r1/libNuroWnds \
		  $(BUILD_PATH_PRI)/code_r1/libNuroWnds/cpp \
		  $(BUILD_PATH_PRI)/code_r1/libNuroWnds/BitmapMgr \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libSearchNew \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libMemMgr \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libLCMMUI \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/NaviUI \
		  $(BUILD_PATH_PRI)/code_r1/libNuroWnds/BitmapMgr \
		  $(BUILD_PATH_PRI)/code_r1/libCallManager \
		  $(BUILD_PATH_PRI)/code_r1/libGDI \
		  $(BUILD_PATH_PRI)/code_r1/libGDI/cpp \
		  $(BUILD_PATH_PRI)/code_r1/libGDI/cpp/freetype2312/include \
		  $(BUILD_PATH_PRI)/code_r1/libGDI/cpp/freetype2312/include/freetype/internal \
		  $(BUILD_PATH_PRI)/code_r1/libGDI/cpp/lpng1512 \
		  $(BUILD_PATH_PRI)/code_r1/libGDI/cpp/lpng1512/lpng1512 \
		  $(BUILD_PATH_PRI)/code_r1/libGDI/cpp/zlib127 \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/nuGpsOpen \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libDrawMap \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libNaviCtrl \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libRenderModel \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libRenderModel/opengles1_0/src \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libRenderModel/opengles1_0/src/linux \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libRenderModel/opengles1_0/include \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libRenderModel/opengles1_0/src/arm \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libRenderModel/opengles1_0/src/codegen \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libMathTool \
		  $(BUILD_PATH_PRI)/code_r1/libCallManager \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/nuroInclude \
		  $(BUILD_PATH_PRI)/code_r1/NuroApi \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libRenderModel \
		  $(BUILD_PATH_PRI)/code_r1/baselib/NuroMethod \
		  $(BUILD_PATH_PRI)/code_r1/tinyxml
		  

FLT_DIR_INC = 

DIR_C = \
		$(BUILD_PATH_PRI)/code_r1/baselib/NuroClib/src \
		$(BUILD_PATH_PRI)/code_r1/baselib/NuroOsApi/src \
		$(BUILD_PATH_PRI)/code_r1/libGDI/cpp/freetype2312/src \
		$(BUILD_PATH_PRI)/code_r1/libGDI/cpp/freetype2312/src2 \
		$(BUILD_PATH_PRI)/code_r1/libGDI/cpp/lpng1512 \
		$(BUILD_PATH_PRI)/code_r1/libGDI/cpp/lpng1512/lpng1512 \
		$(BUILD_PATH_PRI)/code_r1/libGDI/cpp/zlib127/zlib-1.2.7 \
		$(BUILD_PATH_PRI)/code_r1/GpsNavi/libRenderModel/opengles1_0/src \
		$(BUILD_PATH_PRI)/code_r1/GpsNavi/libRenderModel/opengles1_0/src/linux \
		$(BUILD_PATH_PRI)/code_r1/GpsNavi/libRenderModel/opengles1_0/src/arm \
		$(BUILD_PATH_PRI)/code_r1/GpsNavi/libRenderModel/opengles1_0/src/codegen \
		$(BUILD_PATH_PRI)/code_r1/OpenResource/7z \
		$(BUILD_PATH_PRI)/code_r1/GpsNavi/NaviUI/cpp

FLT_DIR_C = 

DIR_CPP = \
		  $(BUILD_PATH_PRI)/code_r1/baselib/NuroMethod/src \
		  $(BUILD_PATH_PRI)/code_r1/baselib/ActiveCode \
		  $(BUILD_PATH_PRI)/code_r1/baselib/NuroCrossInterface/src \
		  $(BUILD_PATH_PRI)/code_r1/baselib/New \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libInNavi \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libFileSystem \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libRoute \
		  $(BUILD_PATH_PRI)/code_r1/libNuroWnds \
		  $(BUILD_PATH_PRI)/code_r1/libNuroWnds/cpp \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libSearchNew \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libFileMap \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libMemMgr \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libLCMMUI/cpp \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/NaviUI/cpp \
		  $(BUILD_PATH_PRI)/code_r1/libNuroWnds/BitmapMgr \
		  $(BUILD_PATH_PRI)/code_r1/libCallManager \
		  $(BUILD_PATH_PRI)/code_r1/libGDI/cpp \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/nuGpsOpen \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libDrawMap \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libNaviCtrl \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libRenderModel \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libRenderModel/opengles1_0/src \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libRenderModel/opengles1_0/src/linux \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libRenderModel/opengles1_0/src/arm \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libRenderModel/opengles1_0/src/codegen \
		  $(BUILD_PATH_PRI)/code_r1/OpenResource \
		  $(BUILD_PATH_PRI)/code_r1/GpsNavi/libMathTool \
          $(BUILD_PATH_PRI)/code_r1/tinyxml

FLT_DIR_CPP = 

FLT_SRCS_C = 

FLT_SRCS_CPP = \
			   %FileMapTMC.cpp \
			   %GenScanline.cpp \
			   %FileList.cpp \
			   %FileStru.cpp \
			   %SharedMemory.cpp

CFLAGS += -D_USE_OPENGLES
CFLAGS += -O3 -fPIC
CFLAGS += -DEGL_ON_LINUX
#LDFLAGS += 

DIR_INC := $(filter-out $(FLT_DIR_INC),$(DIR_INC))
DIR_C := $(filter-out $(FLT_DIR_C),$(DIR_C))
DIR_CPP := $(filter-out $(FLT_DIR_CPP),$(DIR_CPP))

SRCS_C = $(foreach s,$(DIR_C),$(wildcard $(s)/*.c))
SRCS_CPP = $(foreach s,$(DIR_CPP),$(wildcard $(s)/*.cpp))
SRCS_C := $(filter-out $(FLT_SRCS_C),$(SRCS_C))
SRCS_CPP := $(filter-out $(FLT_SRCS_CPP),$(SRCS_CPP))

-include $(BUILD_PATH_PRI)/make.inc

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := nuro_r1
LOCAL_SRC_FILES := $(patsubst $(BUILD_PATH_PRI)/%,%,$(SRCS_C) $(SRCS_CPP))
LOCAL_C_INCLUDES := $(DIR_INC)

LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog
#LOCAL_LDLIBS += -lz
#LOCAL_LDLIBS += -L./jni/proLinux -lHZRecog_r_mips 
#LOCAL_LDLIBS += -L./jni/proLinux -lejTTS_mips
LOCAL_LDLIBS += -L./jni/proLinux -lHZRecog_r_arm
LOCAL_LDLIBS += -L./jni/proLinux -lejTTS_arm 
#LOCAL_LDLIBS += -L./jni/code_r1/tinyxml -ltinyxml
LOCAL_CFLAGS = $(CFLAGS) -fwide-exec-charset=UTF-16LE

include $(BUILD_SHARED_LIBRARY) 

