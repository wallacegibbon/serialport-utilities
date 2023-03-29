#include "sp_json_consumer.h"
#include "sp_util.h"
#include "sp_port.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *test_data1 = "{\"a\":\n";
const char *test_data2 = "{\"b\": \"hello, }}}\"}\n";
const char *test_data3 = "}\nXXX\n";

int fake_nonblocking_read(struct SerialportReader *self) {
	static int count = 0;
	switch (count++) {
	case 0:
		strcpy(self->buffer, test_data1);
		return strlen(self->buffer);
	case 5:
		strcpy(self->buffer, test_data2);
		return strlen(self->buffer);
	case 10:
		strcpy(self->buffer, test_data3);
		return strlen(self->buffer);
	default:
		return 0;
	}
}

int main(int argc, const char **argv) {
	struct SerialportReader reader;
	struct JsonConsumer json_consumer;
	int ret;

	SerialportReader_initialize(&reader, 256);
	JsonConsumer_initialize(&json_consumer);

	reader.consumer = (struct SerialportReaderConsumer **) &json_consumer;
	reader.nonblocking_read = fake_nonblocking_read;

	ret = SerialportReader_read(&reader, 5000);
	SerialportReader_destroy(&reader);

	if (!ret)
		exit_info(ret, "timeout (reading from serial port)\n");

	return 0;
}

