all: prog3_32 prog3_64

prog3_32: prog3.c
	gcc -g -m32 -std=gnu99 prog3.c -o prog3_32 -lm

prog3_64: prog3.c
	gcc -g -m64 -std=gnu99 prog3.c -o prog3_64 -lm

clean:
	rm prog3_32 prog3_64

clean_32:
	rm prog3_32

clean_64:
	rm prog3_64
