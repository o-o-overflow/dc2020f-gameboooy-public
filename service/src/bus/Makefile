all:
	g++ -O3 -std=c++14 -I../rpclib/include -I../gb/ emu.cc ../rpclib/build/librpc.a -o emu $(CFLAGS) -lpthread
	g++ -O3 -std=c++14 -I../rpclib/include -I../gb/ konami.cc ../rpclib/build/librpc.a -o konami $(CFLAGS) -lpthread
	g++ -O3 -std=c++14 -I../rpclib/include -I../gb/ check1.cc ../rpclib/build/librpc.a -o check1 $(CFLAGS) -lpthread
	g++ -O3 -std=c++14 -I../rpclib/include -I../gb/ pwn.cc ../rpclib/build/librpc.a -o pwn $(CFLAGS) -lpthread -g

clean:
	rm -fr *dSYM emu
