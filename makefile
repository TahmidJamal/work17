all: control.o user.o
	gcc -o control control.o
	gcc -o user user.o

control.o: control.c
	gcc -c control.c

user.o: user.c
	gcc -c user.c

clean:
	rm *.o
	rm ./control ./user
