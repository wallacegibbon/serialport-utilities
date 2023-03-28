#include "sp_lines_reader.h"
#include "sp_util.h"
#include <string.h>
#include <stdio.h>

const char *starting_string = "CCOVCLCBEGIN";
const char *ending_string = "CCOVCLCEND";

int SerialportLinesReader_nonblocking_read(struct SerialportLinesReader *self);

void SerialportLinesReader_initialize(struct SerialportLinesReader *self) {
	self->phase = STARTING;
	self->phase_tag_index = 0;
	self->time_count = 0;
	self->nonblocking_read = SerialportLinesReader_nonblocking_read;
	self->port = NULL;
}

void SerialportLinesReader_destroy(struct SerialportLinesReader *self) {
	sp_close(self->port);
	sp_free_port(self->port);
}

int SerialportLinesReader_nonblocking_read(
	struct SerialportLinesReader *self
) {
	return sp_nonblocking_read(self->port, self->buffer, SERIALPORT_BUFFER_SIZE);
}

enum ConsumeResult { UNFINISHED, FINISHED, INVALID_START };

enum ConsumeResult SerialportLinesReader_consume_char_ending(
	struct SerialportLinesReader *self, char ch
) {
	int i;

	if (self->phase_tag_index < strlen(ending_string)) {
		self->phase_tag_buffer[self->phase_tag_index++] = ch;
		return UNFINISHED;
	}

	self->phase_tag_buffer[self->phase_tag_index] = '\0';

	if (strcmp(self->phase_tag_buffer, ending_string) == 0)
		return FINISHED;

	/// If it's not a ending tag, print it.
	for (i = 0; i < PHASE_TAG_BUFFER_SIZE; i++)
		putchar(self->phase_tag_buffer[i]);

	self->phase = NORMAL;
	//self->phase_tag_index = 0;

	return UNFINISHED;
}

enum ConsumeResult SerialportLinesReader_consume_char_starting(
	struct SerialportLinesReader *self, char ch
) {
	if (self->phase_tag_index < strlen(starting_string)) {
		self->phase_tag_buffer[self->phase_tag_index++] = ch;
		return UNFINISHED;
	}

	self->phase_tag_buffer[self->phase_tag_index] = '\0';

	if (strcmp(self->phase_tag_buffer, starting_string) != 0)
		return INVALID_START;

	self->phase = NORMAL;
	//self->phase_tag_index = 0;

	return UNFINISHED;
}

enum ConsumeResult SerialportLinesReader_consume_char_normal(
	struct SerialportLinesReader *self, char ch
) {
	int is_not_ending = 0;

	if (ch == '\n' || ch == '\r') {
		is_not_ending = 1;
		self->phase_tag_index = 0;
	}

	if (ch != ending_string[0] || self->phase_tag_index != 0)
		is_not_ending = 1;

	if (is_not_ending) {
		putchar(ch);
		return UNFINISHED;
	}

	self->phase = ENDING;
	return SerialportLinesReader_consume_char_ending(self, ch);
}

enum ConsumeResult SerialportLinesReader_consume_char(
	struct SerialportLinesReader *self, char ch
) {
	enum ConsumeResult result;
	switch (self->phase) {
	case STARTING:
		result = SerialportLinesReader_consume_char_starting(self, ch);
		break;
	case NORMAL:
		result = SerialportLinesReader_consume_char_normal(self, ch);
		break;
	case ENDING:
		result = SerialportLinesReader_consume_char_ending(self, ch);
		break;
	}

	return result;
}

void SerialportLinesReader_check(
	struct SerialportLinesReader *self, enum ConsumeResult prev_consume_ret
) {
	switch (prev_consume_ret) {
	case INVALID_START:
		exit_info(-1, "invalid start string\n");
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

void SerialportLinesReader_next(
	struct SerialportLinesReader *self, int timeout
) {
	enum SerialportLinesReaderState ret;
	char *cursor;
	int count;

	count = self->nonblocking_read(self);
	if (count < 0)
		exit_info(ret, "failed reading serial port: (%d)\n", count);

	cursor = self->buffer;
	ret = UNFINISHED;

	while (count-- && ret == UNFINISHED)
		ret = SerialportLinesReader_consume_char(self, *cursor++);

	SerialportLinesReader_check(self, ret);
}

static inline int SerialportLinesReader_unfinished(
	struct SerialportLinesReader *self, int timeout
) {
	return (
		self->time_count < timeout
	);
}

int SerialportLinesReader_get_lines(
	struct SerialportLinesReader *self, int timeout
) {
	while (SerialportLinesReader_unfinished(self, timeout))
		SerialportLinesReader_next(self, timeout);

	return self->time_count < timeout;
}

