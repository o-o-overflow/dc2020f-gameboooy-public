all:
	g++ -std=c++14 logs.cc -I ../rpclib/include/ ../rpclib/build/librpc.a -o logs_server $(CFLAGS)

clean:
	rm -fr logs_server *.dSYM
