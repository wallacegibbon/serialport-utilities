#ifndef __SP_READER_H
#define __SP_READER_H

#include <libserialport.h>

enum SerialportReaderState {
	SP_READ_STARTING, SP_READ_NORMAL, SP_READ_STRING, SP_READ_ESCAPE,
	SP_READ_ENDED, SP_READ_ERROR
};

typedef int (*SerialportReaderConsumerConsume)(void *consumer, char ch);
typedef int (*SerialportReaderConsumerFinished)(void *consumer);
typedef int (*SerialportReaderConsumerCheckError)(void *consumer, const char **buf);

struct SerialportReaderConsumer {
	SerialportReaderConsumerConsume consume;
	SerialportReaderConsumerFinished finished;
	SerialportReaderConsumerCheckError check_error;
};

struct SerialportReader {
	struct sp_port *port;
	char *buffer;
	int buffer_size;
	int empty_time_count;
	int total_time_count;
	const char *error_info;
	int (*nonblocking_read)(struct SerialportReader *self);
	struct SerialportReaderConsumer **consumer;
};

void SerialportReader_initialize(struct SerialportReader *self, int buffer_size);
void SerialportReader_destroy(struct SerialportReader *self);

int SerialportReader_read(struct SerialportReader *self, int timeout, int total_timeout);

#endif

