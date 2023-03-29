#include "sp_lines_reader.h"
#include "sp_util.h"
#include "sp_port.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *test_data1 = "CCOVCLCBEGIN\n";
const char *test_data2 = "hello\nworld\nCCOVCLCEN\n";
const char *test_data3 = "CCOVCLCEND\nXXX\n";

int fake_nonblocking_read(struct SerialportLinesReader *self) {
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
	struct SerialportLinesReader reader;
	int ret;

	SerialportLinesReader_initialize(&reader);
	reader.nonblocking_read = fake_nonblocking_read;

	ret = SerialportLinesReader_read(&reader, 5000);
	SerialportLinesReader_destroy(&reader);

	if (!ret)
		exit_info(ret, "Failed reading from serial port\n");

	return 0;
}

