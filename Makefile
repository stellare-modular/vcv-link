# If RACK_DIR is not defined when calling the Makefile, defaults to use the Rack-SDK submodule
RACK_DIR ?= Rack-SDK/lin

include $(RACK_DIR)/arch.mk

ifeq ($(ARCH_OS_NAME), lin)
    RACK_DIR = Rack-SDK/lin
    CXXFLAGS += -DLINK_PLATFORM_LINUX=1
endif

ifeq ($(ARCH_OS_NAME), mac)
    RACK_DIR = Rack-SDK/mac
    CXXFLAGS += -DLINK_PLATFORM_MACOSX=1
endif

ifeq ($(ARCH_OS_NAME), win)
    RACK_DIR = Rack-SDK/win
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
