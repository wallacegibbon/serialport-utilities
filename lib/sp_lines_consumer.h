#ifndef __SP_LINES_CONSUMER_H
#define __SP_LINES_CONSUMER_H

#include "sp_reader.h"

enum LinesConsumerState {
	READ_STARTING, READ_NORMAL, READ_ENDING, READ_ENDED, READ_ERROR
};

struct LinesConsumer {
	const struct SerialportReaderConsumer *consumer;
	enum LinesConsumerState state;
	const char *error_info;
	int (*handle_char)(int ch);
	/// `state_tag_buffer` stores "CCOVCLCBEGIN" and "CCOVCLCEND"
	char state_tag_buffer[32];
	int state_tag_index;
	int prev_is_newline;
};

void LinesConsumer_initialize(struct LinesConsumer *self);

#endif

