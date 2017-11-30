

FLAGS += -Imodules/link/include -Imodules/link/modules/asio-standalone/asio/include

ifeq ($(OS),Windows_NT)
    CXXFLAGS += -DLINK_PLATFORM_WINDOWS=1
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


include ../../plugin.mk


DIST_NAME = Stellare
dist: all
ifndef VERSION
	$(error VERSION must be defined when making distributables)
endif
	mkdir -p dist/$(DIST_NAME)
	cp LICENSE* dist/$(DIST_NAME)/
	cp $(TARGET) dist/$(DIST_NAME)/
	cp -R res dist/$(DIST_NAME)/
	cd dist && zip -5 -r $(DIST_NAME)-$(VERSION)-$(ARCH).zip $(DIST_NAME)
