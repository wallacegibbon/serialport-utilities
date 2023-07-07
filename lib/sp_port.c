#include "sp_port.h"
#include "sp_util.h"

void sp_open_port(struct sp_port **port, const char *portname, int baudrate) {
	int ret;

	ret = sp_get_port_by_name(portname, port);
	if (ret < 0)
		exit_info(ret, "failed getting port by name \"%s\": (%d)\n", portname, ret);

	ret = sp_open(*port, SP_MODE_READ_WRITE);
	if (ret < 0)
		exit_info(ret, "failed opening port \"%s\": (%d)\n", portname, ret);

	sp_set_baudrate(*port, baudrate);
	sp_set_bits(*port, 8);
	sp_set_parity(*port, SP_PARITY_NONE);
	sp_set_stopbits(*port, 1);
	sp_set_flowcontrol(*port, SP_FLOWCONTROL_NONE);
}

