LDLIBS += -lseccomp

OS := "$(shell uname)"

CFLAGS ?=

# Add -pthread for Linux building
ifeq ($(OS), "Linux")
	CFLAGS :=-pthread -no-pie
endif

export CFLAGS
server:
	make -C bus
	make -C cpu
	make -C gpu
	make -C romloader
	make -C input
	make -C logs

#../service: service
#	mv service ..

strip:
	strip bus/emu; strip gpu/gpu_server; strip cpu/cpu_server; strip romloader/romloader_server; strip input/input_server; strip logs/logs_server
