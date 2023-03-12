#include "serialport.h"
#include "common_util.h"
#include <stdio.h>

int main(int argc, const char **argv) {
	struct SerialportJsonReader reader;
	int ret;

	if (argc != 2)
		exit_info(1, "Usage: serialport-json-reader /dev/ttyACM0\n");

	SerialportJsonReader_initialize(&reader, argv[1]);
	ret = SerialportJsonReader_get_json(&reader, 5000);

	if (!ret)
		exit_info(ret, "Failed reading json from serial port\n");

	SerialportJsonReader_destroy(&reader);

	return 0;
}

