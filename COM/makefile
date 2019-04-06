RM=rm -rf
CC=gcc

C_SRCS=$(shell find . -iname "*.c" | tr '\n' ' ')
H_SRCS=$(shell find . -iname "*.h" | tr '\n' ' ')
OBJS=$(C_SRCS:./%.c=build/%.o)

# Clean and compile .so
all: build/liblqvg.so

create-dirs:
	mkdir -p build/lqvg/

build/liblqvg.so: create-dirs $(OBJS)
	$(CC) -shared -o "build/liblqvg.so" $(OBJS)

build/lqvg/%.o: lqvg/%.c
	$(CC) -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

#build/commons/collections/%.o: commons/collections/%.c
#	$(CC) -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

# Add debug parameters and compile
debug: CC += -DDEBUG -g
debug: all

# Clean build files
clean:
	$(RM) build

install: all
	cp -u build/liblqvg.so /usr/lib
	cp --parents -u $(H_SRCS) /usr/include

uninstall:
	rm -f /usr/lib/liblqvg.so
	rm -rf /usr/include/lqvg

.PHONY: all create-dirs clean install uninstall
