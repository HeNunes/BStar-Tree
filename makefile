flags:=-g

all: crimes.o registros.o index.o ab.o modulos_ab.o auxiliar.o fornecido.o main.o
	gcc $(flags) main.o crimes.o registros.o index.o ab.o modulos_ab.o auxiliar.o fornecido.o -o exec -std=c99 -lm 

registros.o:
	gcc $(flags) -c registros.c -o registros.o

index.o:
	gcc $(flags) -c index.c -o index.o

ab.o:
	gcc $(flags) -c ab.c -o ab.o

modulos_ab.o:
	gcc $(flags) -c modulos_ab.c -o modulos_ab.o

auxiliar.o:
	gcc $(flags) -c auxiliar.c -o auxiliar.o 

crimes.o:
	gcc $(flags) -c crimes.c -o crimes.o 

fornecido.o:
	gcc $(flags) -c fornecido.c -o fornecido.o 

main.o:
	gcc $(flags) -c main.c -o main.o 

clean:
	rm *.o exec

run:
	./exec

test:
	./exec < 1.in > 1.out

valgrind:
	valgrind -s --leak-check=full --show-leak-kinds=all ./exec < 1.in
