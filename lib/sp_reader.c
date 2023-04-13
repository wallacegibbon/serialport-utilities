#include "sp_reader.h"
#include "sp_util.h"
#include <stdlib.h>

int SerialportReader_nonblocking_read(struct SerialportReader *self);

void SerialportReader_initialize(
	struct SerialportReader *self, int buffer_size
) {
	self->buffer = malloc(buffer_size);
	if (!self->buffer)
		exit_info(-1, "failed alloc memory for buffer\n");

	self->buffer_size = buffer_size;
	self->time_count = 0;
	self->nonblocking_read = SerialportReader_nonblocking_read;
	self->port = NULL;
	self->consumer = NULL;
}

void SerialportReader_destroy(struct SerialportReader *self) {
	free(self->buffer);
	sp_close(self->port);
	sp_free_port(self->port);
}

int SerialportReader_nonblocking_read(struct SerialportReader *self) {
	return sp_nonblocking_read(self->port, self->buffer, self->buffer_size);
}

static inline int SerialportReader_consume(
	struct SerialportReader *self, char ch
) {
	return (*self->consumer)->consume(self->consumer, ch);
}

static inline int SerialportReader_finished(
	struct SerialportReader *self
) {
	return (*self->consumer)->finished(self->consumer);
}

static inline int SerialportReader_check_error(
	struct SerialportReader *self, const char **buf
) {
	return (*self->consumer)->check_error(self->consumer, buf);
}

void SerialportReader_next(struct SerialportReader *self) {
	char *cursor;
	const char *error_info;
	int count;

	count = self->nonblocking_read(self);
	if (count < 0)
		exit_info(-1, "failed reading serial port: (%d)\n", count);

	/// reset counter when data comes
	if (count > 0)
		self->time_count = 0;

	cursor = self->buffer;

	while (count-- && !SerialportReader_finished(self))
		SerialportReader_consume(self, *cursor++);

	if (SerialportReader_check_error(self, &error_info))
		exit_info(-2, "%s\n", error_info);

	sleep_milliseconds(100);
	self->time_count += 100;
}

/// return false on timeout
int SerialportReader_read(struct SerialportReader *self, int timeout) {
	if (self->consumer == NULL)
		exit_info(-3, "consumer is necessary\n");

	while (
		self->time_count < timeout &&
		!SerialportReader_finished(self)
	)
		SerialportReader_next(self);

	return self->time_count < timeout;
}

