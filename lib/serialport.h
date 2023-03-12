#ifndef __SERIALPORT_H
#define __SERIALPORT_H

struct SerialportJsonReader {
	struct sp_port *port;
	char buffer[256];
	int depth;
	int char_count;
	int time_count;
	int (*handle_char)(int ch);
};

void SerialportJsonReader_initialize(
	struct SerialportJsonReader *self, const char *portname
);

void SerialportJsonReader_destroy(struct SerialportJsonReader *self);

int SerialportJsonReader_get_json(
	struct SerialportJsonReader *self, int timeout
);

#endif

