/*
 *
 * Copyright (C) 2014 Impex-Sat Gmbh & Co.KG
 * Written by Sandro Cavazzoni <sandro@skanetwork.com>
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>

#include "omb_common.h"
#include "omb_log.h"
#include "omb_input.h"

#define omb_input_max_fds 10

static int omb_input_num_fds = 0;
static int omb_input_fd[omb_input_max_fds];
static int omb_input_last_event_code = -1;
static int omb_input_last_event_count = 0;

int omb_input_open()
{
	while (omb_input_num_fds < omb_input_max_fds)
	{
		char filename[32];
		sprintf(filename, "/dev/input/event%d", omb_input_num_fds);
		if ((omb_input_fd[omb_input_num_fds] = open(filename, O_RDONLY | O_NONBLOCK)) == -1)
			break;
		omb_input_num_fds++;
	}

	if (omb_input_num_fds == 0)
	{
		omb_log(LOG_ERROR, "cannot open input device");
		return OMB_ERROR;
	}

	omb_log(LOG_DEBUG, "input device opened");
	
	return OMB_SUCCESS;
}

int omb_input_get_code()
{
	struct input_event event;
	int i = 0;
	while (i < omb_input_num_fds)
	{
		if (read(omb_input_fd[i], &event, sizeof(event)) == sizeof(event))
		{
			if (!event.code)
				continue;

			if (event.type == EV_KEY && (event.value == 0 || event.value == 2))
				return event.code;
		}
		i++;
	}

	return -1;
}

void omb_input_close()
{
	int i = 0;
	for (; i < omb_input_num_fds; i++)
		close(omb_input_fd[i]);
}
