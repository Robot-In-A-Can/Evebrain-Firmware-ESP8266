#ifndef __discovery_h__
#define __discovery_h__

extern "C" {
#include "lwip/inet.h"
#include "espconn.h"
}

#define DISCOVERY_HOST "local.evelab.io"

void send_discovery_request(uint32_t, char *);

#endif
