SLUG = StellareModular-Link
VERSION = 0.6.0dev

FLAGS += -DSLUG=$(SLUG) -DVERSION=$(VERSION)
FLAGS += -Imodules/link/include -Imodules/link/modules/asio-standalone/asio/include -Ilink-wrapper

ifeq ($(OS),Windows_NT)
	# On Windows uses the wrapper DLL, needs to be build separately
	LDFLAGS += -Lsrc -llink-wrapper
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CXXFLAGS += -DLINK_PLATFORM_LINUX=1
    endif
    ifeq ($(UNAME_S),Darwin)
        CXXFLAGS += -DLINK_PLATFORM_MACOSX=1
    endif
endif

SOURCES = $(wildcard src/*.cpp)

ifneq ($(OS),Windows_NT)
	# On Mac and Windows directly compiles the wrapper
	SOURCES += $(wildcard link-wrapper/*.cpp)
endif

DISTRIBUTABLES += $(wildcard LICENSE*) res

# If RACK_DIR is not defined when calling the Makefile, default to two levels above
RACK_DIR ?= ../..

# Include the VCV Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk
