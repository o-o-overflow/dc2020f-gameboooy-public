all: libbmp.o
	gcc gpu_impl.c -fno-stack-protector -c -O0 -g
	gcc ../cpu/memory.c -c $(CFLAGS) -O3  -g
	g++ -std=c++14 -c ./logme.cc -I../rpclib/include/  -O2 -g
	g++ -std=c++14 -no-pie gpu.cc gpu_impl.o libbmp.o memory.o logme.o -I ../rpclib/include/ ../rpclib/build/librpc.a -o gpu_server $(CFLAGS) -O0  -g -lpthread
