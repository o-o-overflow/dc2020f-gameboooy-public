#include "../gb/gb.h"
#include "rpc/server.h"
#include <unistd.h>
#include <iostream>
#include <vector>

gameboy_t stepGpu(std::vector<uint8_t> state) {
    gameboy_t gb;

    memcpy(&gb, &state[0], sizeof(gameboy_t));
    gpuStep(&gb);
    return gb;
}

int main() {
	puts("spawning gpu...");
	puts("verify flag in ../flag");
	puts("game on");
	setvbuf ( stdout, NULL , _IONBF, 0);
  rpc::server srv(GPU_SERVER); // listen on TCP port 8080

  srv.bind("gpu::step", [](std::vector<uint8_t> state) -> std::vector<uint8_t> {
    gameboy_t gb = stepGpu(state);
    uint8_t *p = (uint8_t *)&gb;
    std::vector<uint8_t> mem2;
    for (int i = 0; i < sizeof(gameboy_t); i++) {
      mem2.push_back(p[i]);
    }
    return mem2;
  });

  srv.bind("gpu::interrupt", [](std::vector<uint8_t> state) -> std::vector<uint8_t> {
    gameboy_t gb = {0};
    std::vector<uint8_t> mem2;

    memcpy(&gb, &state[0], sizeof(gameboy_t));
    postDraw(&gb);
    uint8_t *p = (uint8_t *)&gb;
    for (int i = 0; i < sizeof(gameboy_t); i++) {
      mem2.push_back(p[i]);
    }
    return mem2;
  });

  srv.bind("gpu::reset", []() -> bool {
    gpuReset();
    return true;
  });

  srv.bind("gpu::render", [](std::vector<uint8_t> state) -> std::vector<uint8_t> {
    gameboy_t gb = {0};
    std::vector<uint8_t> mem;

    if (state.size() < sizeof(gameboy_t))
      return mem;

    memcpy(&gb, &state[0], sizeof(gameboy_t));

    size_t length = 0;
    uint8_t *image = NULL;
    int ok = drawFramebuffer(&gb, &image, &length);

    // zero length vector will indicate failure to remote
	  if (!ok) return mem;
      if (image) {
        uint8_t *p = image;
        for (int i = 0; i < length; i++) {
          mem.push_back(p[i]);
        }

        free(image);
    }

    return mem;
  });

  srv.async_run(4);
  while (1) { sleep(60); }
  //srv.run(); // blocking call, handlers run on this thread.
  return 0;
}
