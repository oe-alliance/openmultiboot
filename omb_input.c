#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>

#include "omb_common.h"
#include "omb_log.h"
#include "omb_input.h"

static int omb_input_fd = 0;
static int omb_input_last_event_code = -1;
static int omb_input_last_event_count = 0;

int omb_input_open()
{
	omb_input_fd = open(OMB_INPUT_DEVICE, O_RDONLY | O_NONBLOCK);
	if (omb_input_fd == -1) {
		omb_log(LOG_ERROR, "cannot open input device");
		return OMB_ERROR;
	}
	
	omb_log(LOG_DEBUG, "input device opened");
	
	return OMB_SUCCESS;
}

int omb_input_get_code()
{
	struct input_event event;
	if (read(omb_input_fd, &event, sizeof(event)) == sizeof(event)) {
		if (!event.code)
			return -1;
		
		if (event.code == omb_input_last_event_code) {
			omb_input_last_event_count++;
			if (omb_input_last_event_count < 3)
				return -1;
		}

		omb_input_last_event_count = 0;
		omb_input_last_event_code = event.code;
		return event.code;
		
	}
	
	omb_input_last_event_count = 0;
	omb_input_last_event_code = -1;
	return -1;
}

void omb_input_close()
{
	close(omb_input_fd);
}
