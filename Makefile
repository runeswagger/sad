all: sad

sad:
	gcc -Wall -Wextra -o sad sad.c event.c delay.c -lpthread

clean:
	rm -f sad
