CFLAGS = -Wall -Wextra -g

all: sad

sad:
	gcc $(CFLAGS) -o sad sad.c event.c delay.c -lpthread
	
event-test:
	gcc $(CFLAGS) -o event-test event-test.c event.c delay.c

libvlc-player:
	gcc $(CFLAGS) -o libvlc-player libvlc-player.c `pkg-config --cflags --libs libvlc`

documentation:
	doxygen
	
clean:
	rm -rf sad event-test libvlc-player doc/*
