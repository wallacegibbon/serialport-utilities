#include "sp_lines_reader.h"
#include "sp_util.h"
#include <string.h>
#include <stdio.h>

const char *starting_string = "CCOVCLCBEGIN";
const char *ending_string = "CCOVCLCEND";

int SerialportLinesReader_nonblocking_read(struct SerialportLinesReader *self);

void SerialportLinesReader_initialize(struct SerialportLinesReader *self) {
	self->state = READ_STARTING;
	self->time_count = 0;
	self->nonblocking_read = SerialportLinesReader_nonblocking_read;
	self->handle_char = putchar;
	self->state_tag_index = 0;
	self->prev_is_newline = 0;
	self->port = NULL;
}

void SerialportLinesReader_destroy(struct SerialportLinesReader *self) {
	sp_close(self->port);
	sp_free_port(self->port);
}

void SerialportLinesReader_consume_char_normal(
	struct SerialportLinesReader *self, char ch
);

void SerialportLinesReader_consume_char_ending(
	struct SerialportLinesReader *self, char ch
) {
	int i;

	if (self->state_tag_index < strlen(ending_string)) {
		self->state_tag_buffer[self->state_tag_index++] = ch;
		return;
	}

	self->state_tag_buffer[self->state_tag_index] = '\0';

	if (strcmp(self->state_tag_buffer, ending_string) == 0) {
		self->state = READ_ENDED;
		return;
	}

	/// If it's not a ending tag, print it.
	for (i = 0; i < self->state_tag_index; i++)
		self->handle_char(self->state_tag_buffer[i]);

	self->state = READ_NORMAL;
	self->state_tag_index = 0;

	SerialportLinesReader_consume_char_normal(self, ch);
}

void SerialportLinesReader_consume_char_starting(
	struct SerialportLinesReader *self, char ch
) {
	if (self->state_tag_index < strlen(starting_string)) {
		self->state_tag_buffer[self->state_tag_index++] = ch;
		return;
	}

	self->state_tag_buffer[self->state_tag_index] = '\0';

	if (strcmp(self->state_tag_buffer, starting_string) != 0) {
		self->state = READ_ERROR;
		self->error_info = "invalid start tag";
		return;
	}

	self->state = READ_NORMAL;
	self->state_tag_index = 0;
}

void SerialportLinesReader_consume_char_normal(
	struct SerialportLinesReader *self, char ch
) {
	if (ch == '\n' || ch == '\r') {
		self->prev_is_newline = 1;
		self->handle_char(ch);
		return;
	}

	if (!(ch == ending_string[0] && self->prev_is_newline)) {
		self->handle_char(ch);
		return;
	}

	self->state = READ_ENDING;
	SerialportLinesReader_consume_char_ending(self, ch);
}

void SerialportLinesReader_consume_char(
	struct SerialportLinesReader *self, char ch
) {
	switch (self->state) {
	case READ_STARTING:
		SerialportLinesReader_consume_char_starting(self, ch);
		break;
	case READ_NORMAL:
		SerialportLinesReader_consume_char_normal(self, ch);
		break;
	case READ_ENDING:
		SerialportLinesReader_consume_char_ending(self, ch);
		break;
	}
}

int SerialportLinesReader_nonblocking_read(
	struct SerialportLinesReader *self
) {
	return sp_nonblocking_read(self->port, self->buffer, SERIALPORT_BUFFER_SIZE);
}

void SerialportLinesReader_next(struct SerialportLinesReader *self) {
	char *cursor;
	int count;

	count = self->nonblocking_read(self);
	if (count < 0)
		exit_info(-1, "failed reading serial port: (%d)\n", count);

	cursor = self->buffer;

	while (count-- && self->state != READ_ENDED)
		SerialportLinesReader_consume_char(self, *cursor++);

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

static inline int SerialportLinesReader_unfinished(
	struct SerialportLinesReader *self, int timeout
) {
	return self->state != READ_ENDED && self->time_count < timeout;
}

int SerialportLinesReader_read(
	struct SerialportLinesReader *self, int timeout
) {
	while (SerialportLinesReader_unfinished(self, timeout))
		SerialportLinesReader_next(self);

	return self->time_count < timeout;
}

