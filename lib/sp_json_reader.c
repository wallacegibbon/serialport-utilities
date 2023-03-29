#include "sp_json_reader.h"
#include "sp_util.h"
#include <stdio.h>

int SerialportJsonReader_nonblocking_read(struct SerialportJsonReader *self);

void SerialportJsonReader_initialize(struct SerialportJsonReader *self) {
	self->state = READ_STARTING;
	self->time_count = 0;
	self->nonblocking_read = SerialportJsonReader_nonblocking_read;
	self->handle_char = putchar;
	self->depth = 0;
	self->port = NULL;
}

void SerialportJsonReader_destroy(struct SerialportJsonReader *self) {
	sp_close(self->port);
	sp_free_port(self->port);
}

void SerialportJsonReader_consume_char_escape(
	struct SerialportJsonReader *self, char ch
) {
	/// nothing to do
}

void SerialportJsonReader_consume_char_string(
	struct SerialportJsonReader *self, char ch
) {
	switch (ch) {
	case '"':
		self->state = READ_NORMAL;
		break;
	case '\\':
		self->state = READ_ESCAPE;
		break;
	}
}

void SerialportJsonReader_consume_char_normal(
	struct SerialportJsonReader *self, char ch
) {
	switch (ch) {
	case '{':
		self->depth++;
		break;
	case '}':
		self->depth--;
		break;
	case '"':
		self->state = READ_STRING;
		break;
	}

	if (self->depth == 0)
		self->state = READ_ENDED;
}

void SerialportJsonReader_consume_char_starting(
	struct SerialportJsonReader *self, char ch
) {
	if (ch == '{') {
		self->state = READ_NORMAL;
		SerialportJsonReader_consume_char_normal(self, ch);
		return;
	}

	self->state = READ_ERROR;
	self->error_info = "invalid start character";
}

void SerialportJsonReader_consume_char(
	struct SerialportJsonReader *self, char ch
) {
	switch (self->state) {
	case READ_STARTING:
		SerialportJsonReader_consume_char_starting(self, ch);
		break;
	case READ_NORMAL:
		SerialportJsonReader_consume_char_normal(self, ch);
		break;
	case READ_STRING:
		SerialportJsonReader_consume_char_string(self, ch);
		break;
	case READ_ESCAPE:
		SerialportJsonReader_consume_char_escape(self, ch);
		break;
	}

	self->handle_char(ch);
}

int SerialportJsonReader_nonblocking_read(struct SerialportJsonReader *self) {
	return sp_nonblocking_read(self->port, self->buffer, SERIALPORT_BUFFER_SIZE);
}

void SerialportJsonReader_next(struct SerialportJsonReader *self) {
	char *cursor;
	int count;

	count = self->nonblocking_read(self);
	if (count < 0)
		exit_info(-1, "failed reading serial port: (%d)\n", count);

	cursor = self->buffer;

	while (count-- && self->state != READ_ENDED)
		SerialportJsonReader_consume_char(self, *cursor++);

	switch (self->state) {
	case READ_ERROR:
		exit_info(-2, "%s\n", self->error_info);
		break;
	default:
		sleep_milliseconds(100);
		self->time_count += 100;
		break;
	}
}

static inline int SerialportJsonReader_unfinished(
	struct SerialportJsonReader *self, int timeout
) {
	return self->state != READ_ENDED && self->time_count < timeout;
}

int SerialportJsonReader_read(
	struct SerialportJsonReader *self, int timeout
) {
	while (SerialportJsonReader_unfinished(self, timeout))
		SerialportJsonReader_next(self);

	return self->time_count < timeout;
}

