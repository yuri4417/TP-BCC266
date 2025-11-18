tporg: main.o maquina.o programas.o ram.o
	gcc -Wall main.o maquina.o programas.o ram.o -o tporg -lm

ram.o: ram.c 
	gcc -Wall -c ram.c
  
main.o: main.c
	gcc -Wall -c main.c

maquina.o: maquina.c
	gcc -Wall -c maquina.c

programas.o: programas.c
	gcc -Wall -c programas.c
    
clean:
	rm -f *.o tporg