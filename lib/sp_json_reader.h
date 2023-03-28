#ifndef __SP_JSON_READER_H
#define __SP_JSON_READER_H

#include <libserialport.h>

enum SerialportJsonReaderState { READ_NORMAL, READ_STRING, READ_ESCAPE };

#define SERIALPORT_BUFFER_SIZE 256

struct SerialportJsonReader {
	struct sp_port *port;
	char buffer[SERIALPORT_BUFFER_SIZE];
	int char_count;
	int time_count;
	int depth;
	enum SerialportJsonReaderState mode;
	int (*handle_char)(int ch);
	int (*nonblocking_read)(struct SerialportJsonReader *self);
};

void SerialportJsonReader_initialize(struct SerialportJsonReader *self);

void SerialportJsonReader_destroy(struct SerialportJsonReader *self);

int SerialportJsonReader_get_json(
	struct SerialportJsonReader *self, int timeout
);

#endif

