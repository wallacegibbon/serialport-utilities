#include "serialport.h"
#include "libserialport.h"
#include "common_util.h"
#include <stdio.h>

void SerialportJsonReader_initialize(
	struct SerialportJsonReader *self, const char *portname, int baudrate
) {
	int ret;

	self->depth = 0;
	self->char_count = 0;
	self->time_count = 0;
	self->mode = READ_NORMAL;
	self->handle_char = putchar;

	ret = sp_get_port_by_name(portname, &self->port);
	if (ret < 0)
		exit_info(ret, "failed getting port by name \"%s\": (%d)\n", portname, ret);

	ret = sp_open(self->port, SP_MODE_READ_WRITE);
	if (ret < 0)
		exit_info(ret, "failed opening port \"%s\": (%d)\n", portname, ret);

	sp_set_baudrate(self->port, baudrate);
	sp_set_bits(self->port, 8);
	sp_set_parity(self->port, SP_PARITY_NONE);
	sp_set_stopbits(self->port, 1);
	sp_set_flowcontrol(self->port, SP_FLOWCONTROL_NONE);
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

void SerialportJsonReader_consume_char_char(
	struct SerialportJsonReader *self, char ch
) {
	switch (ch) {
	case '\'':
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
	case '\'':
		self->mode = READ_CHAR;
		break;
	}
}

int SerialportJsonReader_consume_char(
	struct SerialportJsonReader *self, char ch
) {
	switch (self->mode) {
	case READ_NORMAL:
		SerialportJsonReader_consume_char_normal(self, ch);
		break;
	case READ_STRING:
		SerialportJsonReader_consume_char_string(self, ch);
		break;
	case READ_CHAR:
		SerialportJsonReader_consume_char_char(self, ch);
		break;
	case READ_ESCAPE:
		SerialportJsonReader_consume_char_escape(self, ch);
		break;
	}

	self->char_count++;
	self->handle_char(ch);

	//printf(">>>> %d, %d\n", self->char_count, self->depth);
	if (self->depth == 0)
		return 0;
	else
		return 1;
}

int SerialportJsonReader_next(struct SerialportJsonReader *self) {
	char *cursor;
	int ret;
	int count;

	count = sp_nonblocking_read(self->port, self->buffer, 256);
	if (count < 0)
		exit_info(ret, "failed reading serial port: (%d)\n", count);

	cursor = self->buffer;
	ret = 1;

	while (count-- && ret)
		ret = SerialportJsonReader_consume_char(self, *cursor++);

	if (ret) {
		sleep_milliseconds(100);
		self->time_count += 100;
	}
}

static inline int SerialportJsonReader_finished(
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
	while (SerialportJsonReader_finished(self, timeout))
		SerialportJsonReader_next(self);

	return self->time_count < timeout;
}

