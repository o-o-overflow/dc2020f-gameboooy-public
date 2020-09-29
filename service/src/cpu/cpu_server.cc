#include "../gb/gb.h"
#include "rpc/server.h"
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>

int main() {
	puts("spawning cpu...");
	puts("verify flag is in ../flag");
	puts("game on");
	  setvbuf ( stdout, NULL , _IONBF, 0);
  rpc::server srv(CPU_SERVER); // listen on TCP port 8080

  srv.bind("cpu::execute", [](std::vector<uint8_t> state, std::string nsteps) -> std::vector<uint8_t> {
    gameboy_t gb = {0};
    std::vector<uint8_t> mem2;

    /*
    if (state.size() < sizeof(gameboy_t))
      return mem2;
      */

    memcpy(&gb, &state[0], sizeof(gameboy_t));

    int steps = atoi(nsteps.c_str());
    if (steps <= 0 || steps > 256) {
      steps = 1;
    }

	  for (int i = 0; i < steps; i++) {
		  cpuStep(&gb);
    }

    uint8_t *p = (uint8_t *)&gb;
    for (int i = 0; i < sizeof(gameboy_t); i++) {
      mem2.push_back(p[i]);
    }
    return mem2;
  });

  srv.bind("cpu::reset", [](std::vector<uint8_t> state) -> std::vector<uint8_t> {
    //gameboy_t *gb = (gameboy_t *)&state[0];
	  gameboy_t gb;
    reset(&gb);
    std::vector<uint8_t> mem2;
    uint8_t *p = (uint8_t *)&gb;
    for (int i = 0; i < sizeof(gameboy_t); i++) {
      mem2.push_back(p[i]);
    }
    return mem2;
  });
  
  srv.async_run(4);
    while (1) { sleep(60); }
  return 0;
}
