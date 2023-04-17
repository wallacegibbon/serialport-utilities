#include "sp_lines_consumer.h"
#include "sp_util.h"
#include "sp_port.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char *test_data1 = "CCOVCLCBEGIN\n";
const char *test_data2 = "hello\nworld\nCCOVCLCEN\n";
const char *test_data3 = "hello1\n";
const char *test_data4 = "hello2\n";
const char *test_data5 = "CCOVCLCEND\nXXX\n";

int fake_nonblocking_read(struct SerialportReader *self) {
	static int count = 0;
	switch (count++) {
	case 0:
		strcpy(self->buffer, test_data1);
		return strlen(self->buffer);
	case 5:
		strcpy(self->buffer, test_data2);
		return strlen(self->buffer);
	case 30:
		strcpy(self->buffer, test_data3);
		return strlen(self->buffer);
	case 60:
		strcpy(self->buffer, test_data4);
		return strlen(self->buffer);
	case 90:
		strcpy(self->buffer, test_data5);
		return strlen(self->buffer);
	default:
		return 0;
	}
}

int main(int argc, const char **argv) {
	struct SerialportReader reader;
	struct LinesConsumer lines_consumer;
	int ret;

	SerialportReader_initialize(&reader, 256);
	LinesConsumer_initialize(&lines_consumer);

	reader.consumer = (struct SerialportReaderConsumer **) &lines_consumer;
	reader.nonblocking_read = fake_nonblocking_read;

	//ret = SerialportReader_read(&reader, 5000, 8000);
	ret = SerialportReader_read(&reader, 5000, 10000);
	SerialportReader_destroy(&reader);

	if (!ret)
		exit_info(ret, "timeout (reading from serial port)\n");

	return 0;
}

