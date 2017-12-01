
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
	SOURCES += modules$(wildcard modules/link-wrapper/*.cpp)
endif

include ../../plugin.mk


DIST_NAME = VCV-Link
dist: all
ifndef VERSION
	$(error VERSION must be defined when making distributables)
endif
	mkdir -p dist/$(DIST_NAME)
	cp LICENSE* dist/$(DIST_NAME)/
	cp $(TARGET) dist/$(DIST_NAME)/
	cp -R res dist/$(DIST_NAME)/
	cd dist && zip -5 -r $(DIST_NAME)-$(VERSION)-$(ARCH).zip $(DIST_NAME)
