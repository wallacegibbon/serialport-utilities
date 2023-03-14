#include "serialport.h"
#include "common_util.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char **argv) {
	struct SerialportJsonReader reader;
	int ret;

	if (argc != 3)
		exit_info(1, "Usage: serialport-json-reader /dev/ttyACM0 115200\n");

	SerialportJsonReader_initialize(&reader, argv[1], atoi(argv[2]));
	ret = SerialportJsonReader_get_json(&reader, 10000);
	SerialportJsonReader_destroy(&reader);

	if (!ret)
		exit_info(ret, "Failed reading json from serial port\n");

	return 0;
}

