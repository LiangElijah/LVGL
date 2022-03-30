TARGET = main
SOURCES = $(wildcard src/*.c)
OBJECTS = $(patsubst %.c,%.o,$(SOURCES))

CC = arm-none-linux-gnueabihf-gcc
INCLUDES = -Iinclude -I../Git/lvgl/out/include
LIBRARIES = -llvgl -L../Git/lvgl/out/lib
OPTIONS = -Wno-packed-bitfield-compat

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(LIBRARIES) $(OPTIONS)

%.o: %.c
	$(CC) -c $< -o $@ $(INCLUDES) $(OPTIONS)

.PHONY: clean distclean
clean:
	rm -rf $(OBJECTS)

distclean:
	rm -rf $(OBJECTS) $(TARGET)