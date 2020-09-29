#include "../gb/gb.h"
#include "rpc/server.h"
#include <vector>

int main() {
	puts("GAME ON!\n");
  std::vector<std::vector<uint8_t> > logs;
  rpc::server srv(LOGS_SERVER); // listen on TCP port 8080

  srv.bind("logs::erase", [&logs](bool nothing) -> bool {
		  logs.clear();
		  return true;
  });

  srv.bind("logs::log", [&logs](std::vector<uint8_t> log) -> bool {
		  logs.push_back(log);
		  return true;
  });

  srv.bind("logs::retreive", [&logs](bool nothing) -> std::vector<std::vector<uint8_t> > {
		  return logs;
  });

  srv.run(); // blocking call
  return 0;
}
