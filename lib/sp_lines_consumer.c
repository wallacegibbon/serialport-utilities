#include "sp_lines_consumer.h"
#include "sp_util.h"
#include <string.h>
#include <stdio.h>

void LinesConsumer_consume(struct LinesConsumer *self, char ch);
int LinesConsumer_finished(struct LinesConsumer *self);
int LinesConsumer_check_error(struct LinesConsumer *self, const char **buf);

static const struct SerialportReaderConsumer consumer_vtable = {
	.consume = (SerialportReaderConsumerConsume) LinesConsumer_consume,
	.finished = (SerialportReaderConsumerFinished) LinesConsumer_finished,
	.check_error = (SerialportReaderConsumerCheckError) LinesConsumer_check_error
};

static const char *starting_string = "CCOVCLCBEGIN";
static const char *ending_string = "CCOVCLCEND";

void LinesConsumer_initialize(struct LinesConsumer *self) {
	self->state = READ_STARTING;
	self->handle_char = putchar;
	self->state_tag_index = 0;
	self->prev_is_newline = 0;
	self->consumer = &consumer_vtable;
}

void LinesConsumer_consume_char_normal(struct LinesConsumer *self, char ch);

void LinesConsumer_consume_char_ending(struct LinesConsumer *self, char ch) {
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

	LinesConsumer_consume_char_normal(self, ch);
}

void LinesConsumer_consume_char_starting(struct LinesConsumer *self, char ch) {
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

void LinesConsumer_consume_char_normal(struct LinesConsumer *self, char ch) {
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
	LinesConsumer_consume_char_ending(self, ch);
}

void LinesConsumer_consume(struct LinesConsumer *self, char ch) {
	switch (self->state) {
	case READ_STARTING:
		LinesConsumer_consume_char_starting(self, ch);
		break;
	case READ_NORMAL:
		LinesConsumer_consume_char_normal(self, ch);
		break;
	case READ_ENDING:
		LinesConsumer_consume_char_ending(self, ch);
		break;
	}
}

int LinesConsumer_finished(struct LinesConsumer *self) {
	return self->state == READ_ENDED;
}

int LinesConsumer_check_error(struct LinesConsumer *self, const char **buf) {
	if (self->state == READ_ERROR) {
		*buf = self->error_info;
		return 1;
	}

	return 0;
}

