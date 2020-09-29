#include "../gb/gb.h"
#include "rpc/server.h"
#include "rpc/client.h"
#include <iostream>
#include <vector>

extern "C" {
	void logit(char *x) {
		std::vector<uint8_t> l;
		l.push_back(41);
		l.push_back(41);
		l.push_back(41);
		l.push_back(41);
		l.push_back(41);
		l.push_back(41);
				rpc::client logger("localhost", LOGS_SERVER);
				logger.call("logs::log", l);
	}
}
