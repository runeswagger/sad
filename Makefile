all: sad

sad:
	gcc -Wall -Wextra -o sad sad.c event.c delay.c -lpthread

event-test:
	gcc -Wall -Wextra -o event-test event-test.c event.c delay.c
	
clean:
	rm -f sad event-test
