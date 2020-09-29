#include "../gb/gb.h"
#include "rpc/client.h"
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <vector>
using std::fstream;
using std::string;
using std::vector;

int frame2 = 0;

vector<uint8_t> vec_from_key(keys_t key) {
	vector<uint8_t> v;
	uint8_t *p = (uint8_t *)&key;
	for (int i = 0; i < sizeof(keys_t); i++) {
		v.push_back(p[i]);
	}
	return v;
}

void pk(char ch) {
	int p = 0;
	if (p) {
		printf("PRESSED %c\n", ch);
	}
}

keys_t ch_to_keys(char c) {
	keys_t k;
	pk(c);
	memset(&k, 0, sizeof(k));
	if (c == 'w') {
		k.KEY2.up = 1;
	} else if (c == 'a') {
		k.KEY2.left = 1;
	} else if (c == 's') {
		k.KEY2.down = 1;
	} else if (c == 'd') {
		k.KEY2.down = 1;
	} else if (c == 'b') {
		k.KEY1.b = 1;
	} else if (c == 'n') {
		k.KEY1.a = 1;
	} else if (c == 'o') {
		k.KEY1.select = 1;
	} else if (c == 'p') {
		k.KEY1.select = 1;
	}
	return k;
}

vector<uint8_t> vec_from_gb(gameboy_t gb) {
	vector<uint8_t> v;
	uint8_t *p = (uint8_t *)&gb;
	for (int i = 0; i < sizeof(gameboy_t); i++) {
		v.push_back(p[i]);
	}
	return v;
}

gameboy_t gb_from_vec(vector<uint8_t> vec) {
	gameboy_t gb;
	memcpy(&gb, &vec[0], sizeof(gameboy_t));
	return gb;
}

void dumpregs(vector<uint8_t> vec) {
	gameboy_t gb = gb_from_vec(vec);
	printf("PC: 0x%x\n", gb.cpu.registers.PC);
	printf("SP: 0x%x\n", gb.cpu.registers.SP);
}

int main(int argc, char **argv) {
	int fwport = 0;
	char *host;
	if (argc == 4) {
		host = argv[2];
		fwport = atoi(argv[3]);
	} else if (argc == 3) {
		host = argv[2];
	}
	std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(size);
	if (!file.read(buffer.data(), size)) {
		return 0;
	}
	vector<uint8_t> cart;
	for (int i = 0; i < size; i++) {
		cart.push_back(buffer.data()[i]);
	}
	printf("gameboooy running!\n");
	gameboy_t gb;
	rpc::client romloader(host, fwport ? fwport : ROMLOADER_SERVER);
	rpc::client cpu(host, fwport ? fwport : CPU_SERVER);
	rpc::client gpu(host, fwport ? fwport : GPU_SERVER);
	rpc::client input(host, fwport ? fwport : INPUT_SERVER);
	std::vector<uint8_t> result = vec_from_gb(gb);
	std::vector<uint8_t> lol;
	printf("b\n");
	result = cpu.call("cpu::reset", vec_from_gb(gb)).as<vector<uint8_t>>();
	printf("c\n");
	bool b = gpu.call("gpu::reset").as<bool>();
	printf("d\n");
	//result = cpu.call("cpu::reset", 420).as<vector<uint8_t>>();
	gb = gb_from_vec(result);
	result =
		romloader.call("romloader::load", result, cart).as<vector<uint8_t>>();
	std::vector<uint8_t> image;
	volatile size_t i = 0;


	for (i;; i++) {
		bool b = gpu.call("gpu::reset").as<bool>();
		fd_set set;
		struct timeval tv;

		tv.tv_sec = 0;
		tv.tv_usec = 1;

		FD_ZERO( &set );
		FD_SET( fileno( stdin ), &set );

		int res = 0;
		//int res = select( fileno( stdin )+1, &set, NULL, NULL, &tv );

		char c = 0;
		if( res > 0 )
		{
			read( fileno( stdin ), &c, 1 );
		}

		//if (c) {
		keys_t kt = ch_to_keys('w');
		vector<uint8_t> kp = vec_from_key(kt);
		result = input.call("input::input", result, kp)
			.as<vector<uint8_t>>();

		//}
		result = cpu.call("cpu::execute", result, std::string("255"))
			.as<vector<uint8_t>>();
		result = gpu.call("gpu::step", result).as<vector<uint8_t>>();
		if ((i % 0x10) == 0) {
			printf("%d\n", i);
		}
		if (i == 11064 + 1) {
			//	abort();
		}

		image = gpu.call("gpu::render", result).as<vector<uint8_t>>();
		if (i == 10) {
			FILE *pFile;
			pFile = fopen("check2.bin", "wb");
			fwrite(result.data(), sizeof(char), result.size(), pFile);
			fclose(pFile);
			exit(0);

		}
		if (i == 8) {
			FILE *pFile;
			pFile = fopen("check1.bin", "wb");
			fwrite(result.data(), sizeof(char), result.size(), pFile);
			fclose(pFile);
		}
		if (image.size() > 0) {
			std::ofstream screen("screen.bmp", std::ios::binary | std::ios::trunc);
			screen.write((char *)image.data(), image.size());
			screen.close();
			gb = gb_from_vec(result);
			// printf("%p\n", &gb);
			printf("cnt: %d\n", i);
		}
		result = gpu.call("gpu::interrupt", result).as<vector<uint8_t>>();
	}
}
