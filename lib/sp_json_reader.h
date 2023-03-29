#ifndef __SP_JSON_READER_H
#define __SP_JSON_READER_H

#include <libserialport.h>

enum SerialportJsonReaderState { READ_STARTING, READ_NORMAL, READ_STRING, READ_ESCAPE, READ_ENDED, READ_ERROR };

#define SERIALPORT_BUFFER_SIZE 256

struct SerialportJsonReader {
	struct sp_port *port;
	char buffer[SERIALPORT_BUFFER_SIZE];
	enum SerialportJsonReaderState state;
	const char *error_info;
	int (*nonblocking_read)(struct SerialportJsonReader *self);
	int (*handle_char)(int ch);
	int time_count;
	int depth;
};

void SerialportJsonReader_initialize(struct SerialportJsonReader *self);
void SerialportJsonReader_destroy(struct SerialportJsonReader *self);

int SerialportJsonReader_read(struct SerialportJsonReader *self, int timeout);

#endif

