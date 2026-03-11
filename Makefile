TARGET		=	bin/unit_tests
SOURCES		=   unit_tests.c
OBJ_PATH	?=  obj
CC			=   gcc
CFLAGS		?=	-O0 -g

###

OBJECTS=$(SOURCES:%.c=$(OBJ_PATH)/%.o)
DEPS=$(OBJECTS:.o=.d)
DEPFLAGS=-MMD -MP

all: $(TARGET)

clean:
	@rm -rf $(OBJ_PATH) bin

cleanall: clean

test: $(TARGET)
	$(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(dir $(TARGET))
	@$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LDLIBS)

$(OBJ_PATH)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(DEPFLAGS) $(CFLAGS) -c $< -o $@

-include $(DEPS)