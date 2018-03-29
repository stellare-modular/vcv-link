SLUG = StellareModular-Link
VERSION = 0.6.0

FLAGS += -DSLUG=$(SLUG) -DVERSION=$(VERSION)
FLAGS += -Imodules/link/include -Imodules/link/modules/asio-standalone/asio/include -Ilink-wrapper

# If RACK_DIR is not defined when calling the Makefile, default to two levels above
RACK_DIR ?= ../..

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

SOURCES += $(wildcard src/*.cpp)

DISTRIBUTABLES += $(wildcard LICENSE*) res

# Include the VCV Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk
