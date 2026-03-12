TARGET			=	bin/test
SOURCES			=	unit_tests.c
OBJ_PATH		?=  obj
CC				?=  gcc
CFLAGS			?=	-O0 -g

LIBS_HEADERS_URLS +=	https://github.com/sheredom/utest.h.git

###

OBJECTS=$(SOURCES:%.c=$(OBJ_PATH)/%.o)
DEPS=$(OBJECTS:.o=.d)
DEPFLAGS=-MMD -MP

LIBS_HEADERS_PATHS = $(foreach url,$(LIBS_HEADERS_URLS),$(shell basename $(url) .git))

$(foreach url,$(LIBS_HEADERS_URLS),$(eval URL_$(shell basename $(url) .git) := $(url)))

INCLUDE_DIRS += $(LIBS_HEADERS_PATHS)
CFLAGS += $(addprefix -I, $(INCLUDE_DIRS))

.PHONY: all clean cleanall test

all: $(LIBS_HEADERS_PATHS) $(TARGET)

clean:
	rm -rf $(LIBS_HEADERS_PATHS) $(OBJ_PATH)

cleanall: clean
	rm -rf bin

test: all
	$(TARGET)

$(LIBS_HEADERS_PATHS):
	$(if $(wildcard ../$@), ln -s ../$@ $@, git clone $(URL_$@) $@ 	)
	$(MAKE)

$(TARGET): $(OBJECTS)
	mkdir -p $(dir $(TARGET))
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LDLIBS)


$(OBJ_PATH)/%.o: %.c Makefile
	mkdir -p $(dir $@)
	$(CC) $(DEPFLAGS) $(CFLAGS) -c $< -o $@

-include $(DEPS)
