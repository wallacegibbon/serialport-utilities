#include "sp_json_reader.h"
#include "sp_util.h"
#include <stdio.h>

int SerialportJsonReader_nonblocking_read(struct SerialportJsonReader *self);

void SerialportJsonReader_initialize(struct SerialportJsonReader *self) {
	self->char_count = 0;
	self->time_count = 0;
	self->depth = 0;
	self->mode = READ_NORMAL;
	self->handle_char = putchar;
	self->nonblocking_read = SerialportJsonReader_nonblocking_read;
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
		self->mode = READ_NORMAL;
		break;
	case '\\':
		self->mode = READ_ESCAPE;
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
		self->mode = READ_STRING;
		break;
	}
}

enum ConsumeResult { UNFINISHED, FINISHED, INVALID_JSON };

enum ConsumeResult SerialportJsonReader_consume_char(
	struct SerialportJsonReader *self, char ch
) {
	if (self->char_count == 0 && ch != '{')
		return INVALID_JSON;

	switch (self->mode) {
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

	self->char_count++;
	self->handle_char(ch);

	//printf(">>>> %d, %d\n", self->char_count, self->depth);

	if (self->depth == 0)
		return FINISHED;
	else
		return UNFINISHED;
}

void SerialportJsonReader_check(
	struct SerialportJsonReader *self, enum ConsumeResult prev_consume_ret
) {
	switch (prev_consume_ret) {
	case INVALID_JSON:
		exit_info(-1, "invalid JSON data response\n");
		break;
	case UNFINISHED:
		/// take a sleep before next nonblocking read from serial port
		sleep_milliseconds(100);
		self->time_count += 100;
		break;
	case FINISHED:
		break;
	}
}

int SerialportJsonReader_nonblocking_read(
	struct SerialportJsonReader *self
) {
	return sp_nonblocking_read(self->port, self->buffer, SERIALPORT_BUFFER_SIZE);
}

void SerialportJsonReader_next(struct SerialportJsonReader *self) {
	enum SerialportJsonReaderState ret;
	char *cursor;
	int count;

	count = self->nonblocking_read(self);
	if (count < 0)
		exit_info(ret, "failed reading serial port: (%d)\n", count);

	cursor = self->buffer;
	ret = UNFINISHED;

	while (count-- && ret == UNFINISHED)
		ret = SerialportJsonReader_consume_char(self, *cursor++);

	SerialportJsonReader_check(self, ret);
}

static inline int SerialportJsonReader_unfinished(
	struct SerialportJsonReader *self, int timeout
) {
	return (
		(self->char_count == 0 || self->depth > 0) &&
		(self->time_count < timeout)
	);
}

int SerialportJsonReader_get_json(
	struct SerialportJsonReader *self, int timeout
) {
	while (SerialportJsonReader_unfinished(self, timeout))
		SerialportJsonReader_next(self);

	return self->time_count < timeout;
}

