ACLOCAL_AMFLAGS = -I m4
AM_DEFAULT_SOURCE_EXT = .cpp
bin_PROGRAMS = gpu_main

gpu_main_SOURCES = \
                gpu_main.cpp  \
                gpu_manager.cpp \
                smbus.cpp \
                gpus.cpp 

gpu_main_LDFLAGS =  -L../git/ \
                    -lgpu \
                    -lstdc++fs \
                    $(SDBUSPLUS_LIBS) \
                    $(PHOSPHOR_DBUS_INTERFACES_LIBS) \
                    $(SDEVENTPLUS_LIBS)

gpu_main_CXX_FLAGS =  \
                    $(PHOSPHOR_DBUS_INTERFACES_CFLAGS) \
	            $(SDBUSPLUS_CFLAGS) \
	            $(PHOSPHOR_LOGGING_CFLAGS)

gpudir = $(sysconfdir)/gpu
gpu_DATA = gpu_config.json
