#ifndef __SP_JSON_CONSUMER_H
#define __SP_JSON_CONSUMER_H

#include "sp_reader.h"

enum JsonConsumerState {
	READ_STARTING, READ_NORMAL, READ_STRING, READ_ESCAPE, READ_ENDED, READ_ERROR
};

struct JsonConsumer {
	const struct SerialportReaderConsumer *consumer;
	enum JsonConsumerState state;
	const char *error_info;
	int (*handle_char)(int ch);
	int depth;
};

void JsonConsumer_initialize(struct JsonConsumer *self);

#endif

