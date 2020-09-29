all:
	gcc cpu.c -I../cpu -I../gb -I../gpu -I../input -I../romloader -I../sound -I../gb cb.c cpu.c memory.c interrupt.c -c -O3
	g++ -std=c++14 cpu_server.cc -I../rpclib/include ../rpclib/build/librpc.a cb.o cpu.o memory.o interrupt.o -o cpu_server $(CFLAGS) -O3

clean:
	rm -f *.o cpu_server *.dSYM
