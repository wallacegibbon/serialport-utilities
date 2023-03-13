#ifndef __SERIALPORT_H
#define __SERIALPORT_H

enum SerialportJsonReaderState {
	READ_NORMAL, READ_STRING, READ_CHAR, READ_ESCAPE
};

struct SerialportJsonReader {
	struct sp_port *port;
	char buffer[256];
	int depth;
	int char_count;
	int time_count;
	enum SerialportJsonReaderState mode;
	int (*handle_char)(int ch);
};

void SerialportJsonReader_initialize(
	struct SerialportJsonReader *self, const char *portname, int baudrate
);

void SerialportJsonReader_destroy(struct SerialportJsonReader *self);

int SerialportJsonReader_get_json(
	struct SerialportJsonReader *self, int timeout
);

#endif

