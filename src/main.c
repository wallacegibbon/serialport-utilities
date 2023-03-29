#include "sp_lines_consumer.h"
#include "sp_util.h"
#include "sp_port.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char **argv) {
	struct SerialportReader reader;
	//struct JsonConsumer consumer;
	struct LinesConsumer consumer;
	int ret;
	const char *cmd;

	cmd = "serialport-lines-reader";

	if (argc != 4)
		exit_info(1, "Usage: %s /dev/ttyACM0 115200 10000\n", cmd);

	SerialportReader_initialize(&reader, 256);

	LinesConsumer_initialize(&consumer);

	reader.consumer = (struct SerialportReaderConsumer **) &consumer;

	sp_open_port(&reader.port, argv[1], atoi(argv[2]));

	ret = SerialportReader_read(&reader, atoi(argv[3]));

	SerialportReader_destroy(&reader);

	if (!ret)
		exit_info(ret, "timeout (reading from serial port)\n");

	return 0;
}

