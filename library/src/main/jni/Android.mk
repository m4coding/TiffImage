LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


#$(info $(all-subdir-makefiles))
#all-subdir-makefiles返回的时当前执行的makefile所在的目录的子目录Makefile，放在后面
#包含makefile，会导致包含到不正确的路径
#include $(all-subdir-makefiles)
#include $(call all-makefiles-under,$(call my-dir))


include $(call all-makefiles-under,$(LOCAL_PATH))


