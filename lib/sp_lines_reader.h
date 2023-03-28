#ifndef __SP_LINES_READER_H
#define __SP_LINES_READER_H

#include <libserialport.h>

enum SerialportLinesReaderState { STARTING, NORMAL, ENDING };

#define SERIALPORT_BUFFER_SIZE 256
#define PHASE_TAG_BUFFER_SIZE 32

struct SerialportLinesReader {
	struct sp_port *port;
	char buffer[SERIALPORT_BUFFER_SIZE];
	enum SerialportLinesReaderState phase;
	/// `phase_tag_buffer` stores "CCOVCLCBEGIN" and "CCOVCLCEND"
	char phase_tag_buffer[PHASE_TAG_BUFFER_SIZE];
	int phase_tag_index;
	int time_count;
	int (*nonblocking_read)(struct SerialportLinesReader *self);
};

void SerialportLinesReader_initialize(struct SerialportLinesReader *self);

void SerialportLinesReader_destroy(struct SerialportLinesReader *self);

int SerialportLinesReader_get_lines(
	struct SerialportLinesReader *self, int timeout
);

#endif

