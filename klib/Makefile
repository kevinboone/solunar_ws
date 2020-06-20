NAME    := klib
VERSION := 0.0.1
LIBS    := ${EXTRA_LIBS} 
TARGET	:= $(NAME).a
SOURCES := $(shell find src/ -type f -name *.c)
OBJECTS := $(patsubst src/%,build/%,$(SOURCES:.c=.o))
DEPS	:= $(OBJECTS:.o=.deps)
CFLAGS  := -O3 -fpie -fpic -Wall -Werror -DNAME=\"$(NAME)\" -DVERSION=\"$(VERSION)\" -DSHARE=\"$(SHARE)\" -DPREFIX=\"$(PREFIX)\" -I include ${EXTRA_CFLAGS}
LDFLAGS := -pie ${EXTRA_LDFLAGS} -ffunction-sections -fdata-sections

$(TARGET): $(OBJECTS) 
	$(AR) -r $(TARGET) $(OBJECTS)	

build/%.o: src/%.c
	@mkdir -p build/
	$(CC) $(CFLAGS) -MD -MF $(@:.o=.deps) -c -o $@ $<

-include $(DEPS)

clean:
	$(RM) -r build/ $(TARGET)

.PHONY: clean

