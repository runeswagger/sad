all: sad

sad:
	gcc -Wall -Wextra -o sad sad.c event.c delay.c -lpthread

event-test:
	gcc -Wall -Wextra -o event-test event-test.c event.c delay.c

libvlc-player:
	gcc -o libvlc-player libvlc-player.c `pkg-config --cflags --libs libvlc`
	
clean:
	rm -f sad event-test libvlc-player
