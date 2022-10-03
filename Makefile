# Copyright 2022, Nick Botticelli. <nick.s.botticelli@gmail.com>

PROJECT  = XNU-syscall-hiding-PoC

CC       = xcrun -sdk macosx clang
CC_FLAGS = -Ofast -DNDEBUG -Wall

STRIP    = xcrun -sdk macosx strip

.PHONY: all clean
all: $(PROJECT)
	$(STRIP) $(PROJECT)

$(PROJECT): $(PROJECT).c
	$(CC) $(CC_FLAGS) $^ -o $@

clean:
	rm -rf $(PROJECT) $(PROJECT).dSYM *.o
