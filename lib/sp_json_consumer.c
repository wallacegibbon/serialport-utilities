#include "sp_json_consumer.h"
#include "sp_util.h"
#include <stdio.h>

void JsonConsumer_consume(struct JsonConsumer *self, char ch);
int JsonConsumer_finished(struct JsonConsumer *self);
int JsonConsumer_check_error(struct JsonConsumer *self, const char **buf);

static const struct SerialportReaderConsumer consumer_vtable = {
	.consume = (SerialportReaderConsumerConsume) JsonConsumer_consume,
	.finished = (SerialportReaderConsumerFinished) JsonConsumer_finished,
	.check_error = (SerialportReaderConsumerCheckError) JsonConsumer_check_error
};

void JsonConsumer_initialize(struct JsonConsumer *self) {
	self->state = READ_STARTING;
	self->handle_char = putchar;
	self->depth = 0;
	self->consumer = &consumer_vtable;
}

void JsonConsumer_consume_char_escape(struct JsonConsumer *self, char ch) {
	/// nothing to do
}

void JsonConsumer_consume_char_string(struct JsonConsumer *self, char ch) {
	switch (ch) {
	case '"':
		self->state = READ_NORMAL;
		break;
	case '\\':
		self->state = READ_ESCAPE;
		break;
	}
}

void JsonConsumer_consume_char_normal(struct JsonConsumer *self, char ch) {
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

void JsonConsumer_consume_char_starting(struct JsonConsumer *self, char ch) {
	if (ch == '{') {
		self->state = READ_NORMAL;
		JsonConsumer_consume_char_normal(self, ch);
		return;
	}

	self->state = READ_ERROR;
	self->error_info = "invalid start character";
}

void JsonConsumer_consume(struct JsonConsumer *self, char ch) {
	switch (self->state) {
	case READ_STARTING:
		JsonConsumer_consume_char_starting(self, ch);
		break;
	case READ_NORMAL:
		JsonConsumer_consume_char_normal(self, ch);
		break;
	case READ_STRING:
		JsonConsumer_consume_char_string(self, ch);
		break;
	case READ_ESCAPE:
		JsonConsumer_consume_char_escape(self, ch);
		break;
	}

	self->handle_char(ch);
}

int JsonConsumer_finished(struct JsonConsumer *self) {
	return self->state == READ_ENDED;
}

int JsonConsumer_check_error(struct JsonConsumer *self, const char **buf) {
	if (self->state == READ_ERROR) {
		*buf = self->error_info;
		return 1;
	}

	return 0;
}

