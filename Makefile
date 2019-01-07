# If RACK_DIR is not defined when calling the Makefile, defaults to use the Rack-SDK submodule
RACK_DIR ?= Rack-SDK

# Must follow the format in the Naming section of
# https://vcvrack.com/manual/PluginDevelopmentTutorial.html
SLUG = StellareModular-Link

# Must follow the format in the Versioning section of
# https://vcvrack.com/manual/PluginDevelopmentTutorial.html
VERSION = 0.6.1

include $(RACK_DIR)/arch.mk

ifeq ($(ARCH), lin)
    CXXFLAGS += -DLINK_PLATFORM_LINUX=1
endif

ifeq ($(ARCH), mac)
    CXXFLAGS += -DLINK_PLATFORM_MACOSX=1
endif

ifeq ($(ARCH), win)
    CXXFLAGS += -DLINK_PLATFORM_WINDOWS=1
	LDFLAGS += -lwsock32 -lws2_32 -liphlpapi
endif

FLAGS += -Imodules/link/include -Imodules/link/modules/asio-standalone/asio/include

SOURCES += $(wildcard src/*.cpp)

# Add files to the ZIP package when running `make dist`
# The compiled plugin is automatically added.
DISTRIBUTABLES += $(wildcard LICENSE*) res

# Include the VCV Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk
