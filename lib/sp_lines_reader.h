#ifndef __SP_LINES_READER_H
#define __SP_LINES_READER_H

#include <libserialport.h>

enum SerialportLinesReaderState { READ_STARTING, READ_NORMAL, READ_ENDING, READ_ENDED, READ_ERROR };

#define SERIALPORT_BUFFER_SIZE 256

struct SerialportLinesReader {
	struct sp_port *port;
	char buffer[SERIALPORT_BUFFER_SIZE];
	enum SerialportLinesReaderState state;
	const char *error_info;
	int (*nonblocking_read)(struct SerialportLinesReader *self);
	int (*handle_char)(int ch);
	int time_count;
	/// `state_tag_buffer` stores "CCOVCLCBEGIN" and "CCOVCLCEND"
	char state_tag_buffer[32];
	int state_tag_index;
	int prev_is_newline;
};

void SerialportLinesReader_initialize(struct SerialportLinesReader *self);
void SerialportLinesReader_destroy(struct SerialportLinesReader *self);

int SerialportLinesReader_read(struct SerialportLinesReader *self, int timeout);

#endif

