#ifndef __SP_PORT_H
#define __SP_PORT_H

#include <libserialport.h>

void sp_open_port(
	struct sp_port **port, const char *portname, int baudrate
);

#endif

