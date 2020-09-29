#include "rom.h"
#include "../gb/gb.h"
#include "rpc/server.h"
#include "rpc/client.h"
#include <iostream>
#include <vector>

void foo() { std::cout << "Hey, I'm a free function." << std::endl; }

int main() {
  rpc::server srv(ROMLOADER_SERVER); // listen on TCP port 8080

  srv.bind("romloader::load", [](std::vector<uint8_t> state, std::vector<uint8_t> cart) -> std::vector<uint8_t> {
		printf("loading!\n");

    gameboy_t gb = {0};
    std::vector<uint8_t> state2;
    if (state.size() < sizeof(gameboy_t))
      return state2;

    memcpy(&gb, &state[0], sizeof(gameboy_t));

	  uint8_t *cart_data = (uint8_t *)&cart[0];
	  size_t cart_size = cart.size();
	  char *logs = (char *)calloc (1, 4096 * 4);
	  loadROM(&gb, cart_data, cart_size, &logs);
	  printf("%s\n", logs);
	  std::vector<uint8_t> l;
	  for (int i = 0; i < strlen(logs); i++) {
		  l.push_back(logs[i]);
	  }
	  free (logs);
	  rpc::client logger("localhost", LOGS_SERVER);
	  logger.call("logs::log", l);

    uint8_t *d = (uint8_t *)&gb;
    for (int i = 0; i < sizeof(gameboy_t); i++) {
        state2.push_back(d[i]);
    }
    return state2;
  });

  srv.run(); // blocking call, handlers run on this thread.
  return 0;
}
