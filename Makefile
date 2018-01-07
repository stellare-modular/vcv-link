SLUG = VCV-Link
VERSION = 0.5.1

include ../../arch.mk

FLAGS += -Ilink-wrapper -Imodules/link/include -Imodules/link/modules/asio-standalone/asio/include

ifeq ($(ARCH), lin)
    CXXFLAGS += -DLINK_PLATFORM_LINUX=1
endif

ifeq ($(ARCH), mac)
    CXXFLAGS += -DLINK_PLATFORM_MACOSX=1
endif

ifeq ($(ARCH), win)
    CXXFLAGS += -DLINK_PLATFORM_WINDOWS=1
	LDFLAGS += -L/c/Windows/System32 -lwsock32 -lws2_32 -liphlpapi
endif

SOURCES = $(wildcard src/*.cpp) $(wildcard link-wrapper/*.cpp)

# Add files to the ZIP package when running `make dist`
DISTRIBUTABLES += $(wildcard LICENSE*) res

include ../../plugin.mk
