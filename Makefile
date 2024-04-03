CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

all: build

clean:
	clear
	rm -f simple-talk

build:
	gcc ${CFLAGS} main.c list.o receiver.c sender.c printer.c keyboard.c listOperations.c -o simple-talk -lpthread
