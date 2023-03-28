#include "sp_json_reader.h"
#include "sp_lines_reader.h"
#include "sp_util.h"
#include "sp_port.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char **argv) {
	//struct SerialportJsonReader reader;
	struct SerialportLinesReader reader;
	int ret;
	const char *cmd;

	//cmd = "serialport-json-reader";
	cmd = "serialport-lines-reader";

	if (argc != 4)
		exit_info(1, "Usage: %s /dev/ttyACM0 115200 10000\n", cmd);

	/*
	SerialportJsonReader_initialize(&reader);
	sp_open_port(&reader.port, argv[1], atoi(argv[2]));

	ret = SerialportJsonReader_get_json(&reader, atoi(argv[3]));
	SerialportJsonReader_destroy(&reader);
	*/

	SerialportLinesReader_initialize(&reader);
	sp_open_port(&reader.port, argv[1], atoi(argv[2]));

	ret = SerialportLinesReader_get_lines(&reader, atoi(argv[3]));
	SerialportLinesReader_destroy(&reader);

	if (!ret)
		exit_info(ret, "Failed reading from serial port\n");

	return 0;
}

