#include "sp_json_reader.h"
#include "sp_util.h"
#include "sp_port.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *test_data1 = "{\"a\":\n";
const char *test_data2 = "{\"b\": \"hello, }}}\"}\n";
const char *test_data3 = "}\nXXX\n";

int fake_nonblocking_read(struct SerialportJsonReader *self) {
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
	struct SerialportJsonReader reader;
	int ret;

	SerialportJsonReader_initialize(&reader);
	reader.nonblocking_read = fake_nonblocking_read;

	ret = SerialportJsonReader_get_json(&reader, 5000);
	SerialportJsonReader_destroy(&reader);

	if (!ret)
		exit_info(ret, "Failed reading from serial port\n");

	return 0;
}

